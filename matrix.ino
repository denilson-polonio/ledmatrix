#include "WiFiS3.h"
#include "Arduino_LED_Matrix.h"
#include <EEPROM.h>
#include <time.h>
#if __has_include("config.h")
#include "config.h"
#else
#error "Copy config.example.h to config.h and set your Wi-Fi network"
#endif
#include "page.h"

const int MAX_FRAMES = 100;
const uint32_t MAGIC = 0x4D415831;
const uint32_t BLANK[3] = {0, 0, 0};
const unsigned long PREVIEW_TIMEOUT = 6000;

struct Frame { uint32_t w[3]; uint16_t d; };
struct Sched { uint32_t magic; uint8_t en; uint16_t on; uint16_t off; };
struct Head  { uint32_t magic; uint16_t n; };

const int ADDR_SCHED  = 0;
const int ADDR_HEAD   = 16;
const int ADDR_FRAMES = 32;

ArduinoLEDMatrix matrix;
WiFiServer server(80);

Frame frames[MAX_FRAMES];
uint16_t nFrames = 0;
char hexBuf[MAX_FRAMES * 28 + 1];

Frame prev[MAX_FRAMES];
uint16_t nPrev = 0;
unsigned long prevAt = 0;
bool wasPreview = false;
Sched sched;

bool shown = false;
uint16_t idx = 0;
unsigned long lastChange = 0;

bool timeOk = false;
time_t baseEpoch = 0;
unsigned long baseMs = 0, lastTry = 0;

bool isDST(time_t utc) {
  struct tm *t = gmtime(&utc);
  int y = t->tm_year + 1900;
  int m = t->tm_mon + 1;
  if (m < 3 || m > 10) return false;
  if (m > 3 && m < 10) return true;
  int lastSun = (m == 3) ? 31 - ((5 * y / 4 + 4) % 7) : 31 - ((5 * y / 4 + 1) % 7);
  bool afterSwitch = (t->tm_mday > lastSun) || (t->tm_mday == lastSun && t->tm_hour >= 1);
  return (m == 3) ? afterSwitch : !afterSwitch;
}

time_t utcNow() {
  return baseEpoch + (time_t)((millis() - baseMs) / 1000UL);
}

int localMinutes() {
  if (!timeOk) return -1;
  time_t utc = utcNow();
  time_t loc = utc + (isDST(utc) ? OFFSET_SUMMER : OFFSET_WINTER);
  struct tm *t = gmtime(&loc);
  return t->tm_hour * 60 + t->tm_min;
}

bool shouldBeOn() {
  if (!sched.en) return true;
  int m = localMinutes();
  if (m < 0) return true;
  if (sched.on == sched.off) return true;
  if (sched.on < sched.off) return m >= sched.on && m < sched.off;
  return m >= sched.on || m < sched.off;
}

void syncTime() {
  if (WiFi.status() != WL_CONNECTED) return;
  unsigned long t = WiFi.getTime();
  if (t > 1600000000UL) {
    baseEpoch = (time_t)t;
    baseMs = millis();
    timeOk = true;
  }
}

void saveSched() {
  EEPROM.put(ADDR_SCHED, sched);
}

void saveFrames() {
  for (uint16_t i = 0; i < nFrames; i++) EEPROM.put(ADDR_FRAMES + i * sizeof(Frame), frames[i]);
  Head h = { MAGIC, nFrames };
  EEPROM.put(ADDR_HEAD, h);
}

void loadAll() {
  EEPROM.get(ADDR_SCHED, sched);
  if (sched.magic != MAGIC || sched.on >= 1440 || sched.off >= 1440) {
    sched.magic = MAGIC; sched.en = 0; sched.on = 8 * 60; sched.off = 23 * 60;
  }
  Head h;
  EEPROM.get(ADDR_HEAD, h);
  nFrames = 0;
  if (h.magic == MAGIC && h.n <= MAX_FRAMES) {
    for (uint16_t i = 0; i < h.n; i++) EEPROM.get(ADDR_FRAMES + i * sizeof(Frame), frames[i]);
    nFrames = h.n;
  }
}

void restartAnimation() {
  matrix.loadFrame(BLANK);
  shown = false;
}

bool previewOn() {
  return nPrev > 0 && millis() - prevAt < PREVIEW_TIMEOUT;
}

void updateMatrix() {
  bool pv = previewOn();
  if (pv != wasPreview) {
    wasPreview = pv;
    restartAnimation();
  }
  const Frame *src = pv ? prev : frames;
  uint16_t n = pv ? nPrev : nFrames;
  if (n == 0 || (!pv && !shouldBeOn())) {
    if (shown) restartAnimation();
    return;
  }
  unsigned long now = millis();
  if (!shown || idx >= n) {
    idx = 0;
    matrix.loadFrame(src[0].w);
    lastChange = now;
    shown = true;
  } else if (n > 1 && now - lastChange >= src[idx].d) {
    idx = (idx + 1) % n;
    matrix.loadFrame(src[idx].w);
    lastChange = now;
  }
}

uint32_t hexVal(const char *s, int n) {
  uint32_t v = 0;
  for (int i = 0; i < n; i++) {
    char c = s[i];
    v = v * 16 + (c <= '9' ? c - '0' : (c | 32) - 'a' + 10);
  }
  return v;
}

int getParam(const String &path, const char *key) {
  String k = String(key) + "=";
  int p = path.indexOf(k);
  if (p < 0) return -1;
  return path.substring(p + k.length()).toInt();
}

void sendResponse(WiFiClient &c, int code, const char *type, const char *body) {
  c.print("HTTP/1.1 ");
  c.print(code);
  c.println(code == 200 ? " OK" : " Bad Request");
  size_t len = strlen(body);
  c.print("Content-Type: ");
  c.println(type);
  c.print("Content-Length: ");
  c.println(len);
  c.println("Cache-Control: no-store");
  c.println("Connection: close");
  c.println();
  for (size_t i = 0; i < len; i += 512) c.write((const uint8_t *)body + i, min((size_t)512, len - i));
}

void handleClient() {
  WiFiClient c = server.available();
  if (!c) return;
  c.setTimeout(1000);

  String req = c.readStringUntil('\n');
  if (!req.startsWith("GET ") && !req.startsWith("POST ")) {
    c.stop();
    return;
  }
  int len = 0;
  while (c.connected()) {
    String line = c.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) break;
    line.toLowerCase();
    if (line.startsWith("content-length:")) len = line.substring(15).toInt();
  }
  int sp1 = req.indexOf(' ');
  int sp2 = req.indexOf(' ', sp1 + 1);
  String method = req.substring(0, sp1);
  String path = req.substring(sp1 + 1, sp2);
  bool save = false;

  if (method == "POST" && (path == "/upload" || path == "/preview")) {
    bool isPrev = path == "/preview";
    Frame *dst = isPrev ? prev : frames;
    int got = c.readBytes(hexBuf, min(len, (int)sizeof(hexBuf) - 1));
    int n = got / 28;
    if (got != len || got % 28 != 0 || n < 1 || n > MAX_FRAMES) {
      sendResponse(c, 400, "text/plain", "invalid data");
    } else {
      for (int i = 0; i < n; i++) {
        const char *p = hexBuf + i * 28;
        for (int k = 0; k < 3; k++) dst[i].w[k] = hexVal(p + k * 8, 8);
        dst[i].d = (uint16_t)hexVal(p + 24, 4);
        if (dst[i].d < 20) dst[i].d = 20;
      }
      if (isPrev) {
        nPrev = n;
        prevAt = millis();
      } else {
        nFrames = n;
        save = true;
      }
      restartAnimation();
      sendResponse(c, 200, "text/plain", "ok");
    }
  } else if (path == "/ping") {
    prevAt = millis();
    sendResponse(c, 200, "text/plain", "ok");
  } else if (path == "/stop") {
    nPrev = 0;
    sendResponse(c, 200, "text/plain", "ok");
  } else if (path.startsWith("/sched")) {
    int en = getParam(path, "en"), on = getParam(path, "on"), off = getParam(path, "off");
    if (on < 0 || on >= 1440 || off < 0 || off >= 1440) {
      sendResponse(c, 400, "text/plain", "invalid time");
    } else {
      sched.en = en == 1;
      sched.on = on;
      sched.off = off;
      saveSched();
      restartAnimation();
      sendResponse(c, 200, "text/plain", "ok");
    }
  } else if (path == "/frames") {
    for (uint16_t i = 0; i < nFrames; i++)
      snprintf(hexBuf + i * 28, 29, "%08lx%08lx%08lx%04x", (unsigned long)frames[i].w[0],
               (unsigned long)frames[i].w[1], (unsigned long)frames[i].w[2], frames[i].d);
    hexBuf[nFrames * 28] = 0;
    sendResponse(c, 200, "text/plain", hexBuf);
  } else if (path == "/state") {
    char s[32];
    snprintf(s, sizeof(s), "%d,%d,%d,%d", sched.en, sched.on, sched.off, localMinutes());
    sendResponse(c, 200, "text/plain", s);
  } else {
    sendResponse(c, 200, "text/html; charset=utf-8", PAGE);
  }
  delay(20);
  c.stop();
  if (save) saveFrames();
}

void connectWiFi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found");
    return;
  }
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.config(STATIC_IP, DNS, GATEWAY, SUBNET);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) delay(500);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, retrying later");
    return;
  }
  for (int i = 0; i < 20 && WiFi.localIP() == IPAddress(0, 0, 0, 0); i++) delay(250);
  Serial.print("Open in your browser: http://");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  loadAll();
  connectWiFi();
  server.begin();
  syncTime();
  lastTry = millis();
}

void loop() {
  static unsigned long lastWiFi = 0;
  if (WiFi.status() != WL_CONNECTED && millis() - lastWiFi > 30000UL) {
    lastWiFi = millis();
    connectWiFi();
    server.begin();
  }
  unsigned long every = timeOk ? 6UL * 3600UL * 1000UL : 10000UL;
  if (millis() - lastTry > every) {
    lastTry = millis();
    syncTime();
  }
  handleClient();
  updateMatrix();
}

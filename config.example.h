#pragma once

const char *WIFI_SSID = "your-network";
const char *WIFI_PASS = "your-password";

const IPAddress STATIC_IP(192, 168, 1, 50);
const IPAddress GATEWAY(192, 168, 1, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS(192, 168, 1, 1);

const int OFFSET_WINTER = 3600;
const int OFFSET_SUMMER = 7200;

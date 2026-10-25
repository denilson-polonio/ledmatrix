#pragma once

const char PAGE[] = R"rawliteral(<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<link rel="icon" href="data:,">
<title>LED Matrix</title>
<style>
body{font-family:system-ui,sans-serif;max-width:460px;margin:24px auto;padding:0 16px;background:#111;color:#eee}
h1{font-size:1.2rem;margin-top:28px}
canvas{image-rendering:pixelated;background:#000;border-radius:8px}
#ed{width:100%;touch-action:none;cursor:crosshair;margin:8px 0}
#pv{width:120px}
input,button,select{font-size:1rem;padding:10px;border-radius:8px;border:0;margin:4px 0}
button{background:#e33;color:#fff;width:100%}
button.t,.row button{background:#333}
.row{display:flex;flex-wrap:wrap;gap:4px;align-items:center}
.row button{width:auto;flex:1;padding:8px;min-width:44px}
#st{display:flex;gap:4px;overflow-x:auto;padding:4px 0;position:relative}
#st canvas{width:36px;flex:none;cursor:pointer;border:2px solid transparent;border-radius:4px}
#st .sel{border-color:#e33}
#du{width:90px}
.gal{display:flex;flex-wrap:wrap;gap:6px}
.gal button{width:auto;background:#333;padding:6px;margin:0;font-size:.75rem;display:flex;flex-direction:column;align-items:center;gap:4px}
.gal canvas{width:60px;border-radius:4px}
button.t.on{background:#1a7f37}
select,#tx{width:100%;box-sizing:border-box}
label{display:block;margin:8px 0}
#bar{position:sticky;bottom:0;background:#111;padding:6px 0 10px;border-top:1px solid #333}
#m{min-height:1.4em;color:#9f9}
.file{position:relative;overflow:hidden;background:#333;color:#fff;padding:10px;border-radius:8px;text-align:center;cursor:pointer}
.file input{position:absolute;inset:0;opacity:0;cursor:pointer}
footer{text-align:center;color:#888;font-size:.9rem;margin:40px 0 24px}
footer a{color:#e33;text-decoration:none}
</style></head><body>
<h1>LED Matrix</h1>
<canvas id="ed" width="360" height="240"></canvas>
<div class="row">
<button data-op="clr">Clear</button><button data-op="fill">Fill</button><button data-op="inv">Invert</button><button id="un">Undo</button>
</div>
<div class="row">
<button data-op="l" title="Move left">&larr;</button><button data-op="u" title="Move up">&uarr;</button><button data-op="d" title="Move down">&darr;</button><button data-op="r" title="Move right">&rarr;</button><button data-op="mx" title="Mirror">&harr;</button><button data-op="my" title="Flip">&#8597;</button><button data-op="big" title="Zoom in">+</button><button data-op="small" title="Zoom out">&minus;</button>
</div>
<div class="row">
<button id="pr">&#9664;</button><span id="fi"></span><button id="nx">&#9654;</button>
Duration <input type="number" id="du" min="20" max="65535" step="10"> ms
</div>
<div class="row">
<button id="ad">+ New</button><button id="dp">Duplicate</button><button id="dl">Delete</button><button id="al">Same duration for all</button><button id="ra">Delete all</button>
</div>
<div id="st"></div>
<div class="row"><canvas id="pv" width="120" height="80"></canvas><span id="info"></span></div>
<button id="lv" class="t">Preview on matrix: OFF</button>
<h1>Shapes</h1>
<div id="sh" class="gal"></div>
<h1>Ready-made animations</h1>
<div id="an" class="gal"></div>
<h1>Animated text</h1>
<input id="tx" placeholder="Type here, e.g. HELLO ❤️" maxlength="60" autocomplete="off">
<div id="emo" class="row"></div>
<select id="tm">
<option value="left">Scroll left</option><option value="right">Scroll right</option>
<option value="letters">One letter at a time</option><option value="still">Still</option>
</select>
<select id="te"><option value="s">Small emoji (same size as letters)</option><option value="b">Big emoji (full height)</option></select>
<label><input type="checkbox" id="ta" checked> Animate emoji (beating heart, blinking eyes...)</label>
<label>Speed <input type="range" id="ts" min="1" max="10" value="6"> <span id="tsv"></span></label>
<small>The animation updates as you type. Emoji without a drawing become their silhouette.</small>
<h1>Import</h1>
<label class="file">Choose a GIF, SVG or image<input type="file" id="f" accept="image/gif,image/svg+xml,.svg,image/png,image/jpeg,image/webp"></label>
<label>Thickness (SVG and images) <input type="range" id="th" min="10" max="95" value="65"></label>
<button id="rb" class="t">Reload from matrix</button>
<h1>Effects</h1>
<select id="fx">
<option value="beat">Heartbeat</option><option value="pulse">Breathe</option><option value="blink">Blink</option>
<option value="spin">Spin</option><option value="left">Scroll left</option><option value="right">Scroll right</option>
<option value="up">Scroll up</option><option value="down">Scroll down</option><option value="bounce">Bounce</option>
<option value="shake">Shake</option><option value="sparkle">Sparkle</option><option value="wipe">Wipe</option>
<option value="dissolve">Dissolve</option>
</select>
<label>Time per frame <input type="range" id="sp" min="30" max="500" value="100" step="10"> <span id="spv"></span></label>
<label><input type="checkbox" id="add"> Append (instead of replacing)</label>
<button id="ap" class="t">Apply effect to current frame</button>
<h1>Schedule</h1>
<label><input type="checkbox" id="en"> Use schedule</label>
<label>Turn on at <input type="time" id="on"></label>
<label>Turn off at <input type="time" id="off"></label>
<p><small id="now"></small></p>
<div id="bar"><div id="m">Loading...</div><button id="send">Save</button></div>
<footer>Developed by <a href="https://denilson.it" target="_blank" rel="noopener">Denilson</a> with ❤️</footer>
<script>
window.onerror=function(m){document.getElementById('m').textContent='Error: '+m};
if(!Array.prototype.flatMap)Array.prototype.flatMap=function(f){return [].concat.apply([],this.map(f))};
</script>
<script>
const W=12,H=8,N=W*H,MAX=100,$=id=>document.getElementById(id);
const msg=t=>$('m').textContent=t;
const blank=()=>Array(N).fill(0);
const seq=(n,f)=>Array.from({length:n},(_,i)=>f(i));
const clampD=d=>Math.min(65535,Math.max(20,Math.round(d)||100));
const SAVE_HINT='. Press "Save" to keep it';
let fr=[{bits:blank(),d:100}],cur=0,hist=[],lastImg=null;
let live=false,liveT=0,liveSent='',busy=false,txOn=false,txBase=[];

function drawBits(g,bits,s,gap){
  g.fillStyle='#000';g.fillRect(0,0,W*s,H*s);
  bits.forEach((b,k)=>{g.fillStyle=b?'#f33':'#2a0d0d';g.fillRect(k%W*s+gap,((k/W)|0)*s+gap,s-2*gap,s-2*gap)});
}
function thumb(i){
  const c=document.createElement('canvas');c.width=W*3;c.height=H*3;
  drawBits(c.getContext('2d'),fr[i].bits,3,0);
  if(i===cur)c.className='sel';
  c.onclick=()=>{cur=i;render()};
  return c;
}
function render(quick){
  cur=Math.max(0,Math.min(cur,fr.length-1));
  queueLive();
  drawBits($('ed').getContext('2d'),fr[cur].bits,30,2);
  const st=$('st');
  if(quick){st.replaceChild(thumb(cur),st.children[cur]);return}
  $('fi').textContent=' Frame '+(cur+1)+'/'+fr.length+' ';
  $('du').value=fr[cur].d;
  st.innerHTML='';fr.forEach((_,i)=>st.appendChild(thumb(i)));
  st.scrollLeft=st.children[cur].offsetLeft-st.clientWidth/2;
  $('info').textContent=fr.length+' frames, '+(fr.reduce((a,f)=>a+f.d,0)/1000).toFixed(1)+' s';
}
let pi=0;
(function play(){
  pi%=fr.length;
  drawBits($('pv').getContext('2d'),fr[pi].bits,10,1);
  setTimeout(play,fr[pi++].d);
})();

function snap(){hist.push(JSON.stringify([fr,cur]));if(hist.length>40)hist.shift();txOn=false}
function undo(){if(!hist.length)return;[fr,cur]=JSON.parse(hist.pop());txOn=false;render()}
$('un').onclick=undo;
document.onkeydown=e=>{if((e.ctrlKey||e.metaKey)&&e.key==='z'&&e.target.tagName!=='INPUT'){e.preventDefault();undo()}};

const ed=$('ed');let paint=-1;
function cell(e){
  const r=ed.getBoundingClientRect(),x=Math.floor((e.clientX-r.left)/r.width*W),y=Math.floor((e.clientY-r.top)/r.height*H);
  return x>=0&&x<W&&y>=0&&y<H?y*W+x:-1;
}
ed.onpointerdown=e=>{
  const k=cell(e);if(k<0)return;
  e.preventDefault();
  snap();paint=fr[cur].bits[k]?0:1;
  try{ed.setPointerCapture(e.pointerId)}catch(err){}
  ed.onpointermove(e);
};
ed.onpointermove=e=>{
  if(paint<0)return;
  const k=cell(e);
  if(k>=0&&fr[cur].bits[k]!==paint){fr[cur].bits[k]=paint;render(true)}
};
ed.onpointerup=ed.onpointercancel=()=>paint=-1;

const move=(b,dx,dy,wrap)=>b.map((_,k)=>{
  let x=k%W-dx,y=((k/W)|0)-dy;
  if(wrap){x=(x%W+W)%W;y=(y%H+H)%H}
  return x<0||x>=W||y<0||y>=H?0:b[y*W+x];
});
function xf(b,s,a=0){
  let x0=W,x1=-1,y0=H,y1=-1;
  b.forEach((v,k)=>{if(v){const x=k%W,y=(k/W)|0;x0=Math.min(x0,x);x1=Math.max(x1,x);y0=Math.min(y0,y);y1=Math.max(y1,y)}});
  if(x1<0)return b.slice();
  const cx=(x0+x1+1)/2,cy=(y0+y1+1)/2,co=Math.cos(a),si=Math.sin(a);
  return b.map((_,k)=>{
    let on=0;
    for(let i=0;i<3;i++)for(let j=0;j<3;j++){
      const px=k%W+(i+.5)/3-cx,py=((k/W)|0)+(j+.5)/3-cy;
      const sx=Math.floor((px*co+py*si)/s+cx),sy=Math.floor((-px*si+py*co)/s+cy);
      if(sx>=0&&sx<W&&sy>=0&&sy<H&&b[sy*W+sx])on++;
    }
    return on>=4?1:0;
  });
}
const OPS={
  clr:()=>blank(),fill:()=>Array(N).fill(1),inv:b=>b.map(v=>1-v),
  l:b=>move(b,-1,0,1),r:b=>move(b,1,0,1),u:b=>move(b,0,-1,1),d:b=>move(b,0,1,1),
  mx:b=>b.map((_,k)=>b[((k/W)|0)*W+W-1-k%W]),my:b=>b.map((_,k)=>b[(H-1-((k/W)|0))*W+k%W]),
  big:b=>xf(b,1.25),small:b=>xf(b,.8)
};
document.querySelectorAll('[data-op]').forEach(el=>el.onclick=()=>{snap();fr[cur].bits=OPS[el.dataset.op](fr[cur].bits);render()});

const SHAPES={
  'Heart':['..###..###..','.##########.','.##########.','.##########.','..########..','...######...','....####....','.....##.....'],
  'Small heart':['............','...##..##...','..########..','..########..','...######...','....####....','.....##.....','............'],
  'Smile':['...######...','..#......#..','.#..#..#..#.','.#........#.','.#.#....#.#.','.#..####..#.','..#......#..','...######...'],
  'Sad':['...######...','..#......#..','.#..#..#..#.','.#........#.','.#..####..#.','.#.#....#.#.','..#......#..','...######...'],
  'Sunglasses':['...######...','..#......#..','.##########.','.#.##..##.#.','.#........#.','.#.#....#.#.','..#.####.#..','...######...'],
  'Star':['.....##.....','....####....','############','.##########.','...######...','...######...','..###..###..','.##......##.'],
  'Sun':['.#...##...#.','..#......#..','....####....','##.######.##','##.######.##','....####....','..#......#..','.#...##...#.'],
  'Moon':['.....###....','...####..#..','..####......','..###.......','..###...#...','..####......','...####...#.','.....###....'],
  'Flower':['....####....','...#.##.#...','...######...','...#.##.#...','....####....','.##..##..##.','..##.##.##..','.....##.....'],
  'Tree':['.....##.....','....####....','...######...','....####....','..########..','.##########.','.....##.....','.....##.....'],
  'Cat':['.#........#.','.##......##.','.##########.','.##.####.##.','.##########.','.####..####.','..########..','...######...'],
  'Fish':['............','....####....','..#######..#','.##.########','.###########','..#######..#','....####....','............'],
  'Ghost':['....####....','..########..','.#..####..#.','.#..####..#.','.##########.','.##########.','.##########.','.#.##..##.#.'],
  'Invader':['...#.....#..','....#...#...','...#######..','..##.###.##.','.###########','.#.#######.#','.#.#.....#.#','....##.##...'],
  'Pac-man':['...#####....','..#######...','.#####......','.####.......','.####.......','.#####......','..#######...','...#####....'],
  'Fire':['.....#......','.....##.....','....###..#..','...####.##..','..########..','..###..###..','..##....##..','...######...'],
  'Drop':['.....##.....','.....##.....','....####....','...######...','..########..','..########..','...######...','....####....'],
  'Bolt':['......###...','.....###....','....###.....','...#######..','.....###....','....###.....','...##.......','..#.........'],
  'Note':['....######..','....######..','....#....#..','....#....#..','....#....#..','..###..###..','.####.####..','..##...##...'],
  'House':['.....##.....','....####....','...######...','..########..','...#....#...','...#.##.#...','...#.##.#...','...######...'],
  'Arrow':['......#.....','......##....','......###...','.#########..','.#########..','......###...','......##....','......#.....'],
  'Check':['............','.........##.','........##..','.......##...','.##...##....','..##.##.....','...###......','....#.......'],
  'Circle':['....####....','..##....##..','.#........#.','.#........#.','.#........#.','.#........#.','..##....##..','....####....'],
  'Border':['############','#..........#','#..........#','#..........#','#..........#','#..........#','#..........#','############']
};
const shapeBits=r=>r.join('').split('').map(c=>c==='#'?1:0);
for(const[name,rows]of Object.entries(SHAPES)){
  const b=document.createElement('button'),c=document.createElement('canvas');
  c.width=W*4;c.height=H*4;drawBits(c.getContext('2d'),shapeBits(rows),4,0);
  b.append(c,name);
  b.onclick=()=>{snap();fr[cur].bits=shapeBits(rows);lastImg=null;render();msg('"'+name+'" in frame '+(cur+1)+': edit it or apply an effect')};
  $('sh').appendChild(b);
}

$('pr').onclick=()=>{cur=(cur-1+fr.length)%fr.length;render()};
$('nx').onclick=()=>{cur=(cur+1)%fr.length;render()};
function insert(bits){
  if(fr.length>=MAX){msg('Max '+MAX+' frames');return}
  snap();fr.splice(cur+1,0,{bits,d:fr[cur].d});cur++;render();
}
$('ad').onclick=()=>insert(blank());
$('dp').onclick=()=>insert(fr[cur].bits.slice());
$('dl').onclick=()=>{snap();if(fr.length>1)fr.splice(cur,1);else fr[0].bits=blank();render()};
$('du').onchange=()=>{snap();fr[cur].d=clampD($('du').value);render()};
$('al').onclick=()=>{snap();const d=clampD($('du').value);fr.forEach(f=>f.d=d);render()};
$('ra').onclick=()=>{snap();fr=[{bits:blank(),d:100}];cur=0;lastImg=null;render();msg('All frames deleted (Undo restores them)'+SAVE_HINT)};

function shuffle(a){for(let i=a.length-1;i>0;i--){const j=Math.random()*(i+1)|0;[a[i],a[j]]=[a[j],a[i]]}return a}
const part=(on,n)=>{const f=blank();on.slice(0,n).forEach(k=>f[k]=1);return f};
const FX={
  beat:(b,t)=>{const s=xf(b,.75);return[[b,t],[s,t],[b,t],[s,t*5]]},
  pulse:(b,t)=>[1,.85,.7,.55,.4,.55,.7,.85].map(s=>[xf(b,s),t]),
  blink:(b,t)=>[[b,t*3],[blank(),t*3]],
  spin:(b,t)=>seq(12,i=>[xf(b,1,i*Math.PI/6),t]),
  left:(b,t)=>seq(W,i=>[move(b,-i,0,1),t]),
  right:(b,t)=>seq(W,i=>[move(b,i,0,1),t]),
  up:(b,t)=>seq(H,i=>[move(b,0,-i,1),t]),
  down:(b,t)=>seq(H,i=>[move(b,0,i,1),t]),
  bounce:(b,t)=>[0,-1,-2,-2,-1,0].map(y=>[move(b,0,y),t]),
  shake:(b,t)=>[[move(b,-1,0),t],[move(b,1,0),t],[move(b,-1,0),t],[move(b,1,0),t],[b,t*8]],
  sparkle:(b,t)=>seq(10,()=>[b.map(v=>v&&Math.random()>.3?1:0),t]),
  wipe:(b,t)=>[...seq(W,i=>[b.map((v,k)=>k%W<=i?v:0),i===W-1?t*8:t]),...seq(W,i=>[b.map((v,k)=>k%W>i?v:0),t])],
  dissolve:(b,t)=>{
    const on=shuffle(b.map((v,k)=>v?k:-1).filter(k=>k>=0)),n=on.length;
    return[...seq(8,i=>[part(on,Math.round(n*(i+1)/8)),i===7?t*8:t]),...seq(8,i=>[part(on,Math.round(n*(7-i)/8)),t])];
  }
};
const FONT={A:'25755',B:'65656',C:'34443',D:'65556',E:'74647',F:'74644',G:'34553',H:'55755',I:'72227',J:'11152',
K:'55655',L:'44447',M:'57755',N:'65555',O:'25552',P:'65644',Q:'25563',R:'65655',S:'34216',T:'72222',U:'55557',
V:'55552',W:'55775',X:'55255',Y:'55222',Z:'71247',0:'75557',1:'26227',2:'61247',3:'61216',4:'55711',5:'74616',
6:'34757',7:'71222',8:'75757',9:'75716',' ':'00000','!':'22202','?':'61202','.':'00002',',':'00012','-':'00700',
':':'02020','+':'02720','*':'57720','♥':'57720'};
const EMO={};
for(const[name,list]of Object.entries({'Heart':'❤💖💗💓💕😍','Smile':'😀😃😄😁😊🙂☺😉','Sad':'😢😭☹🙁😞😔','Sunglasses':'😎',
  'Star':'⭐🌟✨','Sun':'☀🌞','Moon':'🌙🌛🌜','Flower':'🌸🌼🌻🌷🌺','Tree':'🎄🌲🌳','Cat':'🐱🐈😺','Fish':'🐟🐠',
  'Ghost':'👻','Invader':'👾','Pac-man':'🟡','Fire':'🔥','Drop':'💧','Bolt':'⚡','Note':'🎵🎶','House':'🏠🏡',
  'Arrow':'➡','Check':'✅✔'}))
  for(const e of Array.from(list))EMO[e]=name;
const MINI={
  'Heart':[['.#.#.','#####','#####','.###.','..#..'],['.....','.#.#.','.###.','..#..','.....']],
  'Smile':[['.#.#.','.#.#.','.....','#...#','.###.'],['.....','.#.#.','.....','#...#','.###.']],
  'Sad':[['.#.#.','.#.#.','.....','.###.','#...#'],['.#.#.','.#.#.','.#...','.###.','#...#']],
  'Sunglasses':[['#####','##.##','.....','#...#','.###.'],['#####','##.##','.....','.....','.###.']],
  'Star':[['..#..','#####','.###.','.#.#.','#...#'],['.....','..#..','.###.','..#..','.....']],
  'Sun':[['#.#.#','.###.','#####','.###.','#.#.#'],['..#..','.###.','#####','.###.','..#..']],
  'Moon':[['.##..','##..#','##...','##...','.##..'],['.##..','##...','##...','##...','.##..']],
  'Flower':[['.###.','##.##','.###.','..#..','.##..'],['.###.','#####','.###.','..#..','..##.']],
  'Tree':[['..#..','.###.','#####','..#..','..#..'],['..#..','.#.#.','##.##','..#..','..#..']],
  'Cat':[['#...#','#####','#.#.#','#####','.###.'],['#...#','#####','#####','#####','.###.']],
  'Fish':[['.....','.##.#','#.###','.##.#','.....'],['.....','.##..','#.###','.##..','.....']],
  'Ghost':[['.###.','#.#.#','#####','#####','#.#.#'],['.###.','#.#.#','#####','#####','.#.#.']],
  'Invader':[['.#.#.','#####','#.#.#','#####','#.#.#'],['.#.#.','#####','#.#.#','#####','.#.#.']],
  'Pac-man':[['.###.','###..','##...','###..','.###.'],['.###.','#####','#####','#####','.###.']],
  'Fire':[['..#..','.##.#','.####','#####','.###.'],['.#...','..##.','#.##.','#####','.###.']],
  'Drop':[['..#..','.###.','#####','#####','.###.'],['.....','..#..','.###.','#####','.###.']],
  'Bolt':[['...#.','..#..','.###.','..#..','.#...'],['.....','.....','.....','.....','.....']],
  'Note':[['..##.','..#.#','..#..','.##..','.##..'],['..##.','..#.#','..#..','###..','##...']],
  'House':[['..#..','.###.','#####','.#.#.','.#.#.'],['..#..','.###.','#####','.###.','.#.#.']],
  'Arrow':[['..#..','...#.','#####','...#.','..#..'],['..#..','...#.','.####','...#.','..#..']],
  'Check':[['.....','....#','...#.','#.#..','.#...']]
};
const empty=()=>Array(H).fill(0);
const colsAll=b=>seq(W,x=>seq(H,y=>b[y*W+x]));
const small=rows=>seq(rows[0].length,x=>seq(H,y=>y>=1&&y<=5&&rows[y-1][x]==='#'?1:0));
function trim(ph){
  const on=i=>ph.some(c=>c[i].some(v=>v));
  let a=0,z=ph[0].length-1;
  while(a<z&&!on(a))a++;
  while(z>a&&!on(z))z--;
  return ph.map(c=>c.slice(a,z+1));
}
function silhouette(e,tw,th){
  const c=document.createElement('canvas');c.width=c.height=64;
  const g=c.getContext('2d');g.font='56px sans-serif';g.textAlign='center';g.textBaseline='middle';g.fillText(e,32,36);
  const d=g.getImageData(0,0,64,64).data,px=new Uint8Array(64*64);
  for(let i=0;i<px.length;i++)px[i]=d[i*4+3]>=128?1:0;
  return toBits(px,64,64,tw,th);
}
function emojiGlyph(k,ch,big,anim){
  const name=EMO[k];
  if(big){
    const b=name?S(name):silhouette(ch,W,H);
    return trim(anim?[colsAll(b),colsAll(xf(b,.75))]:[colsAll(b)]);
  }
  if(name)return trim(MINI[name].slice(0,anim?2:1).map(small));
  const b=silhouette(ch,5,5);
  return trim([small(seq(5,y=>seq(5,x=>b[y*5+x]?'#':'.').join('')))]);
}
let PICTO;
try{PICTO=new RegExp('\\p{Extended_Pictographic}','u')}catch(err){PICTO=/[\u2600-\u27bf]|[\ud83c-\ud83e][\udc00-\udfff]/}
function glyphs(s,big,anim){
  const parts=typeof Intl!=='undefined'&&Intl.Segmenter?[...new Intl.Segmenter().segment(s)].map(x=>x.segment):Array.from(s);
  return parts.map(ch=>{
    const k=ch.replace(/\uFE0F/g,''),c=k.normalize('NFD').replace(/[\u0300-\u036f]/g,'').toUpperCase();
    if(c===' ')return[[empty(),empty()]];
    if(!FONT[c]&&(EMO[k]||PICTO.test(k)))return emojiGlyph(k,ch,big,anim);
    const g=(FONT[c]||FONT['?']).split('').map(Number);
    return trim([small(g.map(r=>[4,2,1].map(m=>r&m?'#':'.').join('')))]);
  });
}
const view=(cols,o)=>seq(N,k=>{const c=cols[o+k%W];return c?c[(k/W)|0]:0});
const centered=cols=>view([...seq(Math.max(0,(W-cols.length)>>1),empty),...cols],0);
function textFx(s,t,mode='left',big=false,anim=true){
  const gl=glyphs(s,big,anim),P=gl.some(g=>g.length>1)?2:1;
  if(mode==='letters')return gl.flatMap(g=>[...(g.length>1?seq(4,j=>[centered(g[j%2]),t*1.5]):[[centered(g[0]),t*6]]),[blank(),t]]);
  const strips=seq(P,p=>gl.flatMap((g,i)=>{const c=g[p%g.length];return i?[empty(),...c]:c}));
  if(mode==='still')return strips[0].length>W?null:seq(P,p=>[centered(strips[p]),P>1?400:1000]);
  const pads=strips.map(c=>[...seq(W,empty),...c,empty(),...seq(W-1,empty)]);
  const n=pads[0].length-W+1,step=Math.max(1,Math.round(300/t));
  return seq(n,i=>[view(pads[Math.floor(i/step)%P],mode==='right'?n-1-i:i),t]);
}
$('sp').oninput=()=>$('spv').textContent=$('sp').value+' ms';
$('sp').oninput();
function useFrames(list,what,hint=''){
  const add=$('add').checked,nf=list.map(([b,d])=>({bits:b.slice(),d:clampD(d)}));
  const out=add?fr.concat(nf):nf;
  if(out.length>MAX){msg('Too many frames ('+out.length+'), max '+MAX+hint);return}
  snap();fr=out;cur=add?fr.length-nf.length:0;render();
  msg(what+': '+nf.length+' frames'+SAVE_HINT);
}
$('ap').onclick=()=>useFrames(FX[$('fx').value](fr[cur].bits,+$('sp').value),'Effect applied');
const textMs=()=>220-$('ts').value*18;
function liveText(){
  $('tsv').textContent=textMs()+' ms';
  const s=$('tx').value;
  if(!s.trim())return;
  const big=$('te').value==='b',anim=$('ta').checked;
  let nf=textFx(s,textMs(),$('tm').value,big,anim),note='';
  if(!nf){nf=textFx(s,textMs(),'left',big,anim);note=' (too wide to stay still, so it scrolls)'}
  if(!txOn){txBase=fr;snap();txOn=true}
  const add=$('add').checked,list=nf.map(([b,d])=>({bits:b.slice(),d:clampD(d)}));
  const out=add?txBase.concat(list):list;
  if(out.length>MAX){msg('Too long: '+out.length+' frames, max '+MAX);return}
  fr=out;cur=add?txBase.length:0;render();
  msg('Text: '+list.length+' frames'+note+SAVE_HINT);
}
$('tx').oninput=$('ts').oninput=$('tm').onchange=$('te').onchange=$('ta').onchange=liveText;
$('tsv').textContent=textMs()+' ms';
for(const e of['❤️','😀','😢','😎','🔥','⭐','☀️','🌙','🌸','🐱','👻','🎵','🏠','⚡','💧','🐟','🎄','✅','👾','➡️','♥']){
  const b=document.createElement('button');b.textContent=e;
  b.onclick=()=>{
    const t=$('tx'),p=t.selectionStart==null?t.value.length:t.selectionStart,q=t.selectionEnd==null?p:t.selectionEnd;
    t.value=t.value.slice(0,p)+e+t.value.slice(q);
    t.focus();t.setSelectionRange(p+e.length,p+e.length);liveText();
  };
  $('emo').appendChild(b);
}

const pt=(b,x,y)=>{if(x>=0&&x<W&&y>=0&&y<H)b[y*W+x]=1};
const S=n=>shapeBits(SHAPES[n]);
function line(ys){
  const b=blank();
  for(let x=0;x<W;x++){
    const a=ys[x],c=x<W-1?ys[x+1]:a;
    for(let y=Math.min(a,c);y<=Math.max(a,c);y++)if(y!==c||a===c)pt(b,x,y);
  }
  return b;
}
const fall=(prob,len,d)=>{
  const ph=seq(W,()=>Math.random()*H|0),on=seq(W,()=>Math.random()<prob);
  return seq(H,f=>[seq(N,k=>{const x=k%W,p=((((k/W)|0)-f-ph[x])%H+H)%H;return on[x]&&p<len?1:0}),d]);
};
const ANIMS={
  'Beating heart':()=>FX.beat(S('Heart'),120),
  'Pac-man':()=>{
    const half=shapeBits(['...#####....','..#######...','.########...','.#######....','.#######....','.########...','..#######...','...#####....']);
    const shut=shapeBits(['...#####....','..#######...','.#########..','.#########..','.#########..','.#########..','..#######...','...#####....']);
    return[[S('Pac-man'),130],[half,130],[shut,130],[half,130]];
  },
  'Rain':()=>fall(.6,2,70),
  'Snow':()=>fall(.5,1,180),
  'Wave':()=>seq(16,f=>[line(seq(W,x=>Math.round(3.5+2.5*Math.sin((x+f)*Math.PI/8)))),70]),
  'Loading':()=>{
    const P=seq(8,i=>{const a=(i-2)*Math.PI/4;return[Math.round(6+3*Math.cos(a))-1,Math.round(4+3*Math.sin(a))-1]});
    return seq(8,f=>{const b=blank();[f,(f+7)%8].forEach(i=>{const[x,y]=P[i];pt(b,x,y);pt(b,x+1,y);pt(b,x,y+1);pt(b,x+1,y+1)});return[b,90]});
  },
  'Equalizer':()=>seq(12,()=>{const b=blank();for(let c=0;c<6;c++){const h=1+Math.random()*8|0;for(let y=H-h;y<H;y++)pt(b,c*2,y)}return[b,110]}),
  'Fireworks':()=>{
    const out=[];
    for(const[cx,cy]of[[4,3],[8,4]]){
      for(let y=H-1;y>cy;y--){const b=blank();pt(b,cx,y);out.push([b,60])}
      for(let r=0;r<=4;r++)out.push([seq(N,k=>Math.abs(Math.hypot(k%W-cx,((k/W)|0)-cy)-r)<.5?1:0),70]);
      out.push([seq(N,k=>{const d=Math.hypot(k%W-cx,((k/W)|0)-cy);return d>2.5&&d<4.5&&Math.random()<.4?1:0}),90],[blank(),250]);
    }
    return out;
  },
  'Eyes':()=>{
    const eyes=(px,blink)=>{
      const b=blank();
      for(const x0 of[1,7])for(let y=1;y<7;y++)for(let x=x0;x<x0+4;x++){
        const corner=(y===1||y===6)&&(x===x0||x===x0+3);
        if(blink?y===4:!corner)pt(b,x,y);
        if(!blink&&y>=3&&y<5&&x>=x0+1+px&&x<x0+3+px)b[y*W+x]=0;
      }
      return b;
    };
    return[[eyes(0),1200],[eyes(-1),500],[eyes(0),300],[eyes(1),500],[eyes(0),800],[eyes(0,1),120],[eyes(0),600],[eyes(0,1),120]];
  },
  'ECG':()=>{
    const ys=[5,5,5,5,5,4,5,5,6,1,7,5,5,5,4,4,5,5,5,5,5,5,5,5],L=ys.length;
    return seq(L,f=>[line(seq(W,x=>ys[(x+f)%L])),60]);
  },
  'Shining sun':()=>[[S('Sun'),300],[S('Sun').map((v,k)=>{const x=k%W,y=(k/W)|0;return x>2&&x<9&&y>1&&y<6?v:0}),300]],
  'Moon and stars':()=>{
    const moon=S('Moon').map((v,k)=>k%W<8?v:0),stars=[[9,1],[8,4],[10,6],[11,3],[10,0],[9,7]];
    return seq(6,()=>{const b=moon.slice();stars.forEach(([x,y])=>{if(Math.random()<.5)pt(b,x,y)});return[b,250]});
  }
};
for(const[name,make]of Object.entries(ANIMS)){
  const b=document.createElement('button'),c=document.createElement('canvas'),f=make();
  c.width=W*4;c.height=H*4;drawBits(c.getContext('2d'),f[f.length>>1][0],4,0);
  b.append(c,name);
  b.onclick=()=>useFrames(make(),'"'+name+'" loaded');
  $('an').appendChild(b);
}

async function pushLive(){
  if(!live)return;
  if(busy){queueLive();return}
  const body=fr.map(hex).join('');
  busy=true;
  try{
    if(body!==liveSent){if((await fetch('/preview',{method:'POST',body})).ok)liveSent=body}
    else await fetch('/ping');
  }catch(e){}
  busy=false;
}
function queueLive(){if(!live)return;clearTimeout(liveT);liveT=setTimeout(pushLive,250)}
setInterval(()=>{if(live&&!busy)pushLive()},2000);
$('lv').onclick=()=>{
  live=!live;liveSent='';
  $('lv').textContent='Preview on matrix: '+(live?'ON':'OFF');
  $('lv').classList.toggle('on',live);
  if(live)pushLive();else fetch('/stop',{method:'POST'}).catch(()=>{});
};
addEventListener('pagehide',()=>{if(live)navigator.sendBeacon('/stop')});

function toBits(px,w,h,tw=W,th=H){
  const lim=1-$('th').value/100;
  const s=Math.min(tw/w,th/h),ox=(tw-w*s)/2,oy=(th-h*s)/2,out=[];
  for(let cy=0;cy<th;cy++)for(let cx=0;cx<tw;cx++){
    const x0=(cx-ox)/s,x1=(cx+1-ox)/s,y0=(cy-oy)/s,y1=(cy+1-oy)/s;
    if(x1<=0||x0>=w||y1<=0||y0>=h){out.push(0);continue}
    const a=Math.max(0,Math.floor(x0)),b=Math.min(w,Math.max(a+1,Math.ceil(x1)));
    const c=Math.max(0,Math.floor(y0)),d=Math.min(h,Math.max(c+1,Math.ceil(y1)));
    let on=0,tot=0;
    for(let y=c;y<d;y++)for(let x=a;x<b;x++){tot++;if(px[y*w+x])on++}
    out.push(on/tot>=lim?1:0);
  }
  return out;
}
function lzw(min,data,n){
  const out=new Uint8Array(n),clr=1<<min,eoi=clr+1;
  const pre=new Uint16Array(4096),suf=new Uint8Array(4096),fst=new Uint8Array(4096),len=new Uint16Array(4096);
  for(let i=0;i<clr;i++){suf[i]=fst[i]=i;len[i]=1}
  let size=min+1,next=eoi+1,old=-1,acc=0,bits=0,p=0,o=0;
  while(o<n){
    while(bits<size){if(p>=data.length)return out;acc|=data[p++]<<bits;bits+=8}
    const code=acc&((1<<size)-1);acc>>>=size;bits-=size;
    if(code===clr){size=min+1;next=eoi+1;old=-1;continue}
    if(code===eoi||code>next)break;
    if(old<0){if(code>=clr)break;out[o++]=code;old=code;continue}
    if(next<4096){
      pre[next]=old;suf[next]=code<next?fst[code]:fst[old];fst[next]=fst[old];len[next]=len[old]+1;
      if(++next===1<<size&&size<12)size++;
    }
    for(let c=code,q=o+len[code]-1;q>=o;q--){if(q<n)out[q]=suf[c];c=pre[c]}
    o+=len[code];old=code;
  }
  return out;
}
function decode(buf){
  const b=new Uint8Array(buf);let p=0;
  const u8=()=>b[p++],u16=()=>b[p++]|b[p++]<<8,skip=()=>{let n;while(p<b.length&&(n=u8()))p+=n};
  if(b[0]!==71||b[1]!==73||b[2]!==70)throw 0;
  p=6;const w=u16(),h=u16(),gf=u8();p+=2;
  if(gf&128)p+=3<<((gf&7)+1);
  const full=new Uint8Array(w*h),raw=[];
  let disp=0,delay=0,ti=-1;
  while(p<b.length){
    const t=u8();
    if(t===0x21){
      if(u8()===0xF9){p++;const f=u8();delay=u16();ti=f&1?b[p]:-1;disp=f>>2&7;p++}
      skip();
    }else if(t===0x2C){
      const x=u16(),y=u16(),fw=u16(),fh=u16(),f=u8();
      if(f&128)p+=3<<((f&7)+1);
      const min=u8(),parts=[];let n,tot=0;
      while(p<b.length&&(n=u8())){parts.push(b.subarray(p,p+n));p+=n;tot+=n}
      const data=new Uint8Array(tot);tot=0;for(const s of parts){data.set(s,tot);tot+=s.length}
      const px=lzw(min,data,fw*fh),rows=[];
      if(f&64){for(const[s,st]of[[0,8],[4,8],[2,4],[1,2]])for(let r=s;r<fh;r+=st)rows.push(r)}
      else for(let r=0;r<fh;r++)rows.push(r);
      const snap=disp===3?full.slice():null;
      for(let i=0;i<fw*fh;i++){
        const yy=y+rows[(i/fw)|0],xx=x+i%fw;
        if(px[i]!==ti&&xx<w&&yy<h)full[yy*w+xx]=1;
      }
      raw.push({bits:toBits(full,w,h),d:delay<2?10:delay});
      if(disp===2)for(let yy=y;yy<Math.min(h,y+fh);yy++)full.fill(0,yy*w+x,yy*w+Math.min(w,x+fw));
      if(snap)full.set(snap);
      disp=0;delay=0;ti=-1;
    }else break;
  }
  if(!raw.length)throw 0;
  const step=Math.ceil(raw.length/MAX),out=[];let acc=0;
  raw.forEach((f,i)=>{acc+=f.d;if(i%step===step-1||i===raw.length-1){out.push({bits:f.bits,d:Math.min(acc*10,65535)});acc=0}});
  return out;
}
function hex(f){
  let s='';
  for(let k=0;k<3;k++){let v=0;for(let b=0;b<32;b++)v=v*2+f.bits[k*32+b];s+=v.toString(16).padStart(8,'0')}
  return s+f.d.toString(16).padStart(4,'0');
}
async function imgMask(file){
  let url;
  if(file.type==='image/svg+xml'||/\.svg$/i.test(file.name)){
    const doc=new DOMParser().parseFromString(await file.text(),'image/svg+xml'),s=doc.documentElement;
    if(s.nodeName!=='svg')throw 0;
    const vb=(s.getAttribute('viewBox')||'').split(/[\s,]+/).map(Number),num=a=>{const v=s.getAttribute(a)||'';return /^[\d.]+(px)?$/.test(v)?parseFloat(v):0};
    let w=num('width'),h=num('height');
    if(!(w>0&&h>0)){w=vb[2]>0?vb[2]:W;h=vb[3]>0?vb[3]:H}
    if(!s.getAttribute('viewBox'))s.setAttribute('viewBox','0 0 '+w+' '+h);
    const k=240/Math.max(w,h);
    s.setAttribute('width',w*k);s.setAttribute('height',h*k);
    url=URL.createObjectURL(new Blob([new XMLSerializer().serializeToString(doc)],{type:'image/svg+xml'}));
  }else url=URL.createObjectURL(file);
  const img=new Image();img.src=url;
  try{await img.decode()}finally{URL.revokeObjectURL(url)}
  const k=Math.min(1,480/Math.max(img.naturalWidth,img.naturalHeight)),w=Math.round(img.naturalWidth*k),h=Math.round(img.naturalHeight*k);
  if(!w||!h)throw 0;
  const c=document.createElement('canvas');c.width=w;c.height=h;
  const g=c.getContext('2d');g.drawImage(img,0,0,w,h);
  const d=g.getImageData(0,0,w,h).data,px=new Uint8Array(w*h);
  let alpha=false;for(let i=3;i<d.length;i+=4)if(d[i]<128){alpha=true;break}
  for(let i=0;i<w*h;i++){
    const a=d[i*4+3],l=d[i*4]*.3+d[i*4+1]*.59+d[i*4+2]*.11;
    px[i]=a>=128&&(alpha||l<128)?1:0;
  }
  return{px,w,h};
}
$('f').onchange=async e=>{
  const file=e.target.files[0];e.target.value='';if(!file)return;
  try{
    let nf;
    if(file.type==='image/gif'){nf=decode(await file.arrayBuffer());lastImg=null}
    else{lastImg=await imgMask(file);nf=[{bits:toBits(lastImg.px,lastImg.w,lastImg.h),d:100}]}
    snap();fr=nf;cur=0;render();
    msg('Loaded ('+fr.length+' frames): edit it, add effects and press "Save"');
  }catch(err){msg('Invalid file')}
};
$('th').onchange=()=>{
  if(!lastImg)return;
  snap();fr[cur].bits=toBits(lastImg.px,lastImg.w,lastImg.h);render();
};
function unhex(t){
  const out=[];
  for(let i=0;i+28<=t.length;i+=28){
    const bits=[];
    for(let k=0;k<3;k++){const v=parseInt(t.substr(i+k*8,8),16);for(let b=31;b>=0;b--)bits.push(v>>>b&1)}
    out.push({bits,d:parseInt(t.substr(i+24,4),16)});
  }
  return out;
}
async function loadBoard(){
  const nf=unhex(await (await fetch('/frames')).text());
  if(!nf.length)return false;
  fr=nf;cur=0;hist=[];render();return true;
}
$('rb').onclick=()=>loadBoard().then(ok=>msg(ok?'Animation loaded from the matrix':'No animation on the matrix')).catch(()=>msg('Board not reachable'));
const mm=t=>{const p=t.split(':');return Number(p[0])*60+Number(p[1])};
const hm=v=>String((v/60)|0).padStart(2,'0')+':'+String(v%60).padStart(2,'0');
$('send').onclick=async()=>{
  const en=$('en').checked,on=$('on').value,off=$('off').value;
  if(en&&(!on||!off)){msg('Set both schedule times');return}
  msg('Saving...');$('send').disabled=true;
  try{
    let ok=(await fetch('/upload',{method:'POST',body:fr.map(hex).join('')})).ok;
    if(ok&&on&&off)ok=(await fetch('/sched?en='+(en?1:0)+'&on='+mm(on)+'&off='+mm(off))).ok;
    msg(ok?'Saved: '+fr.length+' frames, '+(en?'on '+on+' - '+off:'always on'):'Save error');
  }catch(err){msg('Board not reachable')}
  $('send').disabled=false;
};
render();
msg('Ready');
loadBoard().catch(()=>{});
fetch('/state').then(r=>r.text()).then(t=>{
  const p=t.split(',').map(Number);
  $('en').checked=p[0]===1;$('on').value=hm(p[1]);$('off').value=hm(p[2]);
  $('now').textContent=p[3]<0?'Time not synced yet':'Board time: '+hm(p[3]);
});
</script></body></html>)rawliteral";

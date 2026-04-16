async function sha256(str) {
  const buf = new TextEncoder().encode(str);
  const hash = await crypto.subtle.digest("SHA-256", buf);
  return [...new Uint8Array(hash)].map(b => b.toString(16).padStart(2, "0")).join("");
}

/* ---------------- CANVAS FINGERPRINT ---------------- */
function canvasFP() {
  const canvas = document.createElement("canvas");
  const ctx = canvas.getContext("2d");

  ctx.textBaseline = "top";
  ctx.font = "16px Arial";
  ctx.fillStyle = "#f60";
  ctx.fillRect(10, 10, 100, 50);

  ctx.fillStyle = "#069";
  ctx.fillText("Fingerprint 👋 123456", 12, 20);

  ctx.fillStyle = "rgba(102, 204, 0, 0.7)";
  ctx.fillText("Canvas test", 10, 40);

  return canvas.toDataURL();
}

/* ---------------- WEBGL FINGERPRINT ---------------- */
function webglFP() {
  const canvas = document.createElement("canvas");
  const gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
  if (!gl) return "no-webgl";

  const debugInfo = gl.getExtension("WEBGL_debug_renderer_info");

  return {
    vendor: debugInfo ? gl.getParameter(debugInfo.UNMASKED_VENDOR_WEBGL) : "unknown",
    renderer: debugInfo ? gl.getParameter(debugInfo.UNMASKED_RENDERER_WEBGL) : "unknown"
  };
}

/* ---------------- SIMPLE FONT CHECK ---------------- */
function fontFP() {
  const baseFonts = ["monospace", "sans-serif", "serif"];
  const testFonts = ["Arial", "Verdana", "Times New Roman", "Comic Sans MS", "Courier New"];

  const testString = "mmmmmmmmmmlli";
  const size = "72px";

  const span = document.createElement("span");
  span.style.fontSize = size;
  span.innerHTML = testString;
  document.body.appendChild(span);

  const defaultWidths = {};

  baseFonts.forEach(font => {
    span.style.fontFamily = font;
    defaultWidths[font] = span.offsetWidth;
  });

  const detected = [];

  testFonts.forEach(font => {
    let matched = false;

    for (let base of baseFonts) {
      span.style.fontFamily = `${font},${base}`;
      if (span.offsetWidth !== defaultWidths[base]) {
        matched = true;
      }
    }

    if (matched) detected.push(font);
  });

  document.body.removeChild(span);

  return detected.join(",");
}

/* ---------------- AUDIO FINGERPRINT (LIGHT) ---------------- */
async function audioFP() {
  try {
    const ctx = new (window.OfflineAudioContext || window.webkitOfflineAudioContext)(1, 44100, 44100);
    const osc = ctx.createOscillator();
    osc.type = "triangle";
    osc.frequency.value = 1000;

    const comp = ctx.createDynamicsCompressor();

    osc.connect(comp);
    comp.connect(ctx.destination);

    osc.start(0);
    ctx.startRendering();

    const buffer = await new Promise(res => {
      ctx.oncomplete = e => res(e.renderedBuffer.getChannelData(0).slice(0, 1000));
    });

    return buffer.reduce((a, b) => a + Math.abs(b), 0).toString();
  } catch {
    return "no-audio";
  }
}

/* ---------------- DEVICE INFO ---------------- */
function deviceFP() {
  return [
    navigator.userAgent,
    navigator.language,
    screen.width + "x" + screen.height,
    screen.colorDepth,
    Intl.DateTimeFormat().resolvedOptions().timeZone,
    navigator.hardwareConcurrency,
    navigator.deviceMemory || "unknown"
  ].join("||");
}

function canvasConsistency() {
  const c = document.createElement("canvas");
  const ctx = c.getContext("2d");

  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, 20, 20);

  const a = ctx.getImageData(0, 0, 20, 20).data.slice();

  ctx.clearRect(0, 0, 20, 20);
  ctx.fillRect(0, 0, 20, 20);

  const b = ctx.getImageData(0, 0, 20, 20).data.slice();

  for (let i = 0; i < a.length; i++) {
    if (a[i] !== b[i]) return false;
  }

  return true;
}

/* ---------------- MAIN ---------------- */
async function generateFingerprint() {

  
  const data = [
    (canvasConsistency()?canvasFP():0) ,
    JSON.stringify(webglFP()),
    fontFP(),
    await audioFP(),
    deviceFP()
  ].join("###");

  return await sha256(data);
}

generateFingerprint().then(fp => {
  console.log(fp);
});
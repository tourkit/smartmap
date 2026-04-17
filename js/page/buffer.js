// =====================================================================
// BUFFER VIEW
// =====================================================================
function renderBufferView(node, container) {
  container.innerHTML = "";

  const wrap = document.createElement("div");
  wrap.className = "buffer-view";

  const toolbar = document.createElement("div");
  toolbar.className = "buffer-toolbar";

  const sizeInputObj = createLabelInput("Size", "number", { value: 200, max: "100000", className: "buffer-size-input" });
  const wInputObj = createLabelInput("W", "range", { min: 10, max: 80, value: 24 });
  const hInputObj = createLabelInput("H", "range", { min: 20, max: 60, value: 34 });

  const styleObj = createRadioGroup("buffer-style", ["fade", "bar", "barH", "cursor"], "fade");
  const gridObj = createCheckbox("grid", { title: "Show grid borders", className: "buffer-grid-check" });
  const decoObj = createCheckbox("deco", { title: "Add decorative fade overlay", className: "buffer-deco-check" });
  const idObj = createCheckbox("id", { title: "Show cell ID", checked: true, className: "buffer-id-check" });
  const valObj = createCheckbox("val", { title: "Show cell value", checked: true, className: "buffer-val-check" });
  const hideZeroObj = createCheckbox("hide0", { title: "Hide cells with value 0", className: "buffer-hide-zero-check" });

  toolbar.appendChild(sizeInputObj.label);
  toolbar.appendChild(wInputObj.label);
  toolbar.appendChild(hInputObj.label);
  toolbar.appendChild(styleObj.div);
  toolbar.appendChild(gridObj.label);
  toolbar.appendChild(decoObj.label);
  toolbar.appendChild(idObj.label);
  toolbar.appendChild(valObj.label);
  toolbar.appendChild(hideZeroObj.label);

  const glCanvas = document.createElement("canvas");
  glCanvas.id = "buffer-gl";

  const textCanvas = document.createElement("canvas");
  textCanvas.id = "buffer-text";

  const tooltip = document.createElement("div");
  tooltip.id = "buffer-tooltip";

  wrap.appendChild(toolbar);
  wrap.appendChild(glCanvas);
  wrap.appendChild(textCanvas);
  wrap.appendChild(tooltip);
  container.appendChild(wrap);

  const parent = container.parentElement;
  const setupBuffer = () => {
    const w = parent.clientWidth;
    const h = parent.clientHeight - toolbar.offsetHeight;

    glCanvas.width = w;
    glCanvas.height = h;
    glCanvas.style.top = toolbar.offsetHeight + "px";

    textCanvas.width = w;
    textCanvas.height = h;
    textCanvas.style.top = toolbar.offsetHeight + "px";

    const bufferObj = initBufferGL(glCanvas, textCanvas, tooltip, {
      size: () => +sizeInputObj.input.value,
      w: () => +wInputObj.input.value,
      h: () => +hInputObj.input.value,
      style: () => styleObj.getValue(),
      grid: () => gridObj.input.checked,
      deco: () => decoObj.input.checked,
      showId: () => idObj.input.checked,
      showVal: () => valObj.input.checked,
      hideZero: () => hideZeroObj.input.checked,
    });
    glCanvas._bufferRefresh = bufferObj.recalculate;
    glCanvas._bufferUpdate = bufferObj.updateSize;
  };

  sizeInputObj.input.oninput = () => sizeInputObj.input.value;
  wInputObj.input.oninput = () => {
    if (glCanvas._bufferUpdate) glCanvas._bufferUpdate();
  };
  hInputObj.input.oninput = () => {
    if (glCanvas._bufferUpdate) glCanvas._bufferUpdate();
  };
  styleObj.div.onchange = () => {
    if (glCanvas._bufferRefresh) glCanvas._bufferRefresh();
  };
  gridObj.input.onchange = () => {
    if (glCanvas._bufferRefresh) glCanvas._bufferRefresh();
  };
  decoObj.input.onchange = () => {
    if (glCanvas._bufferRefresh) glCanvas._bufferRefresh();
  };
  idObj.input.onchange = () => {
    if (glCanvas._bufferRefresh) glCanvas._bufferRefresh();
  };
  valObj.input.onchange = () => {
    if (glCanvas._bufferRefresh) glCanvas._bufferRefresh();
  };
  hideZeroObj.input.onchange = () => {
    if (glCanvas._bufferRefresh) glCanvas._bufferRefresh();
  };

  setupBuffer();
}

function initBufferGL(glCanvas, textCanvas, tooltip, opts, onStyleChange) {
  const gl = glCanvas.getContext("webgl2");
  if (!gl) {
    glCanvas.parentElement.innerHTML = "<div style='color:red;padding:20px;'>WEBGL2 REQUIRED</div>";
    return;
  }

  const tctx = textCanvas.getContext("2d");

  let N = opts.size();
  let buf = new Uint8Array(N);
  let cellW = opts.w();
  let cellH = opts.h();
  let style = opts.style();
  let grid = opts.grid();
  let deco = opts.deco();
  let showId = opts.showId();
  let showVal = opts.showVal();
  let hideZero = opts.hideZero();
  let W = glCanvas.width;
  let H = glCanvas.height;
  let cols = Math.floor(Math.max(5, W / cellW));

  const recalculate = () => {
    style = opts.style();
    grid = opts.grid();
    deco = opts.deco();
    showId = opts.showId();
    showVal = opts.showVal();
    hideZero = opts.hideZero();
    recompileShader();
    draw();
  };

  const updateSize = () => {
    cellW = opts.w();
    cellH = opts.h();
    draw();
  };

  broadcastCallbacks.push((data) => {
    if (data instanceof Blob) {
      data.arrayBuffer().then((buffer) => {
        const arr = new Uint8Array(buffer);
        N = Math.min(N, arr.length);
        buf.set(arr.subarray(0, N));
        draw();
      });
    } else if (data instanceof ArrayBuffer) {
      const arr = new Uint8Array(data);
      N = Math.min(N, arr.length);
      buf.set(arr.subarray(0, N));
      draw();
    } else if (typeof data === "string") {
      const arr = new TextEncoder().encode(data);
      N = Math.min(N, arr.length);
      buf.set(arr.subarray(0, N));
      draw();
    }
  });

  const vs = `#version 300 es
in vec2 a_quad;
in vec2 a_pos;
in vec2 a_size;
in float a_val;

uniform vec2 u_res;

out float v_val;
out vec2 v_uv;

void main(){
  vec2 pos = floor(a_pos + 0.5);
  vec2 size = a_size;
  vec2 offset = a_quad * size;
  vec2 pixelPos = pos + offset;
  vec2 zeroToOne = pixelPos / u_res;
  vec2 clip = zeroToOne * 2.0 - 1.0;
  clip.y = -clip.y;
  gl_Position = vec4(clip, 0, 1);
  v_val = a_val;
  v_uv = a_quad;
}
`;

  const compile = (type, src) => {
    const s = gl.createShader(type);
    gl.shaderSource(s, src);
    gl.compileShader(s);
    return s;
  };

  let prog = null;
  let a_quad, a_pos, a_size, a_val, u_res, u_cellSize;

  const getShaderParts = () => {
    const header = `precision highp float;
in float v_val;
in vec2 v_uv;
out vec4 outColor;

uniform vec2 u_cellSize;

float getV() { return v_val / 255.0; }
vec3 getBase() { return vec3(0.06,0.09,0.13); }
vec3 getBar() { return vec3(0.23,0.62,1.0); }
float getDecoAlpha() { return 1.0 - v_uv.y; }
void addGrid(inout vec3 color) { 
  float px = 1.0 / u_cellSize.x;
  float py = 1.0 / u_cellSize.y;
  float bx = step(1.0 - px, v_uv.x) * step(px, v_uv.x);
  float by = step(1.0 - py, v_uv.y) * step(py, v_uv.y);
  float border = max(bx, by);
  color = mix(color, vec3(1.0), border); 
}`;

    const makeBody = (colorCode) => `void main(){
float v = getV();
${colorCode}
${grid ? 'addGrid(color);' : ''}
outColor = vec4(color, ${deco ? 'getDecoAlpha()' : '1.0'});
}`;

    const bodies = {
      fade: makeBody("vec3 color = mix(getBase(), getBar(), v);"),
      bar: makeBody("float filled = step(1.0 - v, v_uv.y); vec3 color = filled > 0.5 ? getBar() : getBase();"),
      barH: makeBody("float filled = step(v_uv.x, v); vec3 color = filled > 0.5 ? getBar() : getBase();"),
      cursor: makeBody("float cursor = step(abs(v_uv.x - 0.5) * 2.0, v) * step(abs(v_uv.y - 0.5) * 2.0, v); vec3 color = cursor > 0.5 ? getBar() : getBase();")
    };
    return { header, body: bodies[style] || bodies.fade };
  };

  const recompileShader = () => {
    if (prog) gl.deleteProgram(prog);
    const parts = getShaderParts();
    const fsSrc = `#version 300 es\n${parts.header}\n${parts.body}`;
    prog = gl.createProgram();
    gl.attachShader(prog, compile(gl.VERTEX_SHADER, vs));
    gl.attachShader(prog, compile(gl.FRAGMENT_SHADER, fsSrc));
    gl.linkProgram(prog);
    gl.useProgram(prog);
    a_quad = gl.getAttribLocation(prog, "a_quad");
    a_pos = gl.getAttribLocation(prog, "a_pos");
    a_size = gl.getAttribLocation(prog, "a_size");
    a_val = gl.getAttribLocation(prog, "a_val");
    u_res = gl.getUniformLocation(prog, "u_res");
    u_cellSize = gl.getUniformLocation(prog, "u_cellSize");
  };

  recompileShader();

  const quadBuf = gl.createBuffer();
  const posBuf = gl.createBuffer();
  const sizeBuf = gl.createBuffer();
  const valBuf = gl.createBuffer();

  const quadData = new Float32Array([0,0,1,0,0,1,1,1]);

  const draw = () => {
    W = glCanvas.width;
    H = glCanvas.height;
    cols = Math.floor(Math.max(5, W / cellW));

    const pos = new Float32Array(N * 2);
    const size = new Float32Array(N * 2);
    const val = new Float32Array(N);

    for (let i = 0; i < N; i++) {
      const v = buf[i] || 0;
      const hidden = hideZero && v === 0;
      const x = (i % cols) * cellW;
      const y = Math.floor(i / cols) * cellH;
      pos[i * 2] = hidden ? -9999 : x;
      pos[i * 2 + 1] = hidden ? -9999 : y;
      size[i * 2] = cellW;
      size[i * 2 + 1] = cellH;
      val[i] = v;
    }

    gl.viewport(0, 0, W, H);
    gl.clearColor(0.05, 0.07, 0.1, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.uniform2f(u_res, W, H);
    gl.uniform2f(u_cellSize, cellW, cellH);

    gl.bindBuffer(gl.ARRAY_BUFFER, quadBuf);
    gl.bufferData(gl.ARRAY_BUFFER, quadData, gl.STATIC_DRAW);
    gl.enableVertexAttribArray(a_quad);
    gl.vertexAttribPointer(a_quad, 2, gl.FLOAT, false, 0, 0);
    gl.vertexAttribDivisor(a_quad, 0);

    gl.enableVertexAttribArray(a_pos);
    gl.bindBuffer(gl.ARRAY_BUFFER, posBuf);
    gl.bufferData(gl.ARRAY_BUFFER, pos, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(a_pos, 2, gl.FLOAT, false, 0, 0);
    gl.vertexAttribDivisor(a_pos, 1);

    gl.enableVertexAttribArray(a_size);
    gl.bindBuffer(gl.ARRAY_BUFFER, sizeBuf);
    gl.bufferData(gl.ARRAY_BUFFER, size, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(a_size, 2, gl.FLOAT, false, 0, 0);
    gl.vertexAttribDivisor(a_size, 1);

    gl.enableVertexAttribArray(a_val);
    gl.bindBuffer(gl.ARRAY_BUFFER, valBuf);
    gl.bufferData(gl.ARRAY_BUFFER, val, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(a_val, 1, gl.FLOAT, false, 0, 0);
    gl.vertexAttribDivisor(a_val, 1);

    gl.drawArraysInstanced(gl.TRIANGLE_STRIP, 0, 4, N);

    tctx.setTransform(1, 0, 0, 1, 0, 0);
    tctx.clearRect(0, 0, W, H);
    tctx.font = "10px monospace";
    tctx.textAlign = "center";
    tctx.textBaseline = "middle";

    for (let i = 0; i < N; i++) {
      const v = buf[i] || 0;
      if (hideZero && v === 0) continue;
      const x = (i % cols) * cellW;
      const y = Math.floor(i / cols) * cellH;
      const cx = x + cellW / 2;
      const cy = y + cellH / 2;

      if (showId) {
        tctx.fillStyle = "#cfe3ff";
        tctx.fillText(i, cx, cy - cellH * 0.25);
      }
      if (showVal) {
        tctx.fillStyle = "#7fb8ff";
        tctx.fillText(buf[i] || 0, cx, showId ? cy + cellH * 0.25 : cy);
      }
    }
  };

  let dragging = false;

  glCanvas.addEventListener("mousedown", () => (dragging = true));
  window.addEventListener("mouseup", () => (dragging = false));

  const getIndex = (x, y) => {
    const cx = Math.floor(x / cellW);
    const cy = Math.floor(y / cellH);
    return cy * cols + cx;
  };

  glCanvas.addEventListener("mousemove", (e) => {
    const r = glCanvas.getBoundingClientRect();
    const x = e.clientX - r.left;
    const y = e.clientY - r.top;
    const i = getIndex(x, y);

    if (i >= 0 && i < N) {
      tooltip.style.display = "block";
      tooltip.style.left = (e.clientX + 15) + "px";
      tooltip.style.top = (e.clientY + 15) + "px";
      tooltip.innerHTML = `id: ${i}<br>value: ${buf[i]}`;

      if (dragging) {
        const localY = y % cellH;
        const v = Math.max(0, Math.min(255, (1 - localY / cellH) * 255));
        buf[i] = v;
        draw();
      }
    } else {
      tooltip.style.display = "none";
    }
  });

  draw();

  return { recalculate, updateSize };
}
// =====================================================================
// PANE CONTENT RENDERERS
// =====================================================================
// Helper functions for UI components
function createLabelInput(label, type, opts = {}) {
  const l = document.createElement("label");
  l.textContent = label + ":";
  const input = document.createElement("input");
  input.type = type;
  if (opts.value) input.value = opts.value;
  if (opts.min) input.min = opts.min;
  if (opts.max) input.max = opts.max;
  if (opts.className) input.className = opts.className;
  l.appendChild(input);
  return { label: l, input };
}

function createCheckbox(labelText, opts = {}) {
  const l = document.createElement("label");
  l.title = opts.title || "";
  const input = document.createElement("input");
  input.type = "checkbox";
  if (opts.checked) input.checked = true;
  if (opts.className) input.className = opts.className;
  const span = document.createElement("span");
  span.textContent = labelText;
  l.appendChild(input);
  l.appendChild(span);
  return { label: l, input };
}

function createRadioGroup(name, values, defaultValue) {
  const div = document.createElement("div");
  div.className = "buffer-style-radios";
  values.forEach(v => {
    const l = document.createElement("label");
    const radio = document.createElement("input");
    radio.type = "radio";
    radio.name = name;
    radio.value = v;
    if (v === defaultValue) radio.checked = true;
    l.appendChild(radio);
    l.appendChild(document.createTextNode(v));
    div.appendChild(l);
  });
  return { div, getValue: () => div.querySelector(`input[name="${name}"]:checked`).value };
}

function renderPaneContent(node) {
  const el = node._content;
  if (!el) return;
  el.innerHTML = "";

  if (node.source === "treeview") {
    const div = document.createElement("div");
    div.className = "view-tree";
    el.appendChild(div);
    renderTree(div);
  } else if (node.source === "inspector") {
    const div = document.createElement("div");
    div.className = "view-inspector";
    el.appendChild(div);
    renderInspector(div);
  } else if (node.source === "jsoneditor") {
    renderJsonEditor(node, el);
  } else if (node.source === "buffer") {
    renderBufferView(node, el);
  } else {
    const div = document.createElement("div");
    div.className = "view-empty";
    div.textContent = "Empty pane";
    el.appendChild(div);
  }
}

function refreshAllPanes(type) {
     
  forEachLeaf(layoutRoot, node => {
    console.log("oooooo")
    if (!type || node.source === type) renderPaneContent(node);
  });
}

function forEachLeaf(node, fn) {
  if (!node) return;
  if (node.type === "leaf") { fn(node); return; }
  node.children.forEach(c => forEachLeaf(c, fn));
}

// =====================================================================
// TREE RENDER
// =====================================================================
function renderTree(container) {
  container.innerHTML = "";

  const tree = document.createElement("ul");
  tree.className = "tree-list";

  // === Registre (collapsible parent) ===
  const registreLi = document.createElement("li");
  registreLi.className = "tree-category";

  const registreHeader = document.createElement("div");
  registreHeader.className = "tree-category-header";
  registreHeader.innerHTML = `<span class="tree-toggle">▶</span> <span>📁 Registre</span>`;
  registreLi.appendChild(registreHeader);

  const registreChildren = document.createElement("ul");
  registreChildren.className = "tree-children";

  registry.forEach(struct => {
    const structLi = document.createElement("li");
    structLi.className = "tree-item";
    structLi.textContent = "📦 " + struct.name;
    structLi.title = struct.fields.map(f => `${f.type} ${f.label}`).join(", ");
    structLi.onclick = () => selectStruct(struct);
    registreChildren.appendChild(structLi);
  });

  registreLi.appendChild(registreChildren);
  tree.appendChild(registreLi);

  registreHeader.onclick = () => {
    registreHeader.querySelector(".tree-toggle").classList.toggle("expanded");
    registreChildren.classList.toggle("visible");
  };

  // === Buffer (collapsible parent) ===
  const bufferLi = document.createElement("li");
  bufferLi.className = "tree-category";

  const bufferHeader = document.createElement("div");
  bufferHeader.className = "tree-category-header";
  bufferHeader.innerHTML = `<span class="tree-toggle">▶</span> <span>📥 Buffer</span>`;
  bufferLi.appendChild(bufferHeader);

  const bufferChildren = document.createElement("ul");
  bufferChildren.className = "tree-children";

  buffer.forEach(item => {
    const itemLi = document.createElement("li");
    itemLi.className = "tree-item buffer-item";
    
    const row = document.createElement("div");
    row.className = "tree-item-row";

    const toggle = document.createElement("span");
    toggle.className = "tree-toggle";
    toggle.textContent = item.type ? "▶" : "";
    if (!item.type) toggle.style.visibility = "hidden";

    const name = document.createElement("span");
    name.className = "tree-item-name";
    name.textContent = "📦 " + item.name;

    const type = document.createElement("span");
    type.className = "tree-item-type";
    type.textContent = item.type ? `[${item.type}]` : "";

    const menuBtn = document.createElement("button");
    menuBtn.className = "pane-header-btn";
    menuBtn.textContent = "⋯";
    menuBtn.onclick = (e) => {
      e.stopPropagation();
      openPaneTypeMenu(item, menuBtn);
    };

    row.appendChild(toggle);
    row.appendChild(name);
    row.appendChild(type);
    row.appendChild(menuBtn);
    itemLi.appendChild(row);

    if (item.type) {
      const hier = buildBufferHierarchy(item);
      if (hier) {
        const itemChildren = document.createElement("ul");
        itemChildren.className = "tree-children";
        renderHierarchyList(hier, itemChildren);
        itemLi.appendChild(itemChildren);
        itemChildren.classList.remove("visible");

        const onToggle = () => {
          toggle.classList.toggle("expanded");
          toggle.textContent = toggle.classList.contains("expanded") ? "▼" : "▶";
          itemChildren.classList.toggle("visible");
        };

        toggle.onclick = onToggle;
        name.onclick = onToggle;
      }
    }

    bufferChildren.appendChild(itemLi);
  });

  bufferLi.appendChild(bufferChildren);
  tree.appendChild(bufferLi);

  bufferHeader.onclick = () => {
    bufferHeader.querySelector(".tree-toggle").classList.toggle("expanded");
    bufferChildren.classList.toggle("visible");
  };

  // === Files (collapsible parent) ===
  const filesLi = document.createElement("li");
  filesLi.className = "tree-category";

  const filesHeader = document.createElement("div");
  filesHeader.className = "tree-category-header";
  filesHeader.innerHTML = `<span class="tree-toggle">▶</span> <span>📄 Files</span>`;
  filesLi.appendChild(filesHeader);

  const filesChildren = document.createElement("ul");
  filesChildren.className = "tree-children";

  if (fileList.length > 0) {
    fileList.forEach(file => {
      const fileLi = document.createElement("li");
      fileLi.className = "tree-item";
      fileLi.innerHTML = `<span class="tree-item-name">📄 ${file.name || file}</span>`;
      fileLi.onclick = () => {
        configPath = file.path || file.name || file;
        editJSON();
      };
      filesChildren.appendChild(fileLi);
    });
  } else {
    const emptyLi = document.createElement("li");
    emptyLi.className = "tree-item";
    emptyLi.style.color = "#555";
    emptyLi.textContent = "(no files)";
    filesChildren.appendChild(emptyLi);
  }

  filesLi.appendChild(filesChildren);
  tree.appendChild(filesLi);

  filesHeader.onclick = () => {
    filesHeader.querySelector(".tree-toggle").classList.toggle("expanded");
    filesChildren.classList.toggle("visible");
  };

  container.appendChild(tree);
}

function renderHierarchyList(node, container) {
  if (!node) return;

  const li = document.createElement("li");
  li.className = "tree-item";

  const row = document.createElement("div");
  row.className = "tree-item-row";

  const toggle = document.createElement("span");
  toggle.className = "tree-toggle";

  const name = document.createElement("span");
  name.className = "tree-item-name";
  name.textContent = node.name;

  const type = document.createElement("span");
  type.className = "tree-item-type";
  type.textContent = node.type ? `[${node.type}]` : "";

  row.appendChild(toggle);
  row.appendChild(name);
  row.appendChild(type);
  li.appendChild(row);

  if (node.children && node.children.length > 0) {
    const children = document.createElement("ul");
    children.className = "tree-children";
    node.children.forEach(child => renderHierarchyList(child, children));
    li.appendChild(children);
    children.classList.remove("visible");

    const onToggle = () => {
      toggle.classList.toggle("expanded");
      toggle.textContent = toggle.classList.contains("expanded") ? "▼" : "▶";
      children.classList.toggle("visible");
    };

    toggle.onclick = onToggle;
    name.onclick = onToggle;
  }

  container.appendChild(li);
}

function renderJsonFileTree(parsed, container) {
  function walk(key, value, depth) {
    const isObj = value && typeof value === "object" && !Array.isArray(value);
    const isArr = Array.isArray(value);

    const li = document.createElement("li");
    li.className = "tree-item";

    const row = document.createElement("div");
    row.className = "tree-item-row";

    if (isObj || isArr) {
      const toggle = document.createElement("span");
      toggle.className = "tree-toggle";
      toggle.textContent = "▶";

      const name = document.createElement("span");
      name.className = "tree-item-name";
      name.textContent = key;

      const type = document.createElement("span");
      type.className = "tree-item-type";
      type.textContent = isArr ? "[]" : "{}";

      row.appendChild(toggle);
      row.appendChild(name);
      row.appendChild(type);
      li.appendChild(row);

      const children = document.createElement("ul");
      children.className = "tree-children";
      Object.keys(value).forEach(k => walk(k, value[k], depth + 1));
      li.appendChild(children);
      children.classList.remove("visible");

      const onToggle = () => {
        toggle.classList.toggle("expanded");
        toggle.textContent = toggle.classList.contains("expanded") ? "▼" : "▶";
        children.classList.toggle("visible");
      };

      toggle.onclick = onToggle;
      name.onclick = onToggle;
    } else {
      const name = document.createElement("span");
      name.className = "tree-item-name";
      name.textContent = key;

      const valueSpan = document.createElement("span");
      valueSpan.className = "tree-item-value json-num";
      valueSpan.textContent = JSON.stringify(value);

      row.appendChild(name);
      row.appendChild(document.createTextNode(": "));
      row.appendChild(valueSpan);
      li.appendChild(row);
    }

    container.appendChild(li);
  }

  Object.keys(parsed).forEach(k => walk(k, parsed[k], 0));
}

// =====================================================================
// INSPECTOR RENDER
// =====================================================================
function renderInspector(container) {
  if (!selected) {
    container.innerHTML = `<div class="title">Inspector</div><div class="box">Drop a field here or select a struct</div>`;
    return;
  }

  const struct = selected;
  const tm = typeMap();
  container.innerHTML = `
    <div class="title">📦 ${struct.name}</div>
    <div class="box">
      ${struct.fields.map(f => `
        <div class="row">
          <input value="${f.label}" onchange="updateField('${struct.name}','${f.label}',this.value,'label')" />
          <select onchange="updateField('${struct.name}','${f.label}',this.value,'type')">
            ${["int","float","string","bool",...Object.keys(tm)]
              .map(t => `<option ${t===f.type?"selected":""}>${t}</option>`).join("")}
          </select>
        </div>
      `).join("")}
    </div>
  `;
}

function selectStruct(struct) {
  selected = struct;
  refreshAllPanes("inspector");
}

window.updateField = function(structName, oldLabel, value, prop) {
  const s = registry.find(x => x.name === structName);
  if (!s) return;
  const f = s.fields.find(x => x.label === oldLabel);
  if (!f) return;
  f[prop] = value;
  refreshAllPanes("treeview");
};

// =====================================================================
// JSON EDITOR
// =====================================================================

let jsonDocText = "";
let configPath = "./config.json";

async function loadConfig() {
  try {
    const res = await fetch(configPath);
    if (res.ok) {
      const text = await res.text();
      jsonDocText = text;
    } else {
      jsonDocText = "{}";
    }
  } catch (e) {
    jsonDocText = "{}";
  }
}

async function saveConfig() {
  if (validateAndMark(jsonDocText)) {
    try {
      await fetch(configPath, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: jsonDocText
      });
      return true;
    } catch (e) {
      return false;
    }
  }
  return false;
}

function highlightJSON(raw) {
  let s = raw
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;");

  s = s.replace(
    /("(?:[^"\\]|\\.)*")\s*:/g,
    '<span class="json-key">$1</span>:'
  );
  s = s.replace(
    /:\s*("(?:[^"\\]|\\.)*")/g,
    ': <span class="json-str">$1</span>'
  );
  s = s.replace(
    /^(\s*)("(?:[^"\\]|\\.)*")/gm,
    (m, indent, str) => {
      if (m.includes('class="json-key"') || m.includes('class="json-str"')) return m;
      return indent + '<span class="json-str">' + str + '</span>';
    }
  );
  s = s.replace(
    /\b(-?\d+\.?\d*(?:[eE][+-]?\d+)?)\b/g,
    '<span class="json-num">$1</span>'
  );
  s = s.replace(
    /\b(true|false|null)\b/g,
    '<span class="json-kw">$1</span>'
  );
  return s;
}

function updateLineNumbers(linesEl, text) {
  const count = (text.match(/\n/g) || []).length + 1;
  let ln = "";
  for (let i = 1; i <= count; i++) ln += i + "\n";
  linesEl.textContent = ln;
}

function renderJsonEditor(node, container) {
  container.innerHTML = "";

  const wrap = document.createElement("div");
  wrap.className = "view-jsoneditor";

  const toolbar = document.createElement("div");
  toolbar.className = "jsoneditor-toolbar";

  // === Format button with auto checkbox ===
  const formatAutoLabel = document.createElement("label");
  formatAutoLabel.className = "jsoneditor-btn-auto";
  formatAutoLabel.title = "Auto format before formatting";

  const formatAutoCheck = document.createElement("input");
  formatAutoCheck.type = "checkbox";
  formatAutoCheck.checked = false;

  const formatAutoIcon = document.createElement("span");
  formatAutoIcon.textContent = "⚡";
  formatAutoIcon.className = "jsoneditor-btn-icon";

  formatAutoLabel.appendChild(formatAutoCheck);
  formatAutoLabel.appendChild(formatAutoIcon);
  formatAutoLabel.onclick = (e) => e.stopPropagation();

  const formatBtn = document.createElement("button");
  formatBtn.textContent = "⟳ Format";
  formatBtn.title = "Format JSON (Ctrl+Shift+F)";

  // === Save button with auto checkbox ===
  const saveAutoLabel = document.createElement("label");
  saveAutoLabel.className = "jsoneditor-btn-auto";
  saveAutoLabel.title = "Auto format before save";

  const saveAutoCheck = document.createElement("input");
  saveAutoCheck.type = "checkbox";
  saveAutoCheck.checked = false;

  const saveAutoIcon = document.createElement("span");
  saveAutoIcon.textContent = "⚡";
  saveAutoIcon.className = "jsoneditor-btn-icon";

  saveAutoLabel.appendChild(saveAutoCheck);
  saveAutoLabel.appendChild(saveAutoIcon);
  saveAutoLabel.onclick = (e) => e.stopPropagation();

  const saveBtn = document.createElement("button");
  saveBtn.textContent = "💾 Save";
  saveBtn.title = "Save (Ctrl+S)";

  const status = document.createElement("span");
  status.className = "jsoneditor-status";
  status.textContent = configPath;

  toolbar.appendChild(formatAutoLabel);
  toolbar.appendChild(formatBtn);
  toolbar.appendChild(saveAutoLabel);
  toolbar.appendChild(saveBtn);
  toolbar.appendChild(status);

  const body = document.createElement("div");
  body.className = "jsoneditor-body";

  const linesEl = document.createElement("div");
  linesEl.className = "jsoneditor-lines";

  const highlightEl = document.createElement("div");
  highlightEl.className = "jsoneditor-highlight";

  const textarea = document.createElement("textarea");
  textarea.className = "jsoneditor-textarea";
  textarea.spellcheck = false;
  textarea.autocomplete = "off";
  textarea.value = jsonDocText;

  body.appendChild(linesEl);
  body.appendChild(highlightEl);
  body.appendChild(textarea);

  wrap.appendChild(toolbar);
  wrap.appendChild(body);
  container.appendChild(wrap);

  function syncHighlight() {
    highlightEl.innerHTML = highlightJSON(textarea.value);
    updateLineNumbers(linesEl, textarea.value);
  }

  textarea.addEventListener("scroll", () => {
    highlightEl.scrollTop  = textarea.scrollTop;
    highlightEl.scrollLeft = textarea.scrollLeft;
    linesEl.scrollTop      = textarea.scrollTop;
  });

  function validateAndMark(text) {
    try {
      JSON.parse(text);
      status.className = "jsoneditor-status ok";
      status.textContent = "✓ Valid JSON — config.json";
      return true;
    } catch (e) {
      status.className = "jsoneditor-status error";
      status.textContent = "✗ " + e.message;
      return false;
    }
  }

  function doFormat() {
    if (validateAndMark(textarea.value)) {
      try {
        textarea.value = formatPreservingEmptyLines(textarea.value);
        jsonDocText = textarea.value;
        syncHighlight();
        validateAndMark(textarea.value);
      } catch (e) {
        validateAndMark(textarea.value);
      }
    }
  }

  async function doSave() {
    if (validateAndMark(textarea.value)) {
      if (saveAutoCheck.checked) {
        textarea.value = formatPreservingEmptyLines(textarea.value);
        syncHighlight();
      }
      jsonDocText = textarea.value;
      const ok = await saveConfig();
      if (ok) {
        status.className = "jsoneditor-status ok";
        status.textContent = "✓ Saved — " + configPath;
      } else {
        status.className = "jsoneditor-status error";
        status.textContent = "✗ Save failed";
      }
    }
  }

  function formatPreservingEmptyLines(text) {
    const parsed = JSON.parse(text);
    const lines = text.split('\n');
    const emptyLineIndices = new Set();
    lines.forEach((line, i) => {
      if (line.trim() === '') emptyLineIndices.add(i);
    });

    let formatted = JSON.stringify(parsed, null, 2);
    const formattedLines = formatted.split('\n');

    if (emptyLineIndices.size > 0) {
      const result = [];
      let srcIdx = 0;
      let emptySet = new Set(emptyLineIndices);
      for (let i = 0; i < lines.length; i++) {
        if (emptySet.has(i) && srcIdx < formattedLines.length) {
          result.push('');
        }
        if (srcIdx < formattedLines.length) {
          result.push(formattedLines[srcIdx++]);
        }
      }
      formatted = result.join('\n');
    }
    return formatted;
  }

  let formatTimeout = null;

  textarea.addEventListener("input", () => {
    jsonDocText = textarea.value;
    syncHighlight();
    validateAndMark(textarea.value);

    clearTimeout(formatTimeout);
    formatTimeout = setTimeout(() => {
      if (validateAndMark(textarea.value)) {
        try {
          textarea.value = formatPreservingEmptyLines(textarea.value);
          jsonDocText = textarea.value;
          syncHighlight();
        } catch (e) {}
      }
    }, 600);
  });

  function getAutoIndent(text, pos) {
    const before = text.slice(0, pos);
    const lineStart = before.lastIndexOf('\n') + 1;
    const currentLine = before.slice(lineStart);
    let indent = currentLine.match(/^(\s*)/)[1].length;
    const opens = (currentLine.match(/[{[\[]/g) || []).length;
    const closes = (currentLine.match(/[}\]]/g) || []).length;
    if (closes < opens) indent += 2;
    return ' '.repeat(indent);
  }

  textarea.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {
      e.preventDefault();
      const s = textarea.selectionStart;
      const v = textarea.value;
      const indent = getAutoIndent(v, s);
      textarea.value = v.slice(0, s) + '\n' + indent + v.slice(textarea.selectionEnd);
      textarea.selectionStart = textarea.selectionEnd = s + 1 + indent.length;
      syncHighlight();
      return;
    }
    if (e.key === "Tab") {
      e.preventDefault();
      const s = textarea.selectionStart;
      const v = textarea.value;
      textarea.value = v.slice(0, s) + "  " + v.slice(textarea.selectionEnd);
      textarea.selectionStart = textarea.selectionEnd = s + 2;
      syncHighlight();
    }
    if (e.key === "s" && (e.ctrlKey || e.metaKey)) {
      e.preventDefault();
      doSave();
    }
    if (e.key === "F" && (e.ctrlKey || e.metaKey) && e.shiftKey) {
      e.preventDefault();
      doFormat();
    }
  });

  formatBtn.addEventListener("click", doFormat);
  saveBtn.addEventListener("click", doSave);

  syncHighlight();
  validateAndMark(textarea.value);

  node._jsonTextarea = textarea;
}

function editJSON() {
  let target = null;
  forEachLeaf(layoutRoot, n => {
    if (!target && n.source === "jsoneditor") target = n;
  });
  if (target) {
    renderPaneContent(target);
    return;
  }
  forEachLeaf(layoutRoot, n => {
    if (!target && n.source === "inspector") target = n;
  });
  if (!target) {
    forEachLeaf(layoutRoot, n => { if (!target) target = n; });
  }
  if (target) {
    target.source = "jsoneditor";
    target._title.textContent = SOURCES.jsoneditor.icon + " " + SOURCES.jsoneditor.label;
    renderPaneContent(target);
  }
}

// =====================================================================
// BUFFER VIEW (test.html converted to pane)
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

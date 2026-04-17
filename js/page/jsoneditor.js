// =====================================================================
// JSON EDITOR VIEW
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
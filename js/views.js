// =====================================================================
// PANE CONTENT DISPATCHER
// =====================================================================
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
  } else if (node.source === "timeline") {
    const div = document.createElement("div");
    div.className = "view-timeline";
    el.appendChild(div);
    renderTimelineEditor(node, div);
  } else {
    const div = document.createElement("div");
    div.className = "view-empty";
    div.textContent = "Empty pane";
    el.appendChild(div);
  }
}

function refreshAllPanes(type) {
  forEachLeaf(layoutRoot, node => {
    if (!type || node.source === type) renderPaneContent(node);
  });
}

function forEachLeaf(node, fn) {
  if (!node) return;
  if (node.type === "leaf") { fn(node); return; }
  node.children.forEach(c => forEachLeaf(c, fn));
}
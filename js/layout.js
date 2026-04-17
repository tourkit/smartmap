// =====================================================================
// PANE LAYOUT ENGINE
// =====================================================================
// A node is either:
//   { type: "leaf",  id, source, el }
//   { type: "split", direction: "vertical"|"horizontal", children: [node,node,...], sizes:[...], el }

let layoutRoot = null;
let paneIdCounter = 0;

const SOURCES = {
  treeview:   { label: "Tree View",   icon: "🗂" },
  inspector: { label: "Inspector",   icon: "🔍" },
  jsoneditor: { label: "JSON Editor", icon: "📝" },
  buffer:     { label: "Buffer",      icon: "▦" },
  empty:      { label: "Empty",       icon: "⬜" },
};

function makeLeaf(source = "empty") {
  return { type: "leaf", id: ++paneIdCounter, source, el: null };
}

function buildDOM(node, parentEl) {
  if (node.type === "leaf") {
    const wrapper = document.createElement("div");
    wrapper.className = "pane-leaf";
    wrapper.dataset.paneId = node.id;

    const header = document.createElement("div");
    header.className = "pane-header";

    const title = document.createElement("span");
    title.className = "pane-title";
    title.textContent = (SOURCES[node.source]?.icon || "") + " " + (SOURCES[node.source]?.label || node.source);

    const menuBtn = document.createElement("button");
    menuBtn.className = "pane-header-btn";
    menuBtn.title = "Pane options";
    menuBtn.textContent = "⋯";
    menuBtn.addEventListener("click", (e) => {
      e.stopPropagation();
      openPaneMenu(node, menuBtn);
    });

    header.appendChild(title);
    header.appendChild(menuBtn);

    const content = document.createElement("div");
    content.className = "pane-content";
    content.id = "pane-content-" + node.id;

    wrapper.appendChild(header);
    wrapper.appendChild(content);

    node.el = wrapper;
    node._header = header;
    node._title  = title;
    node._content = content;

    parentEl.appendChild(wrapper);
    renderPaneContent(node);
    return wrapper;
  }

  if (node.type === "split") {
    const wrap = document.createElement("div");
    wrap.className = "pane-split " + node.direction;
    node.el = wrap;
    parentEl.appendChild(wrap);

    node.children.forEach((child, i) => {
      buildDOM(child, wrap);
      applySize(node, i);
      if (i < node.children.length - 1) {
        const spl = makeSplitter(node, i);
        wrap.appendChild(spl);
      }
    });

    rebuildSplitDOM(node);
    return wrap;
  }
}

function rebuildSplitDOM(node) {
  const wrap = node.el;
  wrap.innerHTML = "";
  node.children.forEach((child, i) => {
    if (!child.el) buildDOM(child, wrap);
    else wrap.appendChild(child.el);
    applySize(node, i);
    if (i < node.children.length - 1) {
      wrap.appendChild(makeSplitter(node, i));
    }
  });
}

function applySize(splitNode, childIdx) {
  const child = splitNode.children[childIdx];
  if (!child.el) return;
  const size = splitNode.sizes[childIdx];
  if (splitNode.direction === "vertical") {
    child.el.style.width  = size + "%";
    child.el.style.height = "100%";
    child.el.style.flex   = "none";
  } else {
    child.el.style.height = size + "%";
    child.el.style.width  = "100%";
    child.el.style.flex   = "none";
  }
}

function makeSplitter(splitNode, afterIdx) {
  const spl = document.createElement("div");
  spl.className = "splitter";

  spl.addEventListener("mousedown", (e) => {
    e.preventDefault();
    spl.classList.add("active");
    const isVert = splitNode.direction === "vertical";
    const container = splitNode.el;
    const rect = container.getBoundingClientRect();
    const totalPx = isVert ? rect.width : rect.height;

    const onMove = (ev) => {
      const pos = isVert ? ev.clientX - rect.left : ev.clientY - rect.top;
      const sumBefore = splitNode.sizes.slice(0, afterIdx).reduce((a,b)=>a+b,0);
      const sumAfter  = splitNode.sizes.slice(afterIdx+2).reduce((a,b)=>a+b,0);
      const available = 100 - sumBefore - sumAfter;
      let newA = (pos / totalPx) * 100 - sumBefore;
      newA = Math.max(5, Math.min(available - 5, newA));
      splitNode.sizes[afterIdx]   = newA;
      splitNode.sizes[afterIdx+1] = available - newA;
      applySize(splitNode, afterIdx);
      applySize(splitNode, afterIdx+1);
    };

    const onUp = () => {
      spl.classList.remove("active");
      document.removeEventListener("mousemove", onMove);
      document.removeEventListener("mouseup", onUp);
      saveLayout();
    };

    document.addEventListener("mousemove", onMove);
    document.addEventListener("mouseup", onUp);
  });

  return spl;
}

// =====================================================================
// SPLIT / CLOSE
// =====================================================================
function splitPane(leaf, direction) {
  const parent = findParent(layoutRoot, null, leaf);
  const newLeaf = makeLeaf(leaf.source);

  if (!parent) {
    const split = {
      type: "split",
      direction,
      children: [leaf, newLeaf],
      sizes: [50, 50],
      el: null
    };
    layoutRoot = split;
    const rootEl = document.getElementById("root-container");
    rootEl.innerHTML = "";
    buildDOM(layoutRoot, rootEl);
    saveLayout();
} else {
      const idx = parent.children.indexOf(leaf);

      if (parent.direction === direction) {
        const sz = parent.sizes[idx] / 2;
        parent.sizes[idx] = sz;
        parent.sizes.splice(idx + 1, 0, sz);
        parent.children.splice(idx + 1, 0, newLeaf);
        newLeaf.el = null;
        rebuildSplitDOM(parent);
        saveLayout();
        renderPaneContent(newLeaf);
    } else {
      const newSplit = {
        type: "split",
        direction,
        children: [leaf, newLeaf],
        sizes: [50, 50],
        el: null
      };
      parent.children[idx] = newSplit;
      newSplit.el = null;
      leaf.el = null;
      rebuildSplitDOM(parent);
      renderPaneContent(leaf);
      renderPaneContent(newLeaf);
    }
  }
}

function closePane(leaf) {
  const parent = findParent(layoutRoot, null, leaf);
  if (!parent) return;

  const idx = parent.children.indexOf(leaf);
  const removedSize = parent.sizes[idx];

  parent.children.splice(idx, 1);
  parent.sizes.splice(idx, 1);

  if (parent.sizes.length > 0) {
    const neighborIdx = Math.min(idx, parent.sizes.length - 1);
    parent.sizes[neighborIdx] += removedSize;
  }

  if (parent.children.length === 1) {
    collapseParent(parent);
  } else {
    rebuildSplitDOM(parent);
    saveLayout();
  }
}

function collapseParent(splitNode) {
  const grandParent = findParent(layoutRoot, null, splitNode);
  const onlyChild = splitNode.children[0];

  if (!grandParent) {
    layoutRoot = onlyChild;
    const rootEl = document.getElementById("root-container");
    rootEl.innerHTML = "";
    onlyChild.el = null;
    buildDOM(layoutRoot, rootEl);
  } else {
    const idx = grandParent.children.indexOf(splitNode);
    grandParent.children[idx] = onlyChild;
    onlyChild.el = null;
    rebuildSplitDOM(grandParent);
    if (onlyChild.type === "leaf") renderPaneContent(onlyChild);
  }
  saveLayout();
}

function findParent(node, parent, target) {
  if (node === target) return parent;
  if (node.type === "split") {
    for (const child of node.children) {
      const found = findParent(child, node, target);
      if (found !== undefined) return found;
    }
  }
  return undefined;
}

// =====================================================================
// INIT
// =====================================================================
const STORAGE_KEY = "smartmap-layout";

function serializeLayout(node) {
  if (node.type === "leaf") {
    return { type: "leaf", source: node.source };
  }
  return {
    type: "split",
    direction: node.direction,
    sizes: node.sizes,
    children: node.children.map(serializeLayout)
  };
}

function saveLayout() {
  if (!layoutRoot) return;
  const data = serializeLayout(layoutRoot);
  localStorage.setItem(STORAGE_KEY, JSON.stringify(data));
}

function loadLayout() {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return null;
    return JSON.parse(raw);
  } catch (e) {
    return null;
  }
}

function deserializeLayout(data) {
  if (data.type === "leaf") {
    return makeLeaf(data.source);
  }
  return {
    type: "split",
    direction: data.direction,
    sizes: data.sizes,
    children: data.children.map(deserializeLayout),
    el: null
  };
}

function initLayout() {
  const saved = loadLayout();
  if (saved) {
    layoutRoot = deserializeLayout(saved);
  } else {
    layoutRoot = {
      type: "split",
      direction: "vertical",
      sizes: [25, 75],
      children: [
        makeLeaf("treeview"),
        makeLeaf("inspector")
      ],
      el: null
    };
  }

  const rootEl = document.getElementById("root-container");
  buildDOM(layoutRoot, rootEl);
  saveLayout();
}

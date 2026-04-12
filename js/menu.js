// =====================================================================
// PANE CONTEXT MENU
// =====================================================================
let activeMenuNode = null;
const menuEl = document.getElementById("pane-menu");

document.addEventListener("click", () => closeMenu());

function openPaneMenu(node, anchorEl) {
  activeMenuNode = node;
  menuEl.innerHTML = "";

  addMenuLabel("Split");
  addMenuItem("↔ Split Vertical",   () => splitPane(node, "vertical"));
  addMenuItem("↕ Split Horizontal", () => splitPane(node, "horizontal"));

  addMenuSep();

  addMenuLabel("View");
  Object.entries(SOURCES).forEach(([key, info]) => {
    const active = node.source === key;
    addMenuItem((active ? "✓ " : "    ") + info.icon + " " + info.label, () => {
      node.source = key;
      node._title.textContent = info.icon + " " + info.label;
      renderPaneContent(node);
      closeMenu();
    });
  });

  addMenuSep();
  addMenuItem("✕ Close pane", () => closePane(node));

  const rect = anchorEl.getBoundingClientRect();
  menuEl.style.display = "block";
  const mw = menuEl.offsetWidth;
  let left = rect.left;
  if (left + mw > window.innerWidth) left = window.innerWidth - mw - 4;
  menuEl.style.left = left + "px";
  menuEl.style.top  = (rect.bottom + 4) + "px";
}

function closeMenu() {
  menuEl.style.display = "none";
  activeMenuNode = null;
}

function addMenuLabel(text) {
  const el = document.createElement("div");
  el.className = "pane-menu-sub-label";
  el.textContent = text;
  menuEl.appendChild(el);
}

function addMenuItem(text, fn) {
  const el = document.createElement("div");
  el.className = "pane-menu-item";
  el.innerHTML = text;
  el.addEventListener("click", (e) => { e.stopPropagation(); fn(); closeMenu(); });
  menuEl.appendChild(el);
}

function addMenuSep() {
  const el = document.createElement("hr");
  el.className = "pane-menu-sep";
  menuEl.appendChild(el);
}

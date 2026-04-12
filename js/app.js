// =====================================================================
// MAIN APP
// =====================================================================

// =====================================================================
// WEBSOCKET
// =====================================================================
const ws = new WebSocket("ws://localhost:1337");

ws.addEventListener("open", () => {
  console.log("Connecté au serveur WebSocket");
  ws.send("get");
});

ws.addEventListener("message", (event) => {
  try {
    const data = JSON.parse(event.data);
    registry = structuredClone(data["types"]);
    refreshAllPanes("treeview");
    console.log("Registry mis à jour :", registry);
  } catch (err) {
    console.error("Erreur de parsing JSON :", err);
  }
});

// =====================================================================
// STARTUP
// =====================================================================
initLayout();

loadConfig().then(() => {
  forEachLeaf(layoutRoot, n => {
    if (n.source === "jsoneditor") renderPaneContent(n);
  });
});

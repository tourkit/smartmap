// =====================================================================
// MAIN APP
// =====================================================================

// =====================================================================
// WEBSOCKET
// =====================================================================
const ws = new WebSocket("ws://localhost:1337");

ws.addEventListener("open", () => {
  console.log("Connecté au serveur WebSocket");
  ws.send("registre");
});

ws.addEventListener("message", (event) => {
  try {
    const data = JSON.parse(event.data);
    registry = structuredClone(data[0]);
 
    refreshAllPanes("treeview");
    console.log("Registry mis à jour :", registry);
  } catch (err) {
    console.error("Erreur de parsing JSdssdfON :", err);
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

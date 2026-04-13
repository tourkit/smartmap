// =====================================================================
// MAIN APP
// =====================================================================

let fileList = [];

// =====================================================================
// WEBSOCKET
// =====================================================================
const ws = new WebSocket("ws://localhost:1337");

ws.addEventListener("open", () => {
  console.log("Connecté au serveur WebSocket");
  ws.send("registre");
  ws.send("files");
});

ws.addEventListener("message", (event) => {

    const message = JSON.parse(event.data);
    
    if (message["type"] === "registre") {
      registry = structuredClone(message["body"]);
      refreshAllPanes("treeview");
      console.log("Registry mis à jour :", registry);
    }
    if (message["type"] === "files") {
      fileList = structuredClone(message["body"]);

      message["body"].forEach(e => {

        fileList.push(e["path"])
        
      });
      refreshAllPanes("treeview");
      console.log("Registry mis à jour :", registry);
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

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
  //ws.send("{\"type\": \"registre\", \"data\": [123]}");
  //ws.send("{\"type\": \"files\"}");
});

ws.addEventListener("message", (event) => {

    if (!(typeof event.data === "string"))
        return;
    const message = JSON.parse(event.data);
    
    if (message["type"] === "registre") {
      registry = structuredClone(message["body"]);
      refreshAllPanes("treeview");
    }
    else if (message["type"] === "files") {
      fileList = structuredClone(message["body"]);

      message["body"].forEach(e => {

        fileList.push(e["path"])
        
      });
      refreshAllPanes("treeview");
    }else 
      console.log(message)

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

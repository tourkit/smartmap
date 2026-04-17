

// =====================================================================
// WEBSOCKET
// =====================================================================
const ws = new WebSocket("ws://localhost:1337");

ws.binaryType = "arraybuffer";

ws.addEventListener("message", (event) => {

    if (!(typeof event.data === "string")) {

      const buffer = event.data; // ArrayBuffer
      const view = new Uint8Array(buffer);
      console.log(view);

      for (const cb of broadcastCallbacks) {
        cb(event.data);
      }
      return;
    }
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
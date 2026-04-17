// =====================================================================
// TREE VIEW
// =====================================================================
function renderTree(container) {
  container.innerHTML = "";

  const tree = document.createElement("ul");
  tree.className = "tree-list";

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

  const timelineLi = document.createElement("li");
  timelineLi.className = "tree-category";

  const timelineHeader = document.createElement("div");
  timelineHeader.className = "tree-category-header";
  timelineHeader.innerHTML = `<span class="tree-toggle">▶</span> <span>⏱ Timelines</span>`;
  timelineLi.appendChild(timelineHeader);

  const timelineChildren = document.createElement("ul");
  timelineChildren.className = "tree-children";

  if (typeof timelinesData !== "undefined" && timelinesData.length > 0) {
    timelinesData.forEach(t => {
      const tli = document.createElement("li");
      tli.className = "tree-item";
      tli.innerHTML = `<span class="tree-item-name">⏱ Timeline ${t.id}</span>`;
      tli.onclick = () => {
        selectedTimelineId = t.id;
        openTimelineEditor();
      };
      timelineChildren.appendChild(tli);
    });
  } else {
    const emptyLi = document.createElement("li");
    emptyLi.className = "tree-item";
    emptyLi.style.color = "#555";
    emptyLi.textContent = "(no timelines)";
    timelineChildren.appendChild(emptyLi);
  }

  timelineLi.appendChild(timelineChildren);
  tree.appendChild(timelineLi);

  timelineHeader.onclick = () => {
    timelineHeader.querySelector(".tree-toggle").classList.toggle("expanded");
    timelineChildren.classList.toggle("visible");
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
  name.title = node.desc || "";

  row.appendChild(toggle);
  row.appendChild(name);
  li.appendChild(row);

  if (node.children && node.children.length > 0) {
    const childrenContainer = document.createElement("ul");
    childrenContainer.className = "tree-children";

    node.children.forEach(child => {
      renderHierarchyList(child, childrenContainer);
    });

    li.appendChild(childrenContainer);
    childrenContainer.classList.remove("visible");

    toggle.onclick = () => {
      toggle.classList.toggle("expanded");
      toggle.textContent = toggle.classList.contains("expanded") ? "▼" : "▶";
      childrenContainer.classList.toggle("visible");
    };
    name.onclick = toggle.onclick;
  } else {
    toggle.style.visibility = "hidden";
  }

  container.appendChild(li);
}
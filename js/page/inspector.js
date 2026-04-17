// =====================================================================
// INSPECTOR VIEW
// =====================================================================
function renderInspector(container) {
  if (!selected) {
    container.innerHTML = `<div class="title">Inspector</div><div class="box">Drop a field here or select a struct</div>`;
    return;
  }

  const struct = selected;
  const tm = typeMap();
  container.innerHTML = `
    <div class="title">📦 ${struct.name}</div>
    <div class="box">
      ${struct.fields.map(f => `
        <div class="row">
          <input value="${f.label}" onchange="updateField('${struct.name}','${f.label}',this.value,'label')" />
          <select onchange="updateField('${struct.name}','${f.label}',this.value,'type')">
            ${["int","float","string","bool",...Object.keys(tm)]
              .map(t => `<option ${t===f.type?"selected":""}>${t}</option>`).join("")}
          </select>
        </div>
      `).join("")}
    </div>
  `;
}

function selectStruct(struct) {
  selected = struct;
  refreshAllPanes("inspector");
}

window.updateField = function(structName, oldLabel, value, prop) {
  const s = registry.find(x => x.name === structName);
  if (!s) return;
  const f = s.fields.find(x => x.label === oldLabel);
  if (!f) return;
  f[prop] = value;
  refreshAllPanes("treeview");
};
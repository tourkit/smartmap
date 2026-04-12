// =====================================================================
// DATA
// =====================================================================
let registry = [
  { name: "3Dello", fields: [{ label:"x",type:"float" },{ label:"y",type:"float" },{ label:"z",type:"float" }] },
  { name: "Objet",  fields: [{ label:"coord",type:"3D" },{ label:"RGBA",type:"int",quantity:4 }] }
];

let buffer = [
  { name: "Cell", type: null, fields: [] },
  { name: "Line", type: null, fields: [] }
];

const typeMap = () => Object.fromEntries([...registry,...buffer].map(t=>[t.name,t]));

function resolveTypeRef(typeName) {
  const tm = typeMap();
  return tm[typeName] || null;
}

function buildTypeHierarchy(typeName, depth = 0) {
  if (depth > 10) return null;
  const struct = resolveTypeRef(typeName);
  if (!struct) return null;
  
  const node = {
    name: struct.name,
    type: typeName,
    depth,
    expanded: false,
    children: []
  };
  
  for (const field of struct.fields) {
    const childType = resolveTypeRef(field.type);
    if (childType) {
      node.children.push({
        name: field.label,
        type: field.type,
        depth: depth + 1,
        expanded: false,
        children: buildTypeHierarchy(field.type, depth + 1)
      });
    }
  }
  
  return node;
}

// Shared inspector state
let selected = null;

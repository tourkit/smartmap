// =====================================================================
// DATA
// =====================================================================
let registry = [
  { name: "3Dello", fields: [{ label:"x",type:"float" },{ label:"y",type:"float" },{ label:"z",type:"float" }] },
  { name: "Objet",  fields: [{ label:"coord",type:"3D" },{ label:"RGBA",type:"int",quantity:4 }] }
];

let buffer = [
  { name: "Cell", fields: [{ label:"value",type:"int" },{ label:"state",type:"bool" }] },
  { name: "Line", fields: [{ label:"start",type:"3D" },{ label:"end",type:"3D" }] }
];

const typeMap = () => Object.fromEntries([...registry,...buffer].map(t=>[t.name,t]));

// Shared inspector state
let selected = null;

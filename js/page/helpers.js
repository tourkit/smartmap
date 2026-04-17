// =====================================================================
// UI HELPERS
// =====================================================================
function createLabelInput(label, type, opts = {}) {
  const l = document.createElement("label");
  l.textContent = label + ":";
  const input = document.createElement("input");
  input.type = type;
  if (opts.value) input.value = opts.value;
  if (opts.min) input.min = opts.min;
  if (opts.max) input.max = opts.max;
  if (opts.className) input.className = opts.className;
  l.appendChild(input);
  return { label: l, input };
}

function createCheckbox(labelText, opts = {}) {
  const l = document.createElement("label");
  l.title = opts.title || "";
  const input = document.createElement("input");
  input.type = "checkbox";
  if (opts.checked) input.checked = true;
  if (opts.className) input.className = opts.className;
  const span = document.createElement("span");
  span.textContent = labelText;
  l.appendChild(input);
  l.appendChild(span);
  return { label: l, input };
}

function createRadioGroup(name, values, defaultValue) {
  const div = document.createElement("div");
  div.className = "buffer-style-radios";
  values.forEach(v => {
    const l = document.createElement("label");
    const radio = document.createElement("input");
    radio.type = "radio";
    radio.name = name;
    radio.value = v;
    if (v === defaultValue) radio.checked = true;
    l.appendChild(radio);
    l.appendChild(document.createTextNode(v));
    div.appendChild(l);
  });
  return { div, getValue: () => div.querySelector(`input[name="${name}"]:checked`).value };
}
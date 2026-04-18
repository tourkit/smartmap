let timelinesData = [];
let selectedTimelineId = null;

async function loadTimelines() {
    try {
        console.log("Loading timelines...");
        const res = await fetch("/api/timelines");
        console.log("Response:", res);
        if (res.ok) {
            const data = await res.json();
            console.log("Timelines data:", data);
            timelinesData = data;
            refreshAllPanes("timeline");
        }
    } catch (e) {
        console.error("Failed to load timelines:", e);
    }
}

function renderTimelineEditor(node, container) {
    container.innerHTML = "";
    
    if (timelinesData.length === 0) {
        loadTimelines().then(() => renderPaneContent(node));
        container.innerHTML = "<div style='padding:20px;color:#666;'>Loading...</div>";
        return;
    }

    const wrap = document.createElement("div");
    wrap.className = "view-timeline-editor";

    const toolbar = document.createElement("div");
    toolbar.className = "timeline-toolbar";

    const addBtn = document.createElement("button");
    addBtn.textContent = "+ Add Timeline";
    addBtn.onclick = async () => {
        await fetch("/api/timelines?action=create&duration=5000&looping=true", { method: "POST" });
        await loadTimelines();
    };

    const refreshBtn = document.createElement("button");
    refreshBtn.textContent = "⟳ Refresh";
    refreshBtn.onclick = loadTimelines;

    toolbar.appendChild(addBtn);
    toolbar.appendChild(refreshBtn);
    wrap.appendChild(toolbar);

    const timelineList = document.createElement("div");
    timelineList.className = "timeline-list";

    timelinesData.forEach(t => {
        const item = document.createElement("div");
        item.className = "timeline-item" + (t.id === selectedTimelineId ? " selected" : "");
        
        const header = document.createElement("div");
        header.className = "timeline-item-header";
        header.onclick = () => {
            selectedTimelineId = t.id;
            renderTimelineEditor(node, container);
        };

        const title = document.createElement("span");
        title.className = "timeline-title";
        title.textContent = `Timeline ${t.id}`;

        const controls = document.createElement("span");
        controls.className = "timeline-controls";

        const playBtn = document.createElement("button");
        playBtn.textContent = "▶";
        playBtn.title = "Play";
        
        const deleteBtn = document.createElement("button");
        deleteBtn.textContent = "✕";
        deleteBtn.title = "Delete";
        deleteBtn.onclick = async (e) => {
            e.stopPropagation();
            if (confirm("Delete timeline " + t.id + "?")) {
                await fetch("/api/timelines?action=delete&id=" + t.id, { method: "POST" });
                await loadTimelines();
            }
        };

        controls.appendChild(playBtn);
        controls.appendChild(deleteBtn);

        header.appendChild(title);
        header.appendChild(controls);
        item.appendChild(header);

        if (t.id === selectedTimelineId) {
            const editor = document.createElement("div");
            editor.className = "timeline-editor";

            const props = document.createElement("div");
            props.className = "timeline-props";

            const durationRow = document.createElement("div");
            durationRow.className = "timeline-prop-row";
            durationRow.innerHTML = `<label>Duration (ms):</label>`;
            const durationInput = document.createElement("input");
            durationInput.type = "number";
            durationInput.value = t.duration;
            durationInput.onchange = async () => {
                await fetch("/api/timelines?action=update&id=" + t.id + "&duration=" + durationInput.value, { method: "POST" });
                await loadTimelines();
            };
            durationRow.appendChild(durationInput);
            props.appendChild(durationRow);

            const loopingRow = document.createElement("div");
            loopingRow.className = "timeline-prop-row";
            loopingRow.innerHTML = `<label>Looping:</label>`;
            const loopingCheck = document.createElement("input");
            loopingCheck.type = "checkbox";
            loopingCheck.checked = t.looping;
            loopingCheck.onchange = async () => {
                await fetch("/api/timelines?action=update&id=" + t.id + "&looping=" + loopingCheck.checked, { method: "POST" });
                await loadTimelines();
            };
            loopingRow.appendChild(loopingCheck);
            props.appendChild(loopingRow);

            const currentRow = document.createElement("div");
            currentRow.className = "timeline-prop-row";
            currentRow.innerHTML = `<label>Current (ms):</label>`;
            const currentInput = document.createElement("input");
            currentInput.type = "number";
            currentInput.value = t.current;
            currentInput.onchange = async () => {
                await fetch("/api/timelines?action=update&id=" + t.id + "&current=" + currentInput.value, { method: "POST" });
                await loadTimelines();
            };
            currentRow.appendChild(currentInput);
            props.appendChild(currentRow);

            editor.appendChild(props);

            const keysHeader = document.createElement("div");
            keysHeader.className = "timeline-keys-header";
            keysHeader.textContent = "Keys";
            editor.appendChild(keysHeader);

            const keysList = document.createElement("div");
            keysList.className = "timeline-keys-list";

            t.keys.forEach((k, idx) => {
                const keyItem = document.createElement("div");
                keyItem.className = "timeline-key-item";

                const posInput = document.createElement("input");
                posInput.type = "number";
                posInput.value = k.position;
                posInput.placeholder = "position (ms)";
                posInput.onchange = () => updateKey(t.id, idx, posInput.value, valInput.value, typeSelect.value);

                const valInput = document.createElement("input");
                valInput.type = "number";
                valInput.step = "0.1";
                valInput.value = k.value;
                valInput.placeholder = "value";
                valInput.onchange = () => updateKey(t.id, idx, posInput.value, valInput.value, typeSelect.value);

                const typeSelect = document.createElement("select");
                ["LINEAR", "SINUS", "TRIANGLE", "SQUARE"].forEach((type, i) => {
                    const opt = document.createElement("option");
                    opt.value = i;
                    opt.textContent = type;
                    if (k.type === i) opt.selected = true;
                    typeSelect.appendChild(opt);
                });
                typeSelect.onchange = () => updateKey(t.id, idx, posInput.value, valInput.value, typeSelect.value);

                const delBtn = document.createElement("button");
                delBtn.textContent = "✕";
                delBtn.onclick = () => deleteKey(t.id, idx);

                keyItem.appendChild(posInput);
                keyItem.appendChild(valInput);
                keyItem.appendChild(typeSelect);
                keyItem.appendChild(delBtn);
                keysList.appendChild(keyItem);
            });

            const addKeyBtn = document.createElement("button");
            addKeyBtn.textContent = "+ Add Key";
            addKeyBtn.onclick = () => addKey(t.id);
            keysList.appendChild(addKeyBtn);

            editor.appendChild(keysList);

            const canvasWrap = document.createElement("div");
            canvasWrap.className = "timeline-canvas-wrap";
            const canvas = document.createElement("canvas");
            canvas.width = 600;
            canvas.height = 200;
            canvasWrap.appendChild(canvas);
            editor.appendChild(canvasWrap);

            drawTimeline(canvas, t);

            item.appendChild(editor);
        }

        timelineList.appendChild(item);
    });

    wrap.appendChild(timelineList);
    container.appendChild(wrap);

    setInterval(() => loadTimelines(), 1000);
}

async function updateKey(timelineId, keyIdx, position, value, type) {
    const t = timelinesData.find(t => t.id === timelineId);
    if (!t) return;
    const keys = [...t.keys];
    keys[keyIdx] = { position: parseInt(position), value: parseFloat(value), type: parseInt(type) };
    await fetch("/api/timelines?action=update&id=" + timelineId + "&keys=" + JSON.stringify(keys), { method: "POST" });
    await loadTimelines();
}

async function addKey(timelineId) {
    const t = timelinesData.find(t => t.id === timelineId);
    if (!t) return;
    const keys = [...t.keys, { position: t.duration / 2, value: 0.5, type: 0 }];
    await fetch("/api/timelines?action=update&id=" + timelineId + "&keys=" + JSON.stringify(keys), { method: "POST" });
    await loadTimelines();
}

async function deleteKey(timelineId, keyIdx) {
    const t = timelinesData.find(t => t.id === timelineId);
    if (!t) return;
    const keys = t.keys.filter((_, i) => i !== keyIdx);
    await fetch("/api/timelines?action=update&id=" + timelineId + "&keys=" + JSON.stringify(keys), { method: "POST" });
    await loadTimelines();
}

function drawTimeline(canvas, timeline) {
    const ctx = canvas.getContext("2d");
    const w = canvas.width;
    const h = canvas.height;

    ctx.fillStyle = "#1a1a1a";
    ctx.fillRect(0, 0, w, h);

    ctx.strokeStyle = "#333";
    ctx.lineWidth = 1;
    for (let i = 0; i <= 10; i++) {
        const y = i * h / 10;
        ctx.beginPath();
        ctx.moveTo(0, y);
        ctx.lineTo(w, y);
        ctx.stroke();
    }

    if (timeline.keys.length < 2) return;

    ctx.strokeStyle = "#00ff88";
    ctx.lineWidth = 2;
    ctx.beginPath();

    const xScale = w / timeline.duration;
    const yScale = h;

    for (let px = 0; px < w; px++) {
        const ms = px / xScale;
        const val = sampleTimeline(timeline, ms);
        const py = h - (val * yScale);
        if (px === 0) ctx.moveTo(px, py);
        else ctx.lineTo(px, py);
    }
    ctx.stroke();

    timeline.keys.forEach(k => {
        const kx = k.position * xScale;
        const ky = h - (k.value * yScale);
        ctx.fillStyle = "#ff6b35";
        ctx.beginPath();
        ctx.arc(kx, ky, 6, 0, Math.PI * 2);
        ctx.fill();
    });

    const curX = timeline.current * xScale;
    ctx.strokeStyle = "#fff";
    ctx.lineWidth = 1;
    ctx.setLineDash([4, 4]);
    ctx.beginPath();
    ctx.moveTo(curX, 0);
    ctx.lineTo(curX, h);
    ctx.stroke();
    ctx.setLineDash([]);
}

function sampleTimeline(timeline, ms) {
    const keys = timeline.keys;
    if (!keys.length) return 0;
    if (ms <= keys[0].position) return keys[0].value;
    if (ms >= keys[keys.length - 1].position) return keys[keys.length - 1].value;

    for (let i = 0; i < keys.length - 1; i++) {
        const k0 = keys[i];
        const k1 = keys[i + 1];
        if (ms >= k0.position && ms <= k1.position) {
            let t = (ms - k0.position) / (k1.position - k0.position);
            t = ease(t, k0.type);
            return k0.value + (k1.value - k0.value) * t;
        }
    }
    return 0;
}

function ease(t, type) {
    switch (type) {
        case 0: return t;
        case 1: return 0.5 - 0.5 * Math.cos(t * Math.PI);
        case 2: return t < 0.5 ? 2 * t : 2 * (1 - t);
        case 3: return t < 0.5 ? 0 : 1;
        default: return t;
    }
}

function openTimelineEditor() {
    let target = null;
    forEachLeaf(layoutRoot, n => {
        if (!target && n.source === "timeline") target = n;
    });
    if (target) {
        renderPaneContent(target);
        return;
    }
    forEachLeaf(layoutRoot, n => {
        if (!target && n.source === "inspector") target = n;
    });
    if (!target) {
        forEachLeaf(layoutRoot, n => { if (!target) target = n; });
    }
    if (target) {
        target.source = "timeline";
        target._title.textContent = "⏱ Timeline";
        renderPaneContent(target);
        loadTimelines();
    }
}
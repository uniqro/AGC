const ui = {};
const state = {
  debug: null,
  selectedFrame: 0,
  signalWindowMs: 100,
  metricKey: "smoothed_rms",
};

const metricLabels = {
  smoothed_rms: "Smoothed RMS",
  envelope: "Envelope",
  desired_gain: "Desired Gain",
  applied_gain: "Applied Gain",
  crest_factor_db: "Crest Factor",
  smoothed_crest_factor_db: "Smoothed Crest Factor",
  cf_blend_weight: "CF Blend Weight",
  stage_output_peak: "Output Peak",
  stage_output_rms: "Output RMS",
};

const metricValueFormatters = {
  gate_open: (v) => (v ? "open" : "closed"),
  headroom_limited: (v) => (v ? "yes" : "no"),
  limiter_active: (v) => (v ? "yes" : "no"),
  compressor_active: (v) => (v ? "yes" : "no"),
  peak_protector_active: (v) => (v ? "yes" : "no"),
};

function bindUi() {
  [
    "jsonFile", "statusBox", "timeWindowMs", "metricSelect", "frameRange", "prevFrame", "nextFrame",
    "summaryMode", "summaryRate", "summaryTarget", "summaryPeak", "summaryLimiter", "summaryFrames",
    "signalReadout", "frameReadout", "frameTimeReadout", "metricGrid", "decisionSummary", "decisionList",
    "stageGrid", "inputScope", "outputScope", "metricScope", "frameInputScope", "frameOutputScope",
  ].forEach((id) => {
    ui[id] = document.getElementById(id);
  });
}

function setStatus(text) {
  ui.statusBox.textContent = text;
}

function clamp(value, min, max) {
  return Math.min(max, Math.max(min, value));
}

function formatNumber(value, digits = 3) {
  return Number(value).toFixed(digits);
}

function drawWave(canvas, samples, color, highlightRange = null) {
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  const height = canvas.height;
  ctx.clearRect(0, 0, width, height);
  ctx.fillStyle = "#08111d";
  ctx.fillRect(0, 0, width, height);

  if (highlightRange) {
    ctx.fillStyle = "rgba(255,255,255,0.06)";
    ctx.fillRect(highlightRange.x, 0, highlightRange.width, height);
  }

  ctx.strokeStyle = "rgba(255,255,255,0.08)";
  ctx.beginPath();
  ctx.moveTo(0, height / 2);
  ctx.lineTo(width, height / 2);
  ctx.stroke();

  if (!samples || samples.length === 0) {
    return;
  }

  ctx.strokeStyle = color;
  ctx.lineWidth = 1.5;
  ctx.beginPath();
  for (let i = 0; i < samples.length; i++) {
    const x = (i / Math.max(samples.length - 1, 1)) * width;
    const y = height * 0.5 - samples[i] * height * 0.42;
    if (i === 0) {
      ctx.moveTo(x, y);
    } else {
      ctx.lineTo(x, y);
    }
  }
  ctx.stroke();
}

function drawMetric(canvas, frames, key, selectedIndex) {
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  const height = canvas.height;
  ctx.clearRect(0, 0, width, height);
  ctx.fillStyle = "#08111d";
  ctx.fillRect(0, 0, width, height);

  if (!frames || frames.length === 0) {
    return;
  }

  const values = frames.map((frame) => Number(frame[key] ?? 0));
  const min = Math.min(...values);
  const max = Math.max(...values);
  const span = Math.max(max - min, 1e-6);

  ctx.strokeStyle = "rgba(255,255,255,0.08)";
  ctx.beginPath();
  for (let i = 0; i <= 4; i++) {
    const y = (i / 4) * height;
    ctx.moveTo(0, y);
    ctx.lineTo(width, y);
  }
  ctx.stroke();

  ctx.strokeStyle = "#69a8ff";
  ctx.lineWidth = 1.5;
  ctx.beginPath();
  values.forEach((value, index) => {
    const x = (index / Math.max(values.length - 1, 1)) * width;
    const y = height - ((value - min) / span) * (height - 12) - 6;
    if (index === 0) {
      ctx.moveTo(x, y);
    } else {
      ctx.lineTo(x, y);
    }
  });
  ctx.stroke();

  frames.forEach((frame, index) => {
    const x = (index / Math.max(frames.length - 1, 1)) * width;
    if (frame.limiter_active) {
      ctx.fillStyle = "rgba(255,125,125,0.8)";
      ctx.fillRect(x - 1, height - 18, 2, 18);
    }
    if (frame.headroom_limited) {
      ctx.fillStyle = "rgba(255,213,106,0.7)";
      ctx.fillRect(x - 1, 0, 2, 14);
    }
    if (frame.gate_open) {
      ctx.fillStyle = "rgba(105,227,173,0.45)";
      ctx.fillRect(x - 1, height - 6, 2, 6);
    }
  });

  const selectedX = (selectedIndex / Math.max(frames.length - 1, 1)) * width;
  ctx.strokeStyle = "#ffffff";
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(selectedX, 0);
  ctx.lineTo(selectedX, height);
  ctx.stroke();
}

function metricItems(frame) {
  return [
    ["Input Peak", frame.input_peak],
    ["Input RMS", frame.input_rms],
    ["Envelope", frame.envelope],
    ["Smoothed RMS", frame.smoothed_rms],
    ["Crest Factor", `${formatNumber(frame.crest_factor_db, 2)} dB`],
    ["Smoothed Crest", `${formatNumber(frame.smoothed_crest_factor_db, 2)} dB`],
    ["Desired Gain", `${formatNumber(frame.desired_gain, 3)}x`],
    ["Applied Gain", `${formatNumber(frame.applied_gain, 3)}x`],
    ["CF Blend", formatNumber(frame.cf_blend_weight, 3)],
    ["Gate", frame.gate_open],
    ["Headroom Limited", frame.headroom_limited],
    ["Limiter Active", frame.limiter_active],
  ];
}

function renderMetricGrid(frame) {
  ui.metricGrid.innerHTML = "";
  metricItems(frame).forEach(([name, rawValue]) => {
    const item = document.createElement("div");
    item.className = "metric-item";
    const label = document.createElement("div");
    label.className = "name";
    label.textContent = name;
    const value = document.createElement("div");
    value.className = "value";
    if (typeof rawValue === "boolean") {
      value.textContent = rawValue ? "yes" : "no";
    } else if (typeof rawValue === "number") {
      value.textContent = formatNumber(rawValue);
    } else {
      value.textContent = rawValue;
    }
    item.append(label, value);
    ui.metricGrid.appendChild(item);
  });
}

function renderStageGrid(frame) {
  const stages = [
    ["Input Peak", frame.stage_input_peak],
    ["After Gain", frame.stage_gain_peak],
    ["After Compressor", frame.stage_compressor_peak],
    ["After Peak Protector", frame.stage_peak_protector_peak],
    ["Output Peak", frame.stage_output_peak],
    ["Output RMS", frame.stage_output_rms],
  ];
  ui.stageGrid.innerHTML = "";
  stages.forEach(([labelText, valueText]) => {
    const item = document.createElement("div");
    item.className = "stage-card";
    const label = document.createElement("div");
    label.className = "label";
    label.textContent = labelText;
    const value = document.createElement("div");
    value.className = "value";
    value.textContent = formatNumber(valueText);
    item.append(label, value);
    ui.stageGrid.appendChild(item);
  });
}

function renderDecision(frame, debug) {
  const targetPeak = debug.metadata.target_peak;
  const limiter = debug.metadata.limiter_threshold;
  const bullets = [];

  if (frame.gate_open) {
    bullets.push(`Gate acik. Envelope ${formatNumber(frame.envelope)} ve smoothed RMS ${formatNumber(frame.smoothed_rms)} ile frame aktif kabul edildi.`);
  } else {
    bullets.push(`Gate kapali. Bu frame hedef seviyeye cekilmeden geciyor.`);
  }

  bullets.push(`Smoothed crest factor ${formatNumber(frame.smoothed_crest_factor_db, 2)} dB. CF blend agirligi ${formatNumber(frame.cf_blend_weight, 3)}.`);
  bullets.push(`Desired gain ${formatNumber(frame.desired_gain, 3)}x, applied gain ${formatNumber(frame.applied_gain, 3)}x.`);

  if (frame.headroom_limited) {
    bullets.push(`Peak-aware yol RMS yolundan daha kisitlayici oldu. Target peak ${formatNumber(targetPeak)} seviyesi devrede.`);
  } else {
    bullets.push(`Bu frame'de gain karari daha cok RMS tarafindan surukleniyor.`);
  }

  if (frame.limiter_active) {
    bullets.push(`Final limiter aktif. Son koruma asamasinda ${formatNumber(frame.limiter_reduction, 4)} kadar reduction olustu.`);
  } else {
    bullets.push(`Final limiter aktif degil. Output peak ${formatNumber(frame.stage_output_peak)} ile limiter esiginin altinda kaldi.`);
  }

  if (frame.compressor_active) {
    bullets.push(`Compressor aktif, ancak mevcut AM deneyinde bunun pasif kalmasi bekleniyor.`);
  }

  ui.decisionSummary.textContent =
    `Bu frame icin hedef RMS ${formatNumber(debug.config.target_rms_fs)} ve limiter esigi ${formatNumber(limiter)}. Karar, aktif-frame RMS ile crest-factor-aware peak sinirlamasi arasinda verildi.`;
  ui.decisionList.innerHTML = "";
  bullets.forEach((text) => {
    const li = document.createElement("li");
    li.textContent = text;
    ui.decisionList.appendChild(li);
  });
}

function updateSummary(debug) {
  ui.summaryMode.textContent = debug.metadata.mode;
  ui.summaryRate.textContent = `${debug.metadata.sample_rate} Hz`;
  ui.summaryTarget.textContent = `${Math.round(debug.metadata.target_rms_percent)}%`;
  ui.summaryPeak.textContent = formatNumber(debug.metadata.target_peak);
  ui.summaryLimiter.textContent = formatNumber(debug.metadata.limiter_threshold);
  ui.summaryFrames.textContent = String(debug.summary.frames);
}

function getWholeWindowSamples(samples, centerSample, windowSamples) {
  const half = Math.floor(windowSamples / 2);
  let start = centerSample - half;
  if (start < 0) {
    start = 0;
  }
  let end = Math.min(samples.length, start + windowSamples);
  start = Math.max(0, end - windowSamples);
  return {
    start,
    end,
    samples: samples.slice(start, end),
  };
}

function updateScopes() {
  const debug = state.debug;
  const frame = debug.frames[state.selectedFrame];
  const sampleRate = debug.metadata.sample_rate;
  const windowSamples = Math.max(1, Math.round(sampleRate * (state.signalWindowMs / 1000)));
  const centerSample = frame.start_sample;

  const inputWindow = getWholeWindowSamples(debug.input_samples, centerSample, windowSamples);
  const outputWindow = getWholeWindowSamples(debug.output_samples, centerSample, windowSamples);

  const highlightX = ((frame.start_sample - inputWindow.start) / Math.max(inputWindow.samples.length, 1)) * ui.inputScope.width;
  const highlightWidth = Math.max(2, ((debug.metadata.frame_samples) / Math.max(inputWindow.samples.length, 1)) * ui.inputScope.width);

  drawWave(ui.inputScope, inputWindow.samples, "#69a8ff", { x: highlightX, width: highlightWidth });
  drawWave(ui.outputScope, outputWindow.samples, "#69e3ad", { x: highlightX, width: highlightWidth });
  drawWave(ui.frameInputScope, frame.frame_input, "#69a8ff");
  drawWave(ui.frameOutputScope, frame.frame_output, "#69e3ad");
  drawMetric(ui.metricScope, debug.frames, state.metricKey, state.selectedFrame);

  ui.signalReadout.textContent = `${state.signalWindowMs} ms view, frame highlight active`;
  ui.frameReadout.textContent = `Frame ${frame.index + 1} / ${debug.frames.length}`;
  ui.frameTimeReadout.textContent =
    `${formatNumber(frame.start_sample / sampleRate, 3)} s - ${formatNumber(frame.end_sample / sampleRate, 3)} s`;
}

function renderSelectedFrame() {
  const debug = state.debug;
  const frame = debug.frames[state.selectedFrame];
  renderMetricGrid(frame);
  renderStageGrid(frame);
  renderDecision(frame, debug);
  updateScopes();
}

function loadDebug(debug) {
  state.debug = debug;
  state.selectedFrame = 0;
  updateSummary(debug);
  ui.frameRange.min = "0";
  ui.frameRange.max = String(Math.max(debug.frames.length - 1, 0));
  ui.frameRange.value = "0";
  renderSelectedFrame();
  setStatus(`Loaded ${debug.metadata.input_file}. ${debug.summary.frames} frame hazir.`);
}

async function handleJsonFile(event) {
  const [file] = event.target.files;
  if (!file) {
    return;
  }
  const text = await file.text();
  const debug = JSON.parse(text);
  loadDebug(debug);
}

function attachEvents() {
  ui.jsonFile.addEventListener("change", handleJsonFile);
  ui.timeWindowMs.addEventListener("change", () => {
    state.signalWindowMs = parseInt(ui.timeWindowMs.value, 10);
    if (state.debug) {
      updateScopes();
    }
  });
  ui.metricSelect.addEventListener("change", () => {
    state.metricKey = ui.metricSelect.value;
    if (state.debug) {
      updateScopes();
    }
  });
  ui.frameRange.addEventListener("input", () => {
    state.selectedFrame = parseInt(ui.frameRange.value, 10);
    if (state.debug) {
      renderSelectedFrame();
    }
  });
  ui.prevFrame.addEventListener("click", () => {
    if (!state.debug) {
      return;
    }
    state.selectedFrame = clamp(state.selectedFrame - 1, 0, state.debug.frames.length - 1);
    ui.frameRange.value = String(state.selectedFrame);
    renderSelectedFrame();
  });
  ui.nextFrame.addEventListener("click", () => {
    if (!state.debug) {
      return;
    }
    state.selectedFrame = clamp(state.selectedFrame + 1, 0, state.debug.frames.length - 1);
    ui.frameRange.value = String(state.selectedFrame);
    renderSelectedFrame();
  });
}

function init() {
  bindUi();
  attachEvents();
  setStatus("Hazir. JSON dosyasini yukleyince giris/cikis waveform, timeline metric ve frame karar izi gorunecek.");
}

init();

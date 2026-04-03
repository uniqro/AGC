const defaults = {
  targetRms: 42,
  maxGainDb: 12,
  gateThreshold: 5,
  gateHoldMs: 80,
  attackMs: 8,
  releaseMs: 300,
  envelopeWindowMs: 4,
  rmsWindowMs: 8,
  peakProtectorRatio: 97,
  limiterThreshold: 98,
  compressorThresholdDb: -12,
  compressorRatio: 3,
  scopeWidth: 560,
  scopeHeight: 180,
  scopeWindowSelect: 10,
  agcEnabled: true,
  monitorEnabled: true,
  compressorEnabled: false
};

const ui = {};
const state = {
  audioContext: null,
  mediaStream: null,
  mediaSource: null,
  processor: null,
  outputGain: null,
  running: false,
  envelope: 0,
  smoothedRms: 0,
  appliedGain: 1,
  gateOpen: false,
  gateHoldSamples: 0,
  latestInputBlock: new Float32Array(256),
  latestOutputBlock: new Float32Array(256),
  latestInputPeak: 0,
  latestInputRms: 0,
  latestOutputPeak: 0,
  latestOutputRms: 0,
  latestDesiredGain: 1,
  latestPeakProtected: false,
  latestLimiter: false,
  historyLength: 0,
  historyIndex: 0,
  inputHistory: null,
  outputHistory: null,
  historySampleRate: 0,
  animationDecimation: 3,
  animationCounter: 0,
  recording: false,
  recordInputChunks: [],
  recordOutputChunks: [],
  recordInputSamples: 0,
  recordOutputSamples: 0
};

const scopeWindowsMs = [10, 100, 500, 1000, 2000];

function clamp(v, min, max) {
  return Math.min(max, Math.max(min, v));
}

function dbToLinear(db) {
  return Math.pow(10, db / 20);
}

function msToAlpha(ms, sampleRate, blockSize) {
  const dtMs = (blockSize / sampleRate) * 1000;
  return clamp(dtMs / (ms + dtMs), 0.0001, 0.9999);
}

function percentToUnit(percent) {
  return clamp(percent / 100, 0, 1);
}

function setStatus(text) {
  ui.statusBox.textContent = text;
}

function floatToInt16(sample) {
  const clamped = clamp(sample, -1, 1);
  return clamped >= 0 ? Math.round(clamped * 32767) : Math.round(clamped * 32768);
}

function createWavBlobFromFloat32(chunks, totalSamples, sampleRate) {
  const dataBytes = totalSamples * 2;
  const buffer = new ArrayBuffer(44 + dataBytes);
  const view = new DataView(buffer);
  let offset = 0;
  let writePos = 44;

  function writeString(text) {
    for (let i = 0; i < text.length; i++) {
      view.setUint8(offset++, text.charCodeAt(i));
    }
  }

  writeString("RIFF");
  view.setUint32(offset, 36 + dataBytes, true); offset += 4;
  writeString("WAVE");
  writeString("fmt ");
  view.setUint32(offset, 16, true); offset += 4;
  view.setUint16(offset, 1, true); offset += 2;
  view.setUint16(offset, 1, true); offset += 2;
  view.setUint32(offset, sampleRate, true); offset += 4;
  view.setUint32(offset, sampleRate * 2, true); offset += 4;
  view.setUint16(offset, 2, true); offset += 2;
  view.setUint16(offset, 16, true); offset += 2;
  writeString("data");
  view.setUint32(offset, dataBytes, true);

  chunks.forEach((chunk) => {
    for (let i = 0; i < chunk.length; i++) {
      view.setInt16(writePos, floatToInt16(chunk[i]), true);
      writePos += 2;
    }
  });

  return new Blob([buffer], { type: "audio/wav" });
}

function downloadBlob(blob, filename) {
  const url = URL.createObjectURL(blob);
  const anchor = document.createElement("a");
  anchor.href = url;
  anchor.download = filename;
  document.body.appendChild(anchor);
  anchor.click();
  document.body.removeChild(anchor);
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}

function resetRecordingBuffers() {
  state.recordInputChunks = [];
  state.recordOutputChunks = [];
  state.recordInputSamples = 0;
  state.recordOutputSamples = 0;
}

function stopRecordingAndDownload() {
  let timestamp;
  if (!state.recording) {
    return;
  }

  state.recording = false;
  ui.recordButton.textContent = "Record";
  ui.recordButton.classList.remove("recording");

  if (!state.audioContext || state.recordInputSamples === 0 || state.recordOutputSamples === 0) {
    resetRecordingBuffers();
    setStatus("Recording stopped, but there was not enough audio to export.");
    return;
  }

  timestamp = new Date().toISOString().replace(/[:.]/g, "-");
  downloadBlob(
    createWavBlobFromFloat32(state.recordInputChunks, state.recordInputSamples, state.audioContext.sampleRate),
    `agc_input_${timestamp}.wav`
  );
  downloadBlob(
    createWavBlobFromFloat32(state.recordOutputChunks, state.recordOutputSamples, state.audioContext.sampleRate),
    `agc_output_${timestamp}.wav`
  );

  resetRecordingBuffers();
  setStatus("Recording stopped. Input and output WAV files were downloaded.");
}

function toggleRecording() {
  if (!state.running) {
    setStatus("Start audio first, then begin recording.");
    return;
  }

  if (state.recording) {
    stopRecordingAndDownload();
    return;
  }

  resetRecordingBuffers();
  state.recording = true;
  ui.recordButton.textContent = "Stop Recording";
  ui.recordButton.classList.add("recording");
  setStatus("Recording input and output. Press Record again to stop and download both WAV files.");
}

function bindUi() {
  [
    "startButton", "stopButton", "recordButton", "resetButton", "statusBox", "inputDevice", "outputDevice",
    "agcEnabled", "monitorEnabled", "compressorEnabled", "targetRms", "maxGainDb", "gateThreshold",
    "gateHoldMs", "attackMs", "releaseMs", "envelopeWindowMs", "rmsWindowMs",
    "peakProtectorRatio", "limiterThreshold", "compressorThresholdDb", "compressorRatio",
    "scopeWidth", "scopeHeight", "scopeWindowSelect",
    "targetRmsValue", "maxGainDbValue", "gateThresholdValue", "gateHoldMsValue",
    "attackMsValue", "releaseMsValue", "envelopeWindowMsValue", "rmsWindowMsValue",
    "peakProtectorRatioValue", "limiterThresholdValue", "compressorThresholdDbValue",
    "compressorRatioValue", "scopeWidthValue", "scopeHeightValue", "scopeSizeReadout", "scopeWindowReadout",
    "inputPeakBar", "inputRmsBar", "outputPeakBar", "outputRmsBar",
    "inputPeakText", "inputRmsText", "outputPeakText", "outputRmsText", "inputState",
    "outputState", "flowEnvelope", "flowRms", "flowDesiredGain", "flowAppliedGain",
    "flowAgcMode", "flowPeakProtector", "flowLimiter", "chipAgc", "chipCompressor",
    "inputScope", "outputScope"
  ].forEach((id) => {
    ui[id] = document.getElementById(id);
  });
}

function updateValueLabels() {
  ui.targetRmsValue.textContent = `${ui.targetRms.value}%`;
  ui.maxGainDbValue.textContent = `${ui.maxGainDb.value} dB`;
  ui.gateThresholdValue.textContent = `${ui.gateThreshold.value}%`;
  ui.gateHoldMsValue.textContent = `${ui.gateHoldMs.value} ms`;
  ui.attackMsValue.textContent = `${ui.attackMs.value} ms`;
  ui.releaseMsValue.textContent = `${ui.releaseMs.value} ms`;
  ui.envelopeWindowMsValue.textContent = `${ui.envelopeWindowMs.value} ms`;
  ui.rmsWindowMsValue.textContent = `${ui.rmsWindowMs.value} ms`;
  ui.peakProtectorRatioValue.textContent = `${ui.peakProtectorRatio.value}%`;
  ui.limiterThresholdValue.textContent = `${ui.limiterThreshold.value}%`;
  ui.compressorThresholdDbValue.textContent = `${ui.compressorThresholdDb.value} dBFS`;
  ui.compressorRatioValue.textContent = `${parseFloat(ui.compressorRatio.value).toFixed(1)}:1`;
  ui.scopeWidthValue.textContent = `${ui.scopeWidth.value} px`;
  ui.scopeHeightValue.textContent = `${ui.scopeHeight.value} px`;
  ui.scopeSizeReadout.textContent =
    `${ui.scopeWidth.value} x ${ui.scopeHeight.value} | ${scopeWindowLabel()}`;
  ui.scopeWindowReadout.textContent = scopeWindowLabel();
  ui.chipAgc.classList.toggle("active", ui.agcEnabled.checked);
  ui.chipCompressor.classList.toggle("active", ui.compressorEnabled.checked);
  applyScopeSize();
}

function scopeWindowLabel() {
  const value = parseInt(ui.scopeWindowSelect.value, 10);
  if (value >= 1000) {
    return value === 1000 ? "1 s" : "2 s";
  }
  return `${value} ms`;
}

function applyScopeSize() {
  const width = parseInt(ui.scopeWidth.value, 10);
  const height = parseInt(ui.scopeHeight.value, 10);
  [
    ui.inputScope,
    ui.outputScope
  ].forEach((canvas) => {
    canvas.width = width;
    canvas.height = height;
    canvas.style.width = `${width}px`;
    canvas.style.height = `${height}px`;
  });
}

function ensureHistory(sampleRate) {
  const desiredLength = Math.max(1, Math.ceil(sampleRate * 2));
  if (state.historyLength === desiredLength &&
      state.inputHistory &&
      state.outputHistory &&
      state.historySampleRate === sampleRate) {
    return;
  }
  state.historyLength = desiredLength;
  state.historySampleRate = sampleRate;
  state.historyIndex = 0;
  state.inputHistory = new Float32Array(desiredLength);
  state.outputHistory = new Float32Array(desiredLength);
}

function pushHistory(inputBlock, outputBlock) {
  if (!state.inputHistory || !state.outputHistory || state.historyLength === 0) {
    return;
  }
  for (let i = 0; i < inputBlock.length; i++) {
    state.inputHistory[state.historyIndex] = inputBlock[i];
    state.outputHistory[state.historyIndex] = outputBlock[i];
    state.historyIndex++;
    if (state.historyIndex >= state.historyLength) {
      state.historyIndex = 0;
    }
  }
}

function getRecentHistory(buffer, sampleCount) {
  const count = Math.min(sampleCount, state.historyLength);
  const out = new Float32Array(count);
  let start = state.historyIndex - count;
  if (start < 0) {
    start += state.historyLength;
  }
  for (let i = 0; i < count; i++) {
    out[i] = buffer[(start + i) % state.historyLength];
  }
  return out;
}

function resetParams() {
  Object.entries(defaults).forEach(([key, value]) => {
    if (ui[key].type === "checkbox") {
      ui[key].checked = value;
    } else {
      ui[key].value = value;
    }
  });
  updateValueLabels();
  setStatus("Parameters reset to baseline-friendly defaults.");
}

async function refreshDevices() {
  if (!navigator.mediaDevices?.enumerateDevices) {
    return;
  }

  const devices = await navigator.mediaDevices.enumerateDevices();
  const inputs = devices.filter((d) => d.kind === "audioinput");
  const outputs = devices.filter((d) => d.kind === "audiooutput");

  ui.inputDevice.innerHTML = "";
  ui.outputDevice.innerHTML = "";

  [["default", "System default"]].forEach(([value, label]) => {
    const inOpt = document.createElement("option");
    inOpt.value = value;
    inOpt.textContent = label;
    ui.inputDevice.appendChild(inOpt);

    const outOpt = document.createElement("option");
    outOpt.value = value;
    outOpt.textContent = label;
    ui.outputDevice.appendChild(outOpt);
  });

  inputs.forEach((device, i) => {
    const opt = document.createElement("option");
    opt.value = device.deviceId;
    opt.textContent = device.label || `Input ${i + 1}`;
    ui.inputDevice.appendChild(opt);
  });

  outputs.forEach((device, i) => {
    const opt = document.createElement("option");
    opt.value = device.deviceId;
    opt.textContent = device.label || `Output ${i + 1}`;
    ui.outputDevice.appendChild(opt);
  });
}

function readParams(sampleRate, blockSize) {
  return {
    monitorEnabled: ui.monitorEnabled.checked,
    agcEnabled: ui.agcEnabled.checked,
    compressorEnabled: ui.compressorEnabled.checked,
    targetRms: percentToUnit(parseFloat(ui.targetRms.value)),
    maxGain: dbToLinear(parseFloat(ui.maxGainDb.value)),
    gateThreshold: percentToUnit(parseFloat(ui.gateThreshold.value)),
    gateHoldSamples: Math.round((parseFloat(ui.gateHoldMs.value) / 1000) * sampleRate),
    attackAlpha: msToAlpha(parseFloat(ui.attackMs.value), sampleRate, blockSize),
    releaseAlpha: msToAlpha(parseFloat(ui.releaseMs.value), sampleRate, blockSize),
    envelopeAlpha: msToAlpha(parseFloat(ui.envelopeWindowMs.value), sampleRate, 1),
    rmsAlpha: msToAlpha(parseFloat(ui.rmsWindowMs.value), sampleRate, blockSize),
    peakProtectorRatio: percentToUnit(parseFloat(ui.peakProtectorRatio.value)),
    limiterThreshold: percentToUnit(parseFloat(ui.limiterThreshold.value)),
    compressorThreshold: dbToLinear(parseFloat(ui.compressorThresholdDb.value)),
    compressorRatio: parseFloat(ui.compressorRatio.value)
  };
}

function processBlock(inputBlock, params) {
  const output = new Float32Array(inputBlock.length);
  let inputPeak = 0;
  let outputPeak = 0;
  let inputEnergy = 0;
  let outputEnergy = 0;

  for (let i = 0; i < inputBlock.length; i++) {
    const x = inputBlock[i];
    const absX = Math.abs(x);
    inputPeak = Math.max(inputPeak, absX);
    inputEnergy += x * x;
    state.envelope += (absX - state.envelope) * params.envelopeAlpha;
  }

  const frameRms = Math.sqrt(inputEnergy / inputBlock.length);
  state.smoothedRms += (frameRms - state.smoothedRms) * params.rmsAlpha;

  if (!state.gateOpen && state.envelope >= params.gateThreshold) {
    state.gateOpen = true;
    state.gateHoldSamples = params.gateHoldSamples;
  } else if (state.envelope >= params.gateThreshold * 0.7) {
    state.gateOpen = true;
    state.gateHoldSamples = params.gateHoldSamples;
  } else if (state.gateHoldSamples > 0) {
    state.gateHoldSamples -= inputBlock.length;
    state.gateOpen = true;
  } else {
    state.gateOpen = false;
    state.gateHoldSamples = 0;
  }

  let desiredGain = 1;
  if (params.agcEnabled && state.gateOpen && state.smoothedRms > 0.00001) {
    desiredGain = clamp(params.targetRms / state.smoothedRms, 0, params.maxGain);
  }

  state.latestDesiredGain = desiredGain;
  if (params.agcEnabled) {
    const alpha = desiredGain < state.appliedGain ? params.attackAlpha : params.releaseAlpha;
    state.appliedGain += (desiredGain - state.appliedGain) * alpha;
    state.appliedGain = clamp(state.appliedGain, 0, params.maxGain);
  } else {
    state.appliedGain = 1;
  }

  for (let i = 0; i < inputBlock.length; i++) {
    let y = inputBlock[i] * state.appliedGain;
    if (params.compressorEnabled) {
      const sign = y < 0 ? -1 : 1;
      const absY = Math.abs(y);
      if (absY > params.compressorThreshold) {
        const over = absY - params.compressorThreshold;
        y = sign * (params.compressorThreshold + over / params.compressorRatio);
      }
    }
    output[i] = y;
  }

  let blockPeak = 0;
  for (let i = 0; i < output.length; i++) {
    blockPeak = Math.max(blockPeak, Math.abs(output[i]));
  }

  state.latestPeakProtected = false;
  state.latestLimiter = false;

  const safePeak = params.limiterThreshold * params.peakProtectorRatio;
  if (blockPeak > safePeak && blockPeak > 0) {
    const scale = safePeak / blockPeak;
    state.latestPeakProtected = true;
    for (let i = 0; i < output.length; i++) {
      output[i] *= scale;
    }
  }

  for (let i = 0; i < output.length; i++) {
    if (output[i] > params.limiterThreshold) {
      output[i] = params.limiterThreshold;
      state.latestLimiter = true;
    } else if (output[i] < -params.limiterThreshold) {
      output[i] = -params.limiterThreshold;
      state.latestLimiter = true;
    }
    outputPeak = Math.max(outputPeak, Math.abs(output[i]));
    outputEnergy += output[i] * output[i];
  }

  state.latestInputPeak = inputPeak;
  state.latestInputRms = Math.sqrt(inputEnergy / inputBlock.length);
  state.latestOutputPeak = outputPeak;
  state.latestOutputRms = Math.sqrt(outputEnergy / output.length);
  state.latestInputBlock = inputBlock.slice(0);
  state.latestOutputBlock = output.slice(0);
  pushHistory(state.latestInputBlock, state.latestOutputBlock);
  if (state.recording) {
    state.recordInputChunks.push(state.latestInputBlock.slice(0));
    state.recordOutputChunks.push(state.latestOutputBlock.slice(0));
    state.recordInputSamples += state.latestInputBlock.length;
    state.recordOutputSamples += state.latestOutputBlock.length;
  }

  ui.inputState.textContent = state.gateOpen ? "active" : "gated";
  if (params.agcEnabled && params.compressorEnabled) {
    ui.outputState.textContent = "agc+comp";
  } else if (params.agcEnabled) {
    ui.outputState.textContent = "agc";
  } else if (params.compressorEnabled) {
    ui.outputState.textContent = "bypass+comp";
  } else {
    ui.outputState.textContent = "bypass";
  }

  return output;
}

function drawWave(canvas, data, color) {
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  const height = canvas.height;
  ctx.clearRect(0, 0, width, height);
  ctx.fillStyle = "#0b1220";
  ctx.fillRect(0, 0, width, height);
  ctx.strokeStyle = "rgba(255,255,255,0.08)";
  ctx.beginPath();
  ctx.moveTo(0, height / 2);
  ctx.lineTo(width, height / 2);
  ctx.stroke();
  ctx.strokeStyle = color;
  ctx.lineWidth = 2;
  ctx.beginPath();
  for (let i = 0; i < data.length; i++) {
    const x = (i / Math.max(data.length - 1, 1)) * width;
    const y = height * 0.5 - data[i] * height * 0.42;
    if (i === 0) {
      ctx.moveTo(x, y);
    } else {
      ctx.lineTo(x, y);
    }
  }
  ctx.stroke();
}

function updateMeters() {
  [["inputPeakBar", "inputPeakText", state.latestInputPeak],
   ["inputRmsBar", "inputRmsText", state.latestInputRms],
   ["outputPeakBar", "outputPeakText", state.latestOutputPeak],
   ["outputRmsBar", "outputRmsText", state.latestOutputRms]].forEach(([barId, textId, value]) => {
    ui[barId].style.width = `${clamp(value, 0, 1) * 100}%`;
    ui[textId].textContent = value.toFixed(3);
  });

  ui.flowEnvelope.textContent = state.envelope.toFixed(3);
  ui.flowRms.textContent = state.smoothedRms.toFixed(3);
  ui.flowDesiredGain.textContent = `${state.latestDesiredGain.toFixed(2)}x`;
  ui.flowAppliedGain.textContent = `${state.appliedGain.toFixed(2)}x`;
  ui.flowAgcMode.textContent = ui.agcEnabled.checked ? "enabled" : "bypass";
  ui.flowPeakProtector.textContent = state.latestPeakProtected ? "active" : "off";
  ui.flowLimiter.textContent = state.latestLimiter ? "active" : "off";

  if (state.inputHistory && state.outputHistory && state.historySampleRate > 0) {
    const windowMs = parseInt(ui.scopeWindowSelect.value, 10);
    const sampleCount = Math.max(1, Math.round((state.historySampleRate * windowMs) / 1000));
    drawWave(ui.inputScope, getRecentHistory(state.inputHistory, sampleCount), "#66a9ff");
    drawWave(ui.outputScope, getRecentHistory(state.outputHistory, sampleCount), "#5de0a7");
  } else {
    drawWave(ui.inputScope, state.latestInputBlock, "#66a9ff");
    drawWave(ui.outputScope, state.latestOutputBlock, "#5de0a7");
  }
}

function animate() {
  state.animationCounter++;
  if (state.animationCounter >= state.animationDecimation) {
    state.animationCounter = 0;
    updateMeters();
  }
  requestAnimationFrame(animate);
}

async function startAudio() {
  if (state.running) {
    return;
  }

  try {
    const AudioContextCtor = window.AudioContext || window.webkitAudioContext;
    state.audioContext = new AudioContextCtor();
    ensureHistory(state.audioContext.sampleRate);
    if (ui.outputDevice.value !== "default") {
      if (typeof state.audioContext.setSinkId === "function") {
        try {
          await state.audioContext.setSinkId(ui.outputDevice.value);
        } catch (sinkError) {
          setStatus(`Output device switch failed, using default output. ${sinkError.message}`);
        }
      } else {
        setStatus("This browser does not support explicit output device routing. Using default output.");
      }
    }
    const constraints = {
      audio: {
        channelCount: 1,
        echoCancellation: false,
        noiseSuppression: false,
        autoGainControl: false,
        deviceId: ui.inputDevice.value && ui.inputDevice.value !== "default" ? { exact: ui.inputDevice.value } : undefined
      }
    };
    state.mediaStream = await navigator.mediaDevices.getUserMedia(constraints);
    state.mediaSource = state.audioContext.createMediaStreamSource(state.mediaStream);
    state.processor = state.audioContext.createScriptProcessor(1024, 1, 1);
    state.outputGain = state.audioContext.createGain();

    state.processor.onaudioprocess = (event) => {
      const input = event.inputBuffer.getChannelData(0);
      const output = event.outputBuffer.getChannelData(0);
      const params = readParams(state.audioContext.sampleRate, input.length);
      const processed = processBlock(input, params);
      for (let i = 0; i < processed.length; i++) {
        output[i] = params.monitorEnabled ? processed[i] : 0;
      }
    };

    state.mediaSource.connect(state.processor);
    state.processor.connect(state.outputGain);
    state.outputGain.connect(state.audioContext.destination);

    state.running = true;
    ui.startButton.disabled = true;
    ui.stopButton.disabled = false;
    ui.recordButton.disabled = false;
    setStatus("Live AGC started. Audio buffer was increased for stability, so latency is higher but crackle risk should be lower.");
  } catch (error) {
    setStatus(`Failed to start audio: ${error.message}`);
    await stopAudio();
  }
}

async function stopAudio() {
  if (state.recording) {
    stopRecordingAndDownload();
  }
  if (state.processor) {
    state.processor.disconnect();
    state.processor.onaudioprocess = null;
    state.processor = null;
  }
  if (state.mediaSource) {
    state.mediaSource.disconnect();
    state.mediaSource = null;
  }
  if (state.outputGain) {
    state.outputGain.disconnect();
    state.outputGain = null;
  }
  if (state.mediaStream) {
    state.mediaStream.getTracks().forEach((track) => track.stop());
    state.mediaStream = null;
  }
  if (state.audioContext) {
    await state.audioContext.close();
    state.audioContext = null;
  }
  state.running = false;
  state.envelope = 0;
  state.smoothedRms = 0;
  state.appliedGain = 1;
  state.gateOpen = false;
  state.gateHoldSamples = 0;
  resetRecordingBuffers();
  state.historyIndex = 0;
  if (state.inputHistory) {
    state.inputHistory.fill(0);
  }
  if (state.outputHistory) {
    state.outputHistory.fill(0);
  }
  ui.startButton.disabled = false;
  ui.stopButton.disabled = true;
  ui.recordButton.disabled = true;
  ui.recordButton.textContent = "Record";
  ui.recordButton.classList.remove("recording");
  ui.inputState.textContent = "idle";
  ui.outputState.textContent = "idle";
  setStatus("Audio stopped.");
}

function attachEvents() {
  [
    ui.targetRms, ui.maxGainDb, ui.gateThreshold, ui.gateHoldMs, ui.attackMs,
    ui.releaseMs, ui.envelopeWindowMs, ui.rmsWindowMs, ui.peakProtectorRatio,
    ui.limiterThreshold, ui.compressorThresholdDb, ui.compressorRatio,
    ui.scopeWidth, ui.scopeHeight, ui.scopeWindowSelect,
    ui.agcEnabled, ui.compressorEnabled, ui.monitorEnabled
  ].forEach((el) => {
    el.addEventListener("input", updateValueLabels);
    el.addEventListener("change", updateValueLabels);
  });
  ui.startButton.addEventListener("click", startAudio);
  ui.stopButton.addEventListener("click", stopAudio);
  ui.recordButton.addEventListener("click", toggleRecording);
  ui.resetButton.addEventListener("click", resetParams);
}

async function init() {
  bindUi();
  attachEvents();
  resetParams();
  await refreshDevices();
  animate();
  setStatus("Ready. Choose devices and start the live AGC monitor.");
}

init();

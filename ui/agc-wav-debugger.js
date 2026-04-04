const ui = {};
const state = {
  debug: null,
  selectedFrame: 0,
  signalWindowMs: 100,
  metricKey: "smoothed_rms",
  currentFileName: "",
};

const metricDescriptions = {
  smoothed_rms: "Smoothed RMS: AGC'nin hedef seviye kararinda kullandigi yavas enerji olcumu.",
  envelope: "Envelope: mutlak genlikten turetilen daha hizli seviye gostergesi; gate davranisini etkiler.",
  desired_gain: "Desired Gain: bu frame icin hesaplanan hedef kazanc.",
  applied_gain: "Applied Gain: attack/release smoothing sonrasi uygulanan gercek kazanc.",
  crest_factor_db: "Crest Factor: peak / RMS orani. Yuksekse sinyal peak-dominant kabul edilir.",
  smoothed_crest_factor_db: "Smoothed Crest Factor: CFAGC kararinda kullanilan yumusatilmis crest factor.",
  cf_blend_weight: "CF Blend Weight: RMS ve peak-aware gain planning arasindaki karisim agirligi.",
  stage_output_peak: "Output Peak: frame sonundaki cikis peak seviyesi.",
  stage_output_rms: "Output RMS: frame sonundaki cikis RMS seviyesi.",
};

function bindUi() {
  [
    "wavFile", "analyzeButton", "downloadOutputButton", "statusBox", "timeWindowMs", "metricSelect",
    "frameRange", "prevFrame", "nextFrame", "prevLimiterFrame", "nextLimiterFrame", "exportFrameButton",
    "prevMetricPeak", "nextMetricPeak",
    "filterGate", "filterHeadroom", "filterLimiter", "issueOnlyMode",
    "modeSelect", "targetRmsPercent", "targetRmsPercentValue",
    "maxGainDb", "maxGainDbValue", "gateThresholdPercent", "gateThresholdPercentValue",
    "summaryMode", "summaryRate", "summaryTarget", "summaryPeak", "summaryLimiter", "summaryFrames", "metricLabel",
    "signalReadout", "frameReadout", "limiterFrameReadout", "metricPeakReadout", "frameTimeReadout", "metricGrid",
    "decisionSummary", "decisionList", "stageGrid", "metricExplain", "desiredGainFormula", "appliedGainFormula",
    "currentCrestReadout", "smoothedCrestReadout",
    "inputScope", "outputScope", "metricScope", "frameInputScope", "frameOutputScope", "frameOutputLegend",
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

function dbToLinear(db) {
  return Math.pow(10, db / 20);
}

function linearToDb(value) {
  if (value <= 1.0e-9) {
    return -180.0;
  }
  return 20.0 * Math.log10(value);
}

function targetPeakFs(targetLevel) {
  return targetLevel + (1.0 - targetLevel) * 0.4;
}

function limiterThresholdFs(targetLevel) {
  return targetLevel + (1.0 - targetLevel) * 0.9;
}

function timeConstantToAlpha(timeMs, frameMs) {
  const dt = frameMs * 0.001;
  const tau = timeMs * 0.001;
  if (tau <= 1.0e-6) {
    return 1.0;
  }
  return 1.0 - Math.exp(-dt / tau);
}

function smoothstep(edge0, edge1, x) {
  if (edge1 <= edge0) {
    return x >= edge1 ? 1.0 : 0.0;
  }
  const t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
}

function peakBiasWeight(blendWeight) {
  if (blendWeight <= 0.0) {
    return 0.0;
  }
  if (blendWeight >= 1.0) {
    return 1.0;
  }
  const keepRms = 1.0 - blendWeight;
  return 1.0 - keepRms * keepRms * keepRms * keepRms;
}

function sampleStats(samples) {
  if (!samples || samples.length === 0) {
    return { peak: 0.0, rms: 0.0 };
  }
  let peak = 0.0;
  let energy = 0.0;
  for (let i = 0; i < samples.length; i++) {
    const value = samples[i];
    const absValue = Math.abs(value);
    if (absValue > peak) {
      peak = absValue;
    }
    energy += value * value;
  }
  return { peak, rms: Math.sqrt(energy / samples.length) };
}

function summarizeSamples(samples) {
  const out = new Array(samples.length);
  for (let i = 0; i < samples.length; i++) {
    out[i] = Number(samples[i].toFixed(6));
  }
  return out;
}

function agcConfigPreset(sampleRate, mode) {
  const frameMs = 4;
  const targetRmsFs = parseFloat(ui.targetRmsPercent.value) / 100.0;
  const config = {
    mode: mode.toUpperCase(),
    sampleRateHz: sampleRate,
    frameMs,
    frameSamples: Math.max(1, Math.round(sampleRate * frameMs / 1000)),
    targetRmsFs,
    attackMs: 8.0,
    releaseMs: 300.0,
    envelopeWindowMs: 4.0,
    rmsWindowMs: 8.0,
    gateHoldMs: 80.0,
    gateThreshold: parseFloat(ui.gateThresholdPercent.value) / 100.0,
    maxGain: dbToLinear(parseFloat(ui.maxGainDb.value)),
    cfagcEnabled: true,
    cfLowDb: 6.0,
    cfHighDb: 10.5,
    cfRiseMs: 8.0,
    cfFallMs: 40.0,
    rmsActivityFloor: 0.05,
    compressorEnabled: false,
    compressorThresholdDbfs: -2.0,
    compressorRatio: 6.0,
    compressorKneeDb: 8.0,
    limiterThreshold: limiterThresholdFs(targetRmsFs),
  };
  if (config.mode === "DIGITAL") {
    config.attackMs = 10.0;
    config.releaseMs = 350.0;
    config.compressorEnabled = true;
    config.compressorThresholdDbfs = -4.0;
    config.compressorRatio = 3.0;
    config.compressorKneeDb = 6.0;
  }
  return config;
}

function measureLevel(frame, detectorState, config) {
  const frameSamples = frame.length;
  const envelopeAlpha = clamp(frameSamples / ((config.envelopeWindowMs * 0.001 * config.sampleRateHz) + frameSamples), 0.001, 1.0);
  const rmsAlpha = clamp(frameSamples / ((config.rmsWindowMs * 0.001 * config.sampleRateHz) + frameSamples), 0.001, 1.0);
  let envelope = detectorState.envelope;
  let inputPeak = 0.0;
  let energy = 0.0;

  for (let i = 0; i < frame.length; i++) {
    const value = frame[i];
    const absValue = Math.abs(value);
    if (absValue > inputPeak) {
      inputPeak = absValue;
    }
    energy += value * value;
    envelope += envelopeAlpha * (absValue - envelope);
  }

  const frameRms = Math.sqrt(energy / Math.max(frameSamples, 1));
  detectorState.envelope = envelope;
  detectorState.smoothedRms += rmsAlpha * (frameRms - detectorState.smoothedRms);

  return {
    inputPeak,
    filteredEnvelope: envelope,
    filteredRms: detectorState.smoothedRms,
  };
}

function updateGate(levelInfo, gateState, config) {
  const openThreshold = config.gateThreshold;
  const closeThreshold = config.gateThreshold * 0.7;
  const holdFrames = Math.max(Math.round(config.gateHoldMs / config.frameMs), 0);
  const activity = levelInfo.filteredEnvelope;

  if (!gateState.isOpen && activity >= openThreshold) {
    gateState.isOpen = true;
    gateState.holdFramesRemaining = holdFrames;
  } else if (activity >= closeThreshold) {
    gateState.isOpen = true;
    gateState.holdFramesRemaining = holdFrames;
  } else if (gateState.holdFramesRemaining > 0) {
    gateState.holdFramesRemaining -= 1;
    gateState.isOpen = true;
  } else {
    gateState.isOpen = false;
    gateState.holdFramesRemaining = 0;
  }
  return gateState.isOpen;
}

function computeDesiredGain(levelInfo, gateOpen, gainState, config) {
  gainState.headroomLimited = false;
  gainState.crestSmoothingActive = false;
  if (!gateOpen || levelInfo.filteredRms <= 1.0e-9) {
    gainState.cfBlendWeight = 0.0;
    return 1.0;
  }

  const gainRms = config.targetRmsFs / levelInfo.filteredRms;
  const gainPeak = levelInfo.inputPeak > 1.0e-6 ? targetPeakFs(config.targetRmsFs) / levelInfo.inputPeak : config.maxGain;
  const crestFactorDb = levelInfo.inputPeak > 1.0e-9 ? linearToDb(levelInfo.inputPeak / Math.max(levelInfo.filteredRms, 1.0e-9)) : 0.0;
  gainState.currentCrestFactorDb = crestFactorDb;
  const crestUpdateAllowed = gateOpen && levelInfo.filteredRms > config.rmsActivityFloor;
  gainState.crestSmoothingActive = crestUpdateAllowed;
  if (crestUpdateAllowed) {
    const cfAlpha = timeConstantToAlpha(
      crestFactorDb > gainState.smoothedCrestFactorDb ? config.cfRiseMs : config.cfFallMs,
      config.frameMs
    );
    gainState.smoothedCrestFactorDb += cfAlpha * (crestFactorDb - gainState.smoothedCrestFactorDb);
  }
  const blendWeight = config.cfagcEnabled ? smoothstep(config.cfLowDb, config.cfHighDb, gainState.smoothedCrestFactorDb) : 0.0;
  let effectiveBlendWeight = blendWeight * blendWeight;
  if (gainPeak < gainRms && blendWeight > 0.0) {
    effectiveBlendWeight = peakBiasWeight(blendWeight);
  }
  let gain;
  if (gainRms > 1.0e-9 && gainPeak > 1.0e-9) {
    gain = Math.exp((1.0 - effectiveBlendWeight) * Math.log(gainRms) + effectiveBlendWeight * Math.log(gainPeak));
  } else {
    gain = (1.0 - effectiveBlendWeight) * gainRms + effectiveBlendWeight * gainPeak;
  }
  gainState.cfBlendWeight = effectiveBlendWeight;
  if (gainPeak < gainRms && effectiveBlendWeight > 0.0) {
    gainState.headroomLimited = true;
  }
  return clamp(gain, 0.0, config.maxGain);
}

function smoothGain(desiredGain, fastRise, gainState, config) {
  const alpha = (desiredGain < gainState.appliedGain || (fastRise && desiredGain > gainState.appliedGain))
    ? timeConstantToAlpha(config.attackMs, config.frameMs)
    : timeConstantToAlpha(config.releaseMs, config.frameMs);
  gainState.desiredGain = desiredGain;
  gainState.appliedGain += alpha * (desiredGain - gainState.appliedGain);
  return gainState.appliedGain;
}

function applyGain(frame, gain, gainState) {
  const out = new Float32Array(frame.length);
  gainState.overflowDetected = false;
  for (let i = 0; i < frame.length; i++) {
    const value = frame[i] * gain;
    if (Math.abs(value) > 1.0) {
      gainState.overflowDetected = true;
    }
    out[i] = value;
  }
  return out;
}

function softKneeOutputDb(inputDb, thresholdDb, ratio, kneeDb) {
  const halfKnee = 0.5 * kneeDb;
  const lower = thresholdDb - halfKnee;
  const upper = thresholdDb + halfKnee;
  if (ratio <= 1.0) {
    return inputDb;
  }
  if (kneeDb <= 0.0) {
    return inputDb <= thresholdDb ? inputDb : thresholdDb + (inputDb - thresholdDb) / ratio;
  }
  if (inputDb <= lower) {
    return inputDb;
  }
  if (inputDb >= upper) {
    return thresholdDb + (inputDb - thresholdDb) / ratio;
  }
  const compressedDb = thresholdDb + (inputDb - thresholdDb) / ratio;
  const t = (inputDb - lower) / kneeDb;
  return inputDb + t * t * (compressedDb - inputDb);
}

function applyCompressor(frame, compressorState, config) {
  compressorState.isActive = false;
  compressorState.maxGainReductionDb = 0.0;
  if (!config.compressorEnabled || config.compressorRatio <= 1.0) {
    return frame.slice(0);
  }
  const out = new Float32Array(frame.length);
  for (let i = 0; i < frame.length; i++) {
    let value = frame[i];
    const absValue = Math.abs(value);
    if (absValue > 1.0e-9) {
      const inputDb = linearToDb(absValue);
      const outputDb = softKneeOutputDb(inputDb, config.compressorThresholdDbfs, config.compressorRatio, config.compressorKneeDb);
      const gainReductionDb = inputDb - outputDb;
      if (gainReductionDb > 0.0) {
        value *= dbToLinear(-gainReductionDb);
        compressorState.isActive = true;
        if (gainReductionDb > compressorState.maxGainReductionDb) {
          compressorState.maxGainReductionDb = gainReductionDb;
        }
      }
    }
    out[i] = value;
  }
  return out;
}

function softLimitSample(value, threshold) {
  const absValue = Math.abs(value);
  const sign = value >= 0.0 ? 1.0 : -1.0;
  if (absValue <= threshold) {
    return value;
  }
  const headroom = 1.0 - threshold;
  if (headroom <= 1.0e-6) {
    return sign * threshold;
  }
  const overshoot = (absValue - threshold) / headroom;
  const compressed = threshold + headroom * Math.tanh(overshoot);
  return sign * Math.min(compressed, 1.0);
}

function applyLimiter(frame, limiterState, config) {
  limiterState.isActive = false;
  limiterState.lastGainReduction = 0.0;
  const out = new Float32Array(frame.length);
  for (let i = 0; i < frame.length; i++) {
    let value = frame[i];
    const absValue = Math.abs(value);
    if (absValue > config.limiterThreshold) {
      const limited = softLimitSample(value, config.limiterThreshold);
      limiterState.isActive = true;
      limiterState.lastGainReduction = Math.max(limiterState.lastGainReduction, absValue - Math.abs(limited));
      value = limited;
    }
    out[i] = value;
  }
  return out;
}

function floatToInt16(sample) {
  const clamped = clamp(sample, -1, 1);
  return clamped >= 0 ? Math.round(clamped * 32767) : Math.round(clamped * 32768);
}

function createWavBlobFromFloat32(samples, sampleRate) {
  const dataBytes = samples.length * 2;
  const buffer = new ArrayBuffer(44 + dataBytes);
  const view = new DataView(buffer);
  let offset = 0;
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
  let writePos = 44;
  for (let i = 0; i < samples.length; i++) {
    view.setInt16(writePos, floatToInt16(samples[i]), true);
    writePos += 2;
  }
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

async function decodeWavFile(file) {
  const arrayBuffer = await file.arrayBuffer();
  const audioContext = new AudioContext();
  try {
    const audioBuffer = await audioContext.decodeAudioData(arrayBuffer.slice(0));
    return {
      sampleRate: audioBuffer.sampleRate,
      samples: Array.from(audioBuffer.getChannelData(0)),
    };
  } finally {
    await audioContext.close();
  }
}

function processSamples(samples, sampleRate, fileName) {
  const config = agcConfigPreset(sampleRate, ui.modeSelect.value);
  const detectorState = { envelope: 0.0, smoothedRms: 0.0 };
  const gateState = { isOpen: false, holdFramesRemaining: 0 };
  const gainState = {
    currentCrestFactorDb: 0.0,
    desiredGain: 1.0,
    appliedGain: 1.0,
    smoothedCrestFactorDb: 0.0,
    cfBlendWeight: 0.0,
    crestSmoothingActive: false,
    headroomLimited: false,
    overflowDetected: false,
  };
  const compressorState = { isActive: false, maxGainReductionDb: 0.0 };
  const limiterState = { isActive: false, lastGainReduction: 0.0 };
  const outputSamples = new Array(samples.length).fill(0.0);
  const frames = [];

  for (let frameIndex = 0, frameStart = 0; frameStart < samples.length; frameIndex += 1, frameStart += config.frameSamples) {
    const frameInput = samples.slice(frameStart, frameStart + config.frameSamples);
    const paddedInput = frameInput.slice(0);
    while (paddedInput.length < config.frameSamples) {
      paddedInput.push(0.0);
    }
    const previousGateOpen = gateState.isOpen;
    const levelInfo = measureLevel(paddedInput, detectorState, config);
    const gateOpen = updateGate(levelInfo, gateState, config);
    const desiredGain = computeDesiredGain(levelInfo, gateOpen, gainState, config);
    const fastRise = (!previousGateOpen) && gateOpen && desiredGain > gainState.appliedGain;
    const appliedGain = smoothGain(desiredGain, fastRise, gainState, config);
    const afterGain = applyGain(paddedInput, appliedGain, gainState);
    const afterCompressor = applyCompressor(afterGain, compressorState, config);
    const afterLimiter = applyLimiter(afterCompressor, limiterState, config);

    for (let i = 0; i < frameInput.length; i++) {
      outputSamples[frameStart + i] = afterLimiter[i];
    }

    const inputStats = sampleStats(frameInput);
    const gainStats = sampleStats(Array.from(afterGain.slice(0, frameInput.length)));
    const compressorStats = sampleStats(Array.from(afterCompressor.slice(0, frameInput.length)));
    const outputStats = sampleStats(Array.from(afterLimiter.slice(0, frameInput.length)));
    const crestFactorDb = levelInfo.inputPeak > 1.0e-9 && levelInfo.filteredRms > 1.0e-9 ? linearToDb(levelInfo.inputPeak / levelInfo.filteredRms) : 0.0;

    frames.push({
      index: frameIndex,
      start_sample: frameStart,
      end_sample: frameStart + Math.max(frameInput.length - 1, 0),
      input_peak: Number(levelInfo.inputPeak.toFixed(6)),
      input_rms: Number(inputStats.rms.toFixed(6)),
      envelope: Number(levelInfo.filteredEnvelope.toFixed(6)),
      smoothed_rms: Number(levelInfo.filteredRms.toFixed(6)),
      crest_factor_db: Number(crestFactorDb.toFixed(6)),
      smoothed_crest_factor_db: Number(gainState.smoothedCrestFactorDb.toFixed(6)),
      crest_smoothing_active: gainState.crestSmoothingActive,
      gate_open: gateOpen,
      gate_hold_frames_remaining: gateState.holdFramesRemaining,
      desired_gain: Number(desiredGain.toFixed(6)),
      applied_gain: Number(appliedGain.toFixed(6)),
      cf_blend_weight: Number(gainState.cfBlendWeight.toFixed(6)),
      headroom_limited: gainState.headroomLimited,
      overflow_detected: gainState.overflowDetected,
      compressor_active: compressorState.isActive,
      compressor_gain_reduction_db: Number(compressorState.maxGainReductionDb.toFixed(6)),
      peak_protector_active: false,
      peak_protector_scale: 1.0,
      limiter_active: limiterState.isActive,
      limiter_reduction: Number(limiterState.lastGainReduction.toFixed(6)),
      stage_input_peak: Number(inputStats.peak.toFixed(6)),
      stage_gain_peak: Number(gainStats.peak.toFixed(6)),
      stage_compressor_peak: Number(compressorStats.peak.toFixed(6)),
      stage_peak_protector_peak: Number(compressorStats.peak.toFixed(6)),
      stage_output_peak: Number(outputStats.peak.toFixed(6)),
      stage_output_rms: Number(outputStats.rms.toFixed(6)),
      frame_input: summarizeSamples(frameInput),
      frame_output: summarizeSamples(Array.from(afterLimiter.slice(0, frameInput.length))),
    });
  }

  const inputSummary = sampleStats(samples);
  const outputSummary = sampleStats(outputSamples);
  return {
    metadata: {
      input_file: fileName,
      sample_rate: sampleRate,
      frame_samples: config.frameSamples,
      frame_ms: config.frameMs,
      mode: config.mode,
      target_rms_percent: parseInt(ui.targetRmsPercent.value, 10),
      max_gain_db: parseInt(ui.maxGainDb.value, 10),
      gate_threshold_percent: parseInt(ui.gateThresholdPercent.value, 10),
      target_peak: Number(targetPeakFs(config.targetRmsFs).toFixed(6)),
      limiter_threshold: Number(config.limiterThreshold.toFixed(6)),
    },
    summary: {
      input_peak: Number(inputSummary.peak.toFixed(6)),
      input_rms: Number(inputSummary.rms.toFixed(6)),
      output_peak: Number(outputSummary.peak.toFixed(6)),
      output_rms: Number(outputSummary.rms.toFixed(6)),
      frames: frames.length,
      gate_frames: frames.filter((frame) => frame.gate_open).length,
      headroom_limited_frames: frames.filter((frame) => frame.headroom_limited).length,
      compressor_frames: frames.filter((frame) => frame.compressor_active).length,
      limiter_frames: frames.filter((frame) => frame.limiter_active).length,
    },
    config: {
      target_rms_fs: Number(config.targetRmsFs.toFixed(6)),
      max_gain: Number(config.maxGain.toFixed(6)),
      attack_ms: Number(config.attackMs.toFixed(6)),
      release_ms: Number(config.releaseMs.toFixed(6)),
      envelope_window_ms: Number(config.envelopeWindowMs.toFixed(6)),
      rms_window_ms: Number(config.rmsWindowMs.toFixed(6)),
      gate_hold_ms: Number(config.gateHoldMs.toFixed(6)),
      gate_threshold: Number(config.gateThreshold.toFixed(6)),
      cfagc_enabled: config.cfagcEnabled,
      cf_low_db: Number(config.cfLowDb.toFixed(6)),
      cf_high_db: Number(config.cfHighDb.toFixed(6)),
      cf_rise_ms: Number(config.cfRiseMs.toFixed(6)),
      cf_fall_ms: Number(config.cfFallMs.toFixed(6)),
      rms_activity_floor: Number(config.rmsActivityFloor.toFixed(6)),
      compressor_enabled: config.compressorEnabled,
      compressor_threshold_dbfs: Number(config.compressorThresholdDbfs.toFixed(6)),
      compressor_ratio: Number(config.compressorRatio.toFixed(6)),
      compressor_knee_db: Number(config.compressorKneeDb.toFixed(6)),
    },
    input_samples: summarizeSamples(samples),
    output_samples: summarizeSamples(outputSamples),
    frames,
  };
}

function drawWave(canvas, samples, color, highlightRange = null, guideLines = []) {
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
  guideLines.forEach((line) => {
    const absValue = clamp(Math.abs(line.value), 0, 1);
    const yTop = height * 0.5 - absValue * height * 0.42;
    const yBottom = height * 0.5 + absValue * height * 0.42;
    ctx.strokeStyle = line.color;
    ctx.setLineDash([6, 6]);
    ctx.beginPath();
    ctx.moveTo(0, yTop);
    ctx.lineTo(width, yTop);
    ctx.moveTo(0, yBottom);
    ctx.lineTo(width, yBottom);
    ctx.stroke();
    ctx.setLineDash([]);
  });
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
  const limiterClusters = [];
  const headroomClusters = [];
  const showGate = ui.filterGate.checked;
  const showHeadroom = ui.filterHeadroom.checked;
  const showLimiter = ui.filterLimiter.checked;
  let clusterStart = -1;
  let headroomStart = -1;
  for (let i = 0; i < frames.length; i++) {
    if (showLimiter && frames[i].limiter_active) {
      if (clusterStart < 0) {
        clusterStart = i;
      }
    } else if (clusterStart >= 0) {
      limiterClusters.push([clusterStart, i - 1]);
      clusterStart = -1;
    }

    if (showHeadroom && frames[i].headroom_limited) {
      if (headroomStart < 0) {
        headroomStart = i;
      }
    } else if (headroomStart >= 0) {
      headroomClusters.push([headroomStart, i - 1]);
      headroomStart = -1;
    }
  }
  if (clusterStart >= 0) {
    limiterClusters.push([clusterStart, frames.length - 1]);
  }
  if (headroomStart >= 0) {
    headroomClusters.push([headroomStart, frames.length - 1]);
  }
  headroomClusters.forEach(([start, end]) => {
    const x0 = (start / Math.max(frames.length - 1, 1)) * width;
    const x1 = (end / Math.max(frames.length - 1, 1)) * width;
    ctx.fillStyle = "rgba(255,213,106,0.10)";
    ctx.fillRect(x0, 0, Math.max(4, x1 - x0), height);
  });
  limiterClusters.forEach(([start, end]) => {
    const x0 = (start / Math.max(frames.length - 1, 1)) * width;
    const x1 = (end / Math.max(frames.length - 1, 1)) * width;
    ctx.fillStyle = "rgba(255,125,125,0.12)";
    ctx.fillRect(x0, 0, Math.max(4, x1 - x0), height);
  });
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
    if (showLimiter && frame.limiter_active) {
      ctx.fillStyle = "rgba(255,125,125,0.8)";
      ctx.fillRect(x - 1, height - 18, 2, 18);
    }
    if (showHeadroom && frame.headroom_limited) {
      ctx.fillStyle = "rgba(255,213,106,0.7)";
      ctx.fillRect(x - 1, 0, 2, 14);
    }
    if (showGate && frame.gate_open) {
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

  const selectedValue = values[selectedIndex];
  const selectedY = height - ((selectedValue - min) / span) * (height - 12) - 6;
  const bubbleText = `${selectedValue.toFixed(3)}`;
  ctx.font = "12px Segoe UI";
  const bubbleWidth = ctx.measureText(bubbleText).width + 16;
  const bubbleHeight = 24;
  let bubbleX = selectedX + 8;
  if (bubbleX + bubbleWidth > width - 4) {
    bubbleX = selectedX - bubbleWidth - 8;
  }
  let bubbleY = selectedY - bubbleHeight - 8;
  if (bubbleY < 4) {
    bubbleY = selectedY + 8;
  }
  ctx.fillStyle = "rgba(8,17,29,0.92)";
  ctx.strokeStyle = "rgba(255,255,255,0.18)";
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.roundRect(bubbleX, bubbleY, bubbleWidth, bubbleHeight, 8);
  ctx.fill();
  ctx.stroke();
  ctx.fillStyle = "#edf3ff";
  ctx.fillText(bubbleText, bubbleX + 8, bubbleY + 16);
}

function updateMetricExplanation() {
  ui.metricLabel.textContent = {
    smoothed_rms: "Smoothed RMS",
    envelope: "Envelope",
    desired_gain: "Desired Gain",
    applied_gain: "Applied Gain",
    crest_factor_db: "Crest Factor",
    smoothed_crest_factor_db: "Smoothed Crest Factor",
    cf_blend_weight: "CF Blend Weight",
    stage_output_peak: "Output Peak",
    stage_output_rms: "Output RMS",
  }[state.metricKey] || state.metricKey;
  ui.metricExplain.textContent =
    `${metricDescriptions[state.metricKey] || "Metric aciklamasi yok."} Event marker'lari: yesil = gate open, sari = headroom limited, kirmizi = limiter active. Sari golgeler headroom-limited cluster'lari, kirmizi golgeler limiter cluster bolgelerini gosterir.`;
}

function renderMetricGrid(frame) {
  const items = [
    ["Input Peak", frame.input_peak],
    ["Input RMS", frame.input_rms],
    ["Envelope", frame.envelope],
    ["Smoothed RMS", frame.smoothed_rms],
    ["Crest Factor", `${formatNumber(frame.crest_factor_db, 2)} dB`],
    ["Smoothed Crest", `${formatNumber(frame.smoothed_crest_factor_db, 2)} dB`],
    ["Desired Gain", `${formatNumber(frame.desired_gain, 3)}x`],
    ["Applied Gain", `${formatNumber(frame.applied_gain, 3)}x`],
    ["CF Blend", formatNumber(frame.cf_blend_weight, 3)],
    ["CF Update", frame.crest_smoothing_active ? "active" : "held"],
    ["Gate", frame.gate_open ? "open" : "closed"],
    ["Headroom Limited", frame.headroom_limited ? "yes" : "no"],
    ["Limiter Active", frame.limiter_active ? "yes" : "no"],
  ];
  ui.metricGrid.innerHTML = "";
  items.forEach(([name, rawValue]) => {
    const item = document.createElement("div");
    item.className = "metric-item";
    const label = document.createElement("div");
    label.className = "name";
    label.textContent = name;
    const value = document.createElement("div");
    value.className = "value";
    value.textContent = typeof rawValue === "number" ? formatNumber(rawValue) : rawValue;
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
  stages.forEach(([name, valueText]) => {
    const item = document.createElement("div");
    item.className = "stage-card";
    const label = document.createElement("div");
    label.className = "label";
    label.textContent = name;
    const value = document.createElement("div");
    value.className = "value";
    value.textContent = formatNumber(valueText);
    item.append(label, value);
    ui.stageGrid.appendChild(item);
  });
}

function renderDecision(frame, debug) {
  const bullets = [];
  bullets.push(frame.gate_open
    ? `Gate acik. Envelope ${formatNumber(frame.envelope)} ve smoothed RMS ${formatNumber(frame.smoothed_rms)} ile frame aktif kabul edildi.`
    : "Gate kapali. Bu frame hedef seviyeye cekilmeden geciyor.");
  bullets.push(`Smoothed crest factor ${formatNumber(frame.smoothed_crest_factor_db, 2)} dB. CF blend agirligi ${formatNumber(frame.cf_blend_weight, 3)}.`);
  bullets.push(frame.crest_smoothing_active
    ? `Crest smoothing aktif. Smoothed RMS ${formatNumber(frame.smoothed_rms)} activity floor ${formatNumber(debug.config.rms_activity_floor)} ustunde.`
    : `Crest smoothing tutuldu. Smoothed RMS ${formatNumber(frame.smoothed_rms)} activity floor ${formatNumber(debug.config.rms_activity_floor)} altinda veya gate kapali.`);
  bullets.push(`Desired gain ${formatNumber(frame.desired_gain, 3)}x, applied gain ${formatNumber(frame.applied_gain, 3)}x.`);
  bullets.push(frame.headroom_limited
    ? `Peak-aware yol RMS yolundan daha kisitlayici oldu. Target peak ${formatNumber(debug.metadata.target_peak)} devrede.`
    : "Bu frame'de gain karari daha cok RMS tarafindan surukleniyor.");
  bullets.push(frame.limiter_active
    ? `Final limiter aktif. Reduction ${formatNumber(frame.limiter_reduction, 4)}.`
    : `Final limiter aktif degil. Output peak ${formatNumber(frame.stage_output_peak)} ile esik alti kaldi.`);

  ui.decisionSummary.textContent = `Hedef RMS ${formatNumber(debug.config.target_rms_fs)}, target peak ${formatNumber(debug.metadata.target_peak)}, limiter ${formatNumber(debug.metadata.limiter_threshold)}.`;
  ui.decisionList.innerHTML = "";
  bullets.forEach((text) => {
    const li = document.createElement("li");
    li.textContent = text;
    ui.decisionList.appendChild(li);
  });

  ui.desiredGainFormula.textContent =
    `gain_rms = target_rms / smoothed_rms = ${formatNumber(debug.config.target_rms_fs)} / ${formatNumber(frame.smoothed_rms)} = ${formatNumber(debug.config.target_rms_fs / Math.max(frame.smoothed_rms, 1.0e-9))} | ` +
    `gain_peak = target_peak / input_peak = ${formatNumber(debug.metadata.target_peak)} / ${formatNumber(frame.input_peak)} = ${formatNumber(debug.metadata.target_peak / Math.max(frame.input_peak, 1.0e-9))} | ` +
    `desired_gain = blend_log(gain_rms, gain_peak, cf_blend=${formatNumber(frame.cf_blend_weight)}) = ${formatNumber(frame.desired_gain)}`;

  ui.appliedGainFormula.textContent =
    `applied_gain[n] = applied_gain[n-1] + alpha * (desired_gain - applied_gain[n-1]) | ` +
    `desired = ${formatNumber(frame.desired_gain)} | applied = ${formatNumber(frame.applied_gain)} | ` +
    `attack/release = ${formatNumber(debug.config.attack_ms, 1)} ms / ${formatNumber(debug.config.release_ms, 1)} ms`;
}

function updateSummary(debug) {
  ui.summaryMode.textContent = debug.metadata.mode;
  ui.summaryRate.textContent = `${debug.metadata.sample_rate} Hz`;
  ui.summaryTarget.textContent = `${Math.round(debug.metadata.target_rms_percent)}%`;
  ui.summaryPeak.textContent = formatNumber(debug.metadata.target_peak);
  ui.summaryLimiter.textContent = formatNumber(debug.metadata.limiter_threshold);
  ui.summaryFrames.textContent = String(debug.summary.frames);
}

function getFullSignalSamples(samples) {
  return { start: 0, samples };
}

function updateScopes() {
  if (!state.debug) {
    return;
  }
  const debug = state.debug;
  const frame = debug.frames[state.selectedFrame];
  const sampleRate = debug.metadata.sample_rate;
  const inputWindow = getFullSignalSamples(debug.input_samples);
  const outputWindow = getFullSignalSamples(debug.output_samples);
  const highlightX = ((frame.start_sample - inputWindow.start) / Math.max(inputWindow.samples.length, 1)) * ui.inputScope.width;
  const highlightWidth = Math.max(2, (debug.metadata.frame_samples / Math.max(inputWindow.samples.length, 1)) * ui.inputScope.width);

  drawWave(ui.inputScope, inputWindow.samples, "#69a8ff", { x: highlightX, width: highlightWidth });
  drawWave(ui.outputScope, outputWindow.samples, "#69e3ad", { x: highlightX, width: highlightWidth });
  drawWave(ui.frameInputScope, frame.frame_input, "#69a8ff");
  drawWave(
    ui.frameOutputScope,
    frame.frame_output,
    "#69e3ad",
    null,
    [
      { value: state.debug.metadata.target_peak, color: "rgba(255,213,106,0.9)" },
      { value: state.debug.metadata.limiter_threshold, color: "rgba(255,125,125,0.9)" },
    ]
  );
  drawMetric(ui.metricScope, debug.frames, state.metricKey, state.selectedFrame);

  ui.signalReadout.textContent = `Full signal ${(debug.input_samples.length / sampleRate).toFixed(2)} s, selected frame highlight active`;
  ui.frameReadout.textContent = `Frame ${frame.index + 1} / ${debug.frames.length}`;
  ui.frameTimeReadout.textContent = `${formatNumber(frame.start_sample / sampleRate, 3)} s - ${formatNumber(frame.end_sample / sampleRate, 3)} s`;
  ui.frameOutputLegend.textContent = `Guide lines: target peak ${formatNumber(state.debug.metadata.target_peak)} , limiter ${formatNumber(state.debug.metadata.limiter_threshold)}`;
}

function selectFrame(index, readoutText = "") {
  if (!state.debug) {
    return;
  }
  state.selectedFrame = clamp(index, 0, state.debug.frames.length - 1);
  ui.frameRange.value = String(state.selectedFrame);
  if (readoutText) {
    ui.metricPeakReadout.textContent = readoutText;
  }
  renderSelectedFrame();
}

function jumpLimiterFrame(direction) {
  if (!state.debug) {
    return;
  }
  const frames = state.debug.frames;
  let index = state.selectedFrame;
  while (true) {
    index += direction;
    if (index < 0 || index >= frames.length) {
      ui.limiterFrameReadout.textContent = "No more limiter frames";
      return;
    }
    if (frames[index].limiter_active) {
      ui.limiterFrameReadout.textContent = `Limiter frame ${index + 1}`;
      selectFrame(index);
      return;
    }
  }
}

function metricValues() {
  if (!state.debug) {
    return [];
  }
  return state.debug.frames.map((frame) => Number(frame[state.metricKey] ?? 0));
}

function issueFrameIndices() {
  if (!state.debug) {
    return [];
  }
  const wantGate = ui.filterGate.checked;
  const wantHeadroom = ui.filterHeadroom.checked;
  const wantLimiter = ui.filterLimiter.checked;
  const indices = [];
  state.debug.frames.forEach((frame, index) => {
    const match =
      (wantGate && frame.gate_open) ||
      (wantHeadroom && frame.headroom_limited) ||
      (wantLimiter && frame.limiter_active);
    if (match) {
      indices.push(index);
    }
  });
  return indices;
}

function jumpIssueFrame(direction) {
  if (!state.debug) {
    return false;
  }
  const issues = issueFrameIndices();
  if (issues.length === 0) {
    return false;
  }
  const current = state.selectedFrame;
  let target = -1;
  if (direction > 0) {
    target = issues.find((index) => index > current);
  } else {
    for (let i = issues.length - 1; i >= 0; i--) {
      if (issues[i] < current) {
        target = issues[i];
        break;
      }
    }
  }
  if (target < 0) {
    return false;
  }
  selectFrame(target);
  return true;
}

function findMetricPeaks() {
  const values = metricValues();
  const peaks = [];
  if (values.length === 0) {
    return peaks;
  }
  for (let i = 1; i < values.length - 1; i++) {
    if (values[i] >= values[i - 1] && values[i] > values[i + 1]) {
      peaks.push(i);
    }
  }
  if (values.length === 1) {
    peaks.push(0);
  }
  return peaks;
}

function jumpMetricPeak(direction) {
  if (!state.debug) {
    return;
  }
  const peaks = findMetricPeaks();
  if (peaks.length === 0) {
    ui.metricPeakReadout.textContent = "No metric peaks";
    return;
  }
  const current = state.selectedFrame;
  let target = -1;
  if (direction > 0) {
    target = peaks.find((index) => index > current);
  } else {
    for (let i = peaks.length - 1; i >= 0; i--) {
      if (peaks[i] < current) {
        target = peaks[i];
        break;
      }
    }
  }
  if (target === -1 || target === undefined) {
    ui.metricPeakReadout.textContent = "No more metric peaks";
    return;
  }
  ui.metricPeakReadout.textContent = `${ui.metricLabel.textContent} peak @ frame ${target + 1}`;
  selectFrame(target);
}

function canvasFramePick(event, canvas, useFrameDomain) {
  if (!state.debug) {
    return;
  }
  const rect = canvas.getBoundingClientRect();
  const x = clamp(event.clientX - rect.left, 0, rect.width);
  const ratio = rect.width <= 0 ? 0 : x / rect.width;
  let frameIndex;
  if (useFrameDomain) {
    frameIndex = Math.round(ratio * Math.max(state.debug.frames.length - 1, 0));
  } else {
    const sampleIndex = Math.round(ratio * Math.max(state.debug.input_samples.length - 1, 0));
    frameIndex = Math.floor(sampleIndex / state.debug.metadata.frame_samples);
  }
  selectFrame(frameIndex);
}

function renderSelectedFrame() {
  if (!state.debug) {
    return;
  }
  const frame = state.debug.frames[state.selectedFrame];
  ui.currentCrestReadout.textContent = `${formatNumber(frame.crest_factor_db, 2)} dB`;
  ui.smoothedCrestReadout.textContent = `${formatNumber(frame.smoothed_crest_factor_db, 2)} dB`;
  renderMetricGrid(frame);
  renderStageGrid(frame);
  renderDecision(frame, state.debug);
  updateScopes();
}

function loadDebug(debug) {
  state.debug = debug;
  state.selectedFrame = 0;
  ui.frameRange.min = "0";
  ui.frameRange.max = String(Math.max(debug.frames.length - 1, 0));
  ui.frameRange.value = "0";
  ui.downloadOutputButton.disabled = false;
  ui.exportFrameButton.disabled = false;
  updateSummary(debug);
  updateMetricExplanation();
  renderSelectedFrame();
  setStatus(`Loaded ${debug.metadata.input_file}. ${debug.summary.frames} frame analiz edildi.`);
}

async function analyzeSelectedFile() {
  const [file] = ui.wavFile.files;
  if (!file) {
    setStatus("Once bir WAV dosyasi sec.");
    return;
  }
  ui.analyzeButton.disabled = true;
  setStatus("WAV decode ve frame analiz basladi...");
  try {
    state.currentFileName = file.name;
    const { sampleRate, samples } = await decodeWavFile(file);
    loadDebug(processSamples(samples, sampleRate, file.name));
  } catch (error) {
    setStatus(`Analiz basarisiz: ${error.message}`);
  } finally {
    ui.analyzeButton.disabled = false;
  }
}

function downloadOutputWav() {
  if (!state.debug) {
    return;
  }
  const blob = createWavBlobFromFloat32(state.debug.output_samples, state.debug.metadata.sample_rate);
  const baseName = state.currentFileName ? state.currentFileName.replace(/\.wav$/i, "") : "agc_debug";
  downloadBlob(blob, `${baseName}_debug_output.wav`);
}

function exportSelectedFrameJson() {
  if (!state.debug) {
    return;
  }
  const frame = state.debug.frames[state.selectedFrame];
  const payload = {
    metadata: state.debug.metadata,
    config: state.debug.config,
    summary: state.debug.summary,
    selected_frame: frame,
  };
  const blob = new Blob([JSON.stringify(payload, null, 2)], { type: "application/json" });
  const baseName = state.currentFileName ? state.currentFileName.replace(/\.wav$/i, "") : "agc_debug";
  downloadBlob(blob, `${baseName}_frame_${String(frame.index + 1).padStart(4, "0")}.json`);
}

function updateValueLabels() {
  ui.targetRmsPercentValue.textContent = `${ui.targetRmsPercent.value}%`;
  ui.maxGainDbValue.textContent = `${ui.maxGainDb.value} dB`;
  ui.gateThresholdPercentValue.textContent = `${ui.gateThresholdPercent.value}%`;
}

function attachEvents() {
  ui.analyzeButton.addEventListener("click", analyzeSelectedFile);
  ui.downloadOutputButton.addEventListener("click", downloadOutputWav);
  ui.exportFrameButton.addEventListener("click", exportSelectedFrameJson);
  ui.timeWindowMs.addEventListener("change", () => {
    state.signalWindowMs = parseInt(ui.timeWindowMs.value, 10);
    updateScopes();
  });
  ui.metricSelect.addEventListener("change", () => {
    state.metricKey = ui.metricSelect.value;
    updateMetricExplanation();
    ui.metricPeakReadout.textContent = "Metric peak jump";
    updateScopes();
  });
  ui.frameRange.addEventListener("input", () => {
    selectFrame(parseInt(ui.frameRange.value, 10));
  });
  ui.prevFrame.addEventListener("click", () => {
    if (!state.debug) {
      return;
    }
    if (ui.issueOnlyMode.checked) {
      if (!jumpIssueFrame(-1)) {
        ui.metricPeakReadout.textContent = "No more issue frames";
      }
      return;
    }
    selectFrame(state.selectedFrame - 1);
  });
  ui.nextFrame.addEventListener("click", () => {
    if (!state.debug) {
      return;
    }
    if (ui.issueOnlyMode.checked) {
      if (!jumpIssueFrame(1)) {
        ui.metricPeakReadout.textContent = "No more issue frames";
      }
      return;
    }
    selectFrame(state.selectedFrame + 1);
  });
  ui.prevLimiterFrame.addEventListener("click", () => jumpLimiterFrame(-1));
  ui.nextLimiterFrame.addEventListener("click", () => jumpLimiterFrame(1));
  ui.prevMetricPeak.addEventListener("click", () => jumpMetricPeak(-1));
  ui.nextMetricPeak.addEventListener("click", () => jumpMetricPeak(1));
  ui.inputScope.addEventListener("click", (event) => canvasFramePick(event, ui.inputScope, false));
  ui.outputScope.addEventListener("click", (event) => canvasFramePick(event, ui.outputScope, false));
  ui.metricScope.addEventListener("click", (event) => canvasFramePick(event, ui.metricScope, true));
  [ui.filterGate, ui.filterHeadroom, ui.filterLimiter, ui.issueOnlyMode].forEach((element) => {
    element.addEventListener("change", () => {
      updateMetricExplanation();
      updateScopes();
    });
  });
  [ui.targetRmsPercent, ui.maxGainDb, ui.gateThresholdPercent].forEach((element) => {
    element.addEventListener("input", updateValueLabels);
  });
}

function init() {
  bindUi();
  attachEvents();
  updateValueLabels();
  updateMetricExplanation();
  setStatus("Hazir. WAV sec, parametreleri ayarla ve Analyze ile tarayicida dogrudan debug trace uret.");
}

init();

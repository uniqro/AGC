import math
import struct
import sys
import wave
from dataclasses import dataclass
from pathlib import Path


@dataclass
class AgcConfig:
    mode: str
    sample_rate_hz: int
    frame_ms: int
    frame_samples: int
    target_rms_fs: float
    attack_ms: float
    release_ms: float
    envelope_window_ms: float
    rms_window_ms: float
    gate_hold_ms: float
    gate_threshold: float
    max_gain: float
    gain_headroom_margin: float
    cfagc_enabled: bool
    cf_low_db: float
    cf_high_db: float
    cf_rise_ms: float
    cf_fall_ms: float
    rms_activity_floor: float
    peak_headroom_cap_enabled: bool
    compressor_enabled: bool
    compressor_threshold_dbfs: float
    compressor_ratio: float
    compressor_knee_db: float
    peak_protector_enabled: bool
    peak_protector_ratio: float
    limiter_threshold: float


@dataclass
class DetectorState:
    envelope: float = 0.0
    smoothed_rms: float = 0.0


@dataclass
class GateState:
    is_open: bool = False
    hold_frames_remaining: int = 0


@dataclass
class GainState:
    current_crest_factor_db: float = 0.0
    desired_gain: float = 1.0
    applied_gain: float = 1.0
    smoothed_crest_factor_db: float = 0.0
    cf_blend_weight: float = 0.0
    crest_smoothing_active: bool = False
    headroom_limited: bool = False
    overflow_detected: bool = False


@dataclass
class CompressorState:
    is_active: bool = False
    max_gain_reduction_db: float = 0.0


@dataclass
class PeakProtectorState:
    is_active: bool = False
    applied_scale: float = 1.0


@dataclass
class LimiterState:
    is_active: bool = False
    last_gain_reduction: float = 0.0


@dataclass
class PipelineState:
    detector: DetectorState
    gate: GateState
    gain: GainState
    compressor: CompressorState
    peak_protector: PeakProtectorState
    limiter: LimiterState
    frame_index: int = 0


def db_to_linear(db: float) -> float:
    return 10.0 ** (db / 20.0)


def target_peak_fs(target_level: float) -> float:
    return target_level + (1.0 - target_level) * 0.4


def limiter_threshold_fs(target_level: float) -> float:
    return target_level + (1.0 - target_level) * 0.9


def linear_to_db(value: float) -> float:
    if value <= 1.0e-9:
        return -180.0
    return 20.0 * math.log10(value)


def agc_config_preset(sample_rate_hz: int, mode: str) -> AgcConfig:
    frame_ms = 4
    config = AgcConfig(
        mode=mode.upper(),
        sample_rate_hz=sample_rate_hz,
        frame_ms=frame_ms,
        frame_samples=(sample_rate_hz * frame_ms) // 1000,
        target_rms_fs=0.42,
        attack_ms=8.0,
        release_ms=150.0,
        envelope_window_ms=4.0,
        rms_window_ms=8.0,
        gate_hold_ms=80.0,
        gate_threshold=0.05,
        max_gain=4.0,
        gain_headroom_margin=1.0,
        cfagc_enabled=True,
        cf_low_db=6.0,
        cf_high_db=10.5,
        cf_rise_ms=8.0,
        cf_fall_ms=40.0,
        rms_activity_floor=0.05,
        peak_headroom_cap_enabled=False,
        compressor_enabled=False,
        compressor_threshold_dbfs=-2.0,
        compressor_ratio=6.0,
        compressor_knee_db=8.0,
        peak_protector_enabled=False,
        peak_protector_ratio=1.0,
        limiter_threshold=limiter_threshold_fs(0.42),
    )
    if config.mode == "DIGITAL":
        config.attack_ms = 10.0
        config.release_ms = 350.0
        config.cfagc_enabled = True
        config.cf_low_db = 6.0
        config.cf_high_db = 10.5
        config.cf_rise_ms = 8.0
        config.cf_fall_ms = 40.0
        config.rms_activity_floor = 0.05
        config.compressor_enabled = True
        config.compressor_threshold_dbfs = -4.0
        config.compressor_ratio = 3.0
        config.compressor_knee_db = 6.0
        config.peak_headroom_cap_enabled = True
        config.limiter_threshold = limiter_threshold_fs(config.target_rms_fs)
    return config


def read_wav_mono_s16(path: Path):
    with wave.open(str(path), "rb") as wav:
        if wav.getnchannels() != 1 or wav.getsampwidth() != 2:
            raise ValueError("need mono PCM16 WAV")
        sample_rate = wav.getframerate()
        frames = wav.readframes(wav.getnframes())
    pcm = struct.unpack("<{}h".format(len(frames) // 2), frames)
    floats = []
    for x in pcm:
        floats.append(x / 32767.0 if x >= 0 else x / 32768.0)
    return sample_rate, floats


def write_wav_mono_s16(path: Path, samples, sample_rate: int):
    pcm = bytearray()
    for x in samples:
        if x > 1.0:
            x = 1.0
        elif x < -1.0:
            x = -1.0
        s = int(x * 32767.0 + 0.5) if x >= 0 else int(x * 32768.0 - 0.5)
        pcm.extend(struct.pack("<h", s))
    with wave.open(str(path), "wb") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(pcm)


def measure_level(frame, state: DetectorState, config: AgcConfig):
    frame_samples = len(frame)
    envelope_alpha = frame_samples / ((config.envelope_window_ms * 0.001 * config.sample_rate_hz) + frame_samples)
    rms_alpha = frame_samples / ((config.rms_window_ms * 0.001 * config.sample_rate_hz) + frame_samples)
    envelope_alpha = min(max(envelope_alpha, 0.001), 1.0)
    rms_alpha = min(max(rms_alpha, 0.001), 1.0)

    input_peak = 0.0
    input_energy = 0.0
    filtered_peak = 0.0
    filtered_energy = 0.0
    envelope = state.envelope

    for value in frame:
        abs_value = abs(value)
        if abs_value > input_peak:
            input_peak = abs_value
        if abs_value > filtered_peak:
            filtered_peak = abs_value
        input_energy += value * value
        filtered_energy += value * value
        envelope += envelope_alpha * (abs_value - envelope)

    if frame_samples > 0:
        input_energy /= frame_samples
        filtered_energy /= frame_samples
    frame_rms = math.sqrt(filtered_energy)
    state.envelope = envelope
    state.smoothed_rms += rms_alpha * (frame_rms - state.smoothed_rms)

    return {
        "input_peak": input_peak,
        "input_energy": input_energy,
        "filtered_peak": filtered_peak,
        "filtered_energy": filtered_energy,
        "filtered_rms": state.smoothed_rms,
        "filtered_envelope": envelope,
    }


def update_gate(level_info, state: GateState, config: AgcConfig) -> bool:
    open_threshold = config.gate_threshold
    close_threshold = config.gate_threshold * 0.7
    activity = level_info["filtered_envelope"]
    hold_frames = max(int(config.gate_hold_ms / config.frame_ms), 0)

    if (not state.is_open) and activity >= open_threshold:
        state.is_open = True
        state.hold_frames_remaining = hold_frames
    else:
        if activity >= close_threshold:
            state.is_open = True
            state.hold_frames_remaining = hold_frames
        elif state.hold_frames_remaining > 0:
            state.hold_frames_remaining -= 1
            state.is_open = True
        else:
            state.is_open = False
    return state.is_open


def time_constant_to_alpha(time_ms: float, config: AgcConfig) -> float:
    dt = config.frame_ms * 0.001
    tau = time_ms * 0.001
    if tau <= 1.0e-6:
        return 1.0
    return 1.0 - math.exp(-dt / tau)


def smoothstep(edge0: float, edge1: float, x: float) -> float:
    if edge1 <= edge0:
        return 1.0 if x >= edge1 else 0.0
    t = (x - edge0) / (edge1 - edge0)
    if t < 0.0:
        t = 0.0
    elif t > 1.0:
        t = 1.0
    return t * t * (3.0 - 2.0 * t)


def peak_bias_weight(blend_weight: float) -> float:
    if blend_weight <= 0.0:
        return 0.0
    if blend_weight >= 1.0:
        return 1.0
    keep_rms = 1.0 - blend_weight
    return 1.0 - keep_rms * keep_rms * keep_rms * keep_rms


def effective_target_peak_fs(config: AgcConfig, smoothed_crest_factor_db: float) -> float:
    target_peak = target_peak_fs(config.target_rms_fs)
    low_crest_target_peak = config.target_rms_fs + (target_peak - config.target_rms_fs) * 0.25
    high_crest_mix = smoothstep(config.cf_low_db, config.cf_high_db, smoothed_crest_factor_db)
    return low_crest_target_peak + high_crest_mix * (target_peak - low_crest_target_peak)


def compute_desired_gain(level_info, gate_open: bool, state: GainState, config: AgcConfig) -> float:
    state.headroom_limited = False
    state.crest_smoothing_active = False
    if not gate_open:
        state.cf_blend_weight = 0.0
        return 1.0
    level = level_info["filtered_rms"]
    if level <= 1.0e-9:
        state.cf_blend_weight = 0.0
        return 1.0
    gain_rms = config.target_rms_fs / level
    if level_info["input_peak"] > 1.0e-6:
        gain_peak = target_peak_fs(config.target_rms_fs) / level_info["input_peak"]
        crest_factor_db = linear_to_db(level_info["input_peak"] / level)
    else:
        gain_peak = config.max_gain
        crest_factor_db = 0.0
    state.current_crest_factor_db = crest_factor_db
    crest_update_allowed = gate_open and level > config.rms_activity_floor
    state.crest_smoothing_active = crest_update_allowed
    if crest_update_allowed:
        cf_alpha = time_constant_to_alpha(
            config.cf_rise_ms if crest_factor_db > state.smoothed_crest_factor_db else config.cf_fall_ms,
            config,
        )
        state.smoothed_crest_factor_db += cf_alpha * (crest_factor_db - state.smoothed_crest_factor_db)
    blend_weight = smoothstep(config.cf_low_db, config.cf_high_db, state.smoothed_crest_factor_db) if config.cfagc_enabled else 0.0
    effective_blend_weight = blend_weight * blend_weight
    if gain_peak < gain_rms and blend_weight > 0.0:
        effective_blend_weight = peak_bias_weight(blend_weight)
    gain_peak = effective_target_peak_fs(config, state.smoothed_crest_factor_db) / level_info["input_peak"] if level_info["input_peak"] > 1.0e-6 else config.max_gain
    if gain_rms > 1.0e-9 and gain_peak > 1.0e-9:
        gain = math.exp((1.0 - effective_blend_weight) * math.log(gain_rms) +
                        effective_blend_weight * math.log(gain_peak))
    else:
        gain = (1.0 - effective_blend_weight) * gain_rms + effective_blend_weight * gain_peak
    state.cf_blend_weight = effective_blend_weight
    if gain_peak < gain_rms and effective_blend_weight > 0.0:
        state.headroom_limited = True
    gain = min(gain, config.max_gain)
    return max(gain, 0.0)


def smooth_gain(desired_gain: float, fast_rise: bool, state: GainState, config: AgcConfig) -> float:
    current = state.applied_gain
    if desired_gain < current or (fast_rise and desired_gain > current):
        alpha = time_constant_to_alpha(config.attack_ms, config)
    else:
        alpha = time_constant_to_alpha(config.release_ms, config)
    state.desired_gain = desired_gain
    state.applied_gain = current + alpha * (desired_gain - current)
    return state.applied_gain


def apply_gain(frame, gain: float, state: GainState):
    state.overflow_detected = False
    out = []
    for value in frame:
        scaled = value * gain
        if abs(scaled) > 1.0:
            state.overflow_detected = True
        out.append(scaled)
    state.applied_gain = gain
    return out


def soft_knee_output_db(input_db: float, threshold_db: float, ratio: float, knee_db: float) -> float:
    half_knee = 0.5 * knee_db
    lower = threshold_db - half_knee
    upper = threshold_db + half_knee
    if ratio <= 1.0:
        return input_db
    if knee_db <= 0.0:
        if input_db <= threshold_db:
            return input_db
        return threshold_db + (input_db - threshold_db) / ratio
    if input_db <= lower:
        return input_db
    if input_db >= upper:
        return threshold_db + (input_db - threshold_db) / ratio
    compressed_db = threshold_db + (input_db - threshold_db) / ratio
    t = (input_db - lower) / knee_db
    return input_db + t * t * (compressed_db - input_db)


def apply_compressor(frame, state: CompressorState, config: AgcConfig):
    state.is_active = False
    state.max_gain_reduction_db = 0.0
    if (not config.compressor_enabled) or config.compressor_ratio <= 1.0:
        return frame
    out = []
    for value in frame:
        abs_value = abs(value)
        if abs_value > 1.0e-9:
            input_db = linear_to_db(abs_value)
            output_db = soft_knee_output_db(input_db, config.compressor_threshold_dbfs, config.compressor_ratio, config.compressor_knee_db)
            gain_reduction_db = input_db - output_db
            if gain_reduction_db > 0.0:
                gain = db_to_linear(-gain_reduction_db)
                value *= gain
                state.is_active = True
                if gain_reduction_db > state.max_gain_reduction_db:
                    state.max_gain_reduction_db = gain_reduction_db
        out.append(value)
    return out


def apply_peak_protector(frame, state: PeakProtectorState, config: AgcConfig):
    state.is_active = False
    state.applied_scale = 1.0
    if not config.peak_protector_enabled:
        return frame
    peak = max(abs(x) for x in frame) if frame else 0.0
    safe_peak = config.limiter_threshold * config.peak_protector_ratio
    if peak > safe_peak and peak > 1.0e-6:
        scale = safe_peak / peak
        state.is_active = True
        state.applied_scale = scale
        return [x * scale for x in frame]
    return frame


def soft_limit_sample(x: float, threshold: float) -> float:
    abs_value = abs(x)
    sign = 1.0 if x >= 0.0 else -1.0
    if abs_value <= threshold:
        return x
    headroom = 1.0 - threshold
    if headroom <= 1.0e-6:
        return sign * threshold
    overshoot = (abs_value - threshold) / headroom
    compressed = threshold + headroom * math.tanh(overshoot)
    return sign * min(compressed, 1.0)


def apply_limiter(frame, state: LimiterState, config: AgcConfig):
    state.is_active = False
    state.last_gain_reduction = 0.0
    out = []
    for value in frame:
        abs_value = abs(value)
        if abs_value > config.limiter_threshold:
            limited = soft_limit_sample(value, config.limiter_threshold)
            reduction = abs_value - abs(limited)
            state.is_active = True
            if reduction > state.last_gain_reduction:
                state.last_gain_reduction = reduction
            value = limited
        out.append(value)
    return out


def process_file(input_path: Path, output_path: Path, target_rms_percent: int, max_gain_db: int, gate_threshold_percent: int, mode: str):
    sample_rate, input_samples = read_wav_mono_s16(input_path)
    config = agc_config_preset(sample_rate, mode)
    config.target_rms_fs = target_rms_percent / 100.0
    config.max_gain = db_to_linear(float(max_gain_db))
    config.gate_threshold = gate_threshold_percent / 100.0
    config.limiter_threshold = limiter_threshold_fs(config.target_rms_fs)

    state = PipelineState(
        detector=DetectorState(),
        gate=GateState(),
        gain=GainState(),
        compressor=CompressorState(),
        peak_protector=PeakProtectorState(),
        limiter=LimiterState(),
    )

    output_samples = [0.0] * len(input_samples)
    frame_samples = config.frame_samples
    frames = 0
    gate_frames = 0
    headroom_limited_frames = 0
    compressor_frames = 0
    peak_protector_frames = 0
    limiter_frames = 0
    max_applied_gain = 0.0
    max_compressor_gain_reduction_db = 0.0

    for frame_start in range(0, len(input_samples), frame_samples):
        frame = input_samples[frame_start:frame_start + frame_samples]
        if len(frame) < frame_samples:
            frame = frame + [0.0] * (frame_samples - len(frame))

        previous_gate_open = state.gate.is_open
        level_info = measure_level(frame, state.detector, config)
        gate_open = update_gate(level_info, state.gate, config)
        desired_gain = compute_desired_gain(level_info, gate_open, state.gain, config)
        fast_rise = (not previous_gate_open) and gate_open and desired_gain > state.gain.applied_gain
        applied_gain = smooth_gain(desired_gain, fast_rise, state.gain, config)
        gained = apply_gain(frame, applied_gain, state.gain)
        gained = apply_compressor(gained, state.compressor, config)
        gained = apply_peak_protector(gained, state.peak_protector, config)
        gained = apply_limiter(gained, state.limiter, config)

        current = min(frame_samples, len(input_samples) - frame_start)
        output_samples[frame_start:frame_start + current] = gained[:current]

        frames += 1
        gate_frames += 1 if gate_open else 0
        headroom_limited_frames += 1 if state.gain.headroom_limited else 0
        compressor_frames += 1 if state.compressor.is_active else 0
        peak_protector_frames += 1 if state.peak_protector.is_active else 0
        limiter_frames += 1 if state.limiter.is_active else 0
        max_applied_gain = max(max_applied_gain, state.gain.applied_gain)
        max_compressor_gain_reduction_db = max(max_compressor_gain_reduction_db, state.compressor.max_gain_reduction_db)

    input_peak = max(abs(x) for x in input_samples) if input_samples else 0.0
    output_peak = max(abs(x) for x in output_samples) if output_samples else 0.0
    input_rms = math.sqrt(sum(x * x for x in input_samples) / len(input_samples)) if input_samples else 0.0
    output_rms = math.sqrt(sum(x * x for x in output_samples) / len(output_samples)) if output_samples else 0.0

    output_path.parent.mkdir(parents=True, exist_ok=True)
    write_wav_mono_s16(output_path, output_samples, sample_rate)

    print(str(input_path))
    print(f"  mode={config.mode}")
    print(f"  sample_rate={sample_rate} samples={len(input_samples)} frames={frames} frame_samples={frame_samples}")
    print(f"  input_peak={input_peak:.4f} input_rms={input_rms:.4f}")
    print(f"  output_peak={output_peak:.4f} output_rms={output_rms:.4f}")
    print(f"  gate_frames={gate_frames} headroom_limited_frames={headroom_limited_frames} compressor_frames={compressor_frames} peak_protector_frames={peak_protector_frames} limiter_frames={limiter_frames}")
    print(f"  max_gain={max_applied_gain:.4f} max_compressor_gain_reduction_db={max_compressor_gain_reduction_db:.2f}")


def main(argv):
    if len(argv) < 3:
        print("Usage: python tools/agc_batch_wav_reference.py <input.wav> <output.wav> [target_rms_percent] [max_gain_db] [gate_threshold_percent] [mode]")
        return 1
    input_path = Path(argv[1])
    output_path = Path(argv[2])
    target_rms_percent = int(argv[3]) if len(argv) >= 4 else 80
    max_gain_db = int(argv[4]) if len(argv) >= 5 else 18
    gate_threshold_percent = int(argv[5]) if len(argv) >= 6 else 5
    mode = argv[6] if len(argv) >= 7 else "am"
    process_file(input_path, output_path, target_rms_percent, max_gain_db, gate_threshold_percent, mode)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))

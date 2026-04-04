import argparse
import json
import math
from pathlib import Path

from agc_batch_wav_reference import (
    CompressorState,
    DetectorState,
    GainState,
    GateState,
    LimiterState,
    PeakProtectorState,
    PipelineState,
    agc_config_preset,
    apply_compressor,
    apply_gain,
    apply_limiter,
    apply_peak_protector,
    db_to_linear,
    limiter_threshold_fs,
    measure_level,
    read_wav_mono_s16,
    smooth_gain,
    target_peak_fs,
    update_gate,
    write_wav_mono_s16,
    compute_desired_gain,
)


def sample_stats(samples):
    if not samples:
        return {"peak": 0.0, "rms": 0.0}
    peak = max(abs(x) for x in samples)
    rms = math.sqrt(sum(x * x for x in samples) / len(samples))
    return {"peak": peak, "rms": rms}


def summarize_samples(samples):
    return [round(float(x), 6) for x in samples]


def export_debug(input_path: Path,
                 output_json: Path,
                 output_wav: Path | None,
                 target_rms_percent: int,
                 max_gain_db: int,
                 gate_threshold_percent: int,
                 mode: str):
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

    frame_samples = config.frame_samples
    output_samples = [0.0] * len(input_samples)
    frames = []

    for frame_index, frame_start in enumerate(range(0, len(input_samples), frame_samples)):
        frame_input = input_samples[frame_start:frame_start + frame_samples]
        padded_input = frame_input + [0.0] * (frame_samples - len(frame_input))
        previous_gate_open = state.gate.is_open

        level_info = measure_level(padded_input, state.detector, config)
        gate_open = update_gate(level_info, state.gate, config)
        desired_gain = compute_desired_gain(level_info, gate_open, state.gain, config)
        fast_rise = (not previous_gate_open) and gate_open and desired_gain > state.gain.applied_gain
        applied_gain = smooth_gain(desired_gain, fast_rise, state.gain, config)

        gained = apply_gain(padded_input, applied_gain, state.gain)
        after_gain = gained[:]
        after_compressor = apply_compressor(after_gain, state.compressor, config)
        after_peak_protector = apply_peak_protector(after_compressor, state.peak_protector, config)
        after_limiter = apply_limiter(after_peak_protector, state.limiter, config)

        valid_count = len(frame_input)
        output_samples[frame_start:frame_start + valid_count] = after_limiter[:valid_count]

        input_stats = sample_stats(frame_input)
        gain_stats = sample_stats(after_gain[:valid_count])
        compressor_stats = sample_stats(after_compressor[:valid_count])
        peak_protector_stats = sample_stats(after_peak_protector[:valid_count])
        output_stats = sample_stats(after_limiter[:valid_count])

        frames.append({
            "index": frame_index,
            "start_sample": frame_start,
            "end_sample": frame_start + max(valid_count - 1, 0),
            "input_peak": round(level_info["input_peak"], 6),
            "input_rms": round(input_stats["rms"], 6),
            "envelope": round(level_info["filtered_envelope"], 6),
            "smoothed_rms": round(level_info["filtered_rms"], 6),
            "crest_factor_db": round(20.0 * math.log10(level_info["input_peak"] / max(level_info["filtered_rms"], 1.0e-9))
                                     if level_info["input_peak"] > 1.0e-9 and level_info["filtered_rms"] > 1.0e-9 else 0.0, 6),
            "smoothed_crest_factor_db": round(state.gain.smoothed_crest_factor_db, 6),
            "crest_smoothing_active": state.gain.crest_smoothing_active,
            "gate_open": gate_open,
            "gate_hold_frames_remaining": int(state.gate.hold_frames_remaining),
            "desired_gain": round(desired_gain, 6),
            "applied_gain": round(applied_gain, 6),
            "cf_blend_weight": round(state.gain.cf_blend_weight, 6),
            "headroom_limited": state.gain.headroom_limited,
            "overflow_detected": state.gain.overflow_detected,
            "compressor_active": state.compressor.is_active,
            "compressor_gain_reduction_db": round(state.compressor.max_gain_reduction_db, 6),
            "peak_protector_active": state.peak_protector.is_active,
            "peak_protector_scale": round(state.peak_protector.applied_scale, 6),
            "limiter_active": state.limiter.is_active,
            "limiter_reduction": round(state.limiter.last_gain_reduction, 6),
            "stage_input_peak": round(input_stats["peak"], 6),
            "stage_gain_peak": round(gain_stats["peak"], 6),
            "stage_compressor_peak": round(compressor_stats["peak"], 6),
            "stage_peak_protector_peak": round(peak_protector_stats["peak"], 6),
            "stage_output_peak": round(output_stats["peak"], 6),
            "stage_output_rms": round(output_stats["rms"], 6),
            "frame_input": summarize_samples(frame_input),
            "frame_output": summarize_samples(after_limiter[:valid_count]),
        })

    input_summary = sample_stats(input_samples)
    output_summary = sample_stats(output_samples)
    limiter_frames = sum(1 for frame in frames if frame["limiter_active"])
    compressor_frames = sum(1 for frame in frames if frame["compressor_active"])
    gate_frames = sum(1 for frame in frames if frame["gate_open"])
    headroom_limited_frames = sum(1 for frame in frames if frame["headroom_limited"])

    payload = {
        "metadata": {
            "input_file": str(input_path),
            "sample_rate": sample_rate,
            "frame_samples": frame_samples,
            "frame_ms": config.frame_ms,
            "mode": config.mode,
            "target_rms_percent": target_rms_percent,
            "max_gain_db": max_gain_db,
            "gate_threshold_percent": gate_threshold_percent,
            "target_peak": round(target_peak_fs(config.target_rms_fs), 6),
            "limiter_threshold": round(config.limiter_threshold, 6),
        },
        "summary": {
            "input_peak": round(input_summary["peak"], 6),
            "input_rms": round(input_summary["rms"], 6),
            "output_peak": round(output_summary["peak"], 6),
            "output_rms": round(output_summary["rms"], 6),
            "frames": len(frames),
            "gate_frames": gate_frames,
            "headroom_limited_frames": headroom_limited_frames,
            "compressor_frames": compressor_frames,
            "limiter_frames": limiter_frames,
        },
        "config": {
            "target_rms_fs": round(config.target_rms_fs, 6),
            "max_gain": round(config.max_gain, 6),
            "attack_ms": round(config.attack_ms, 6),
            "release_ms": round(config.release_ms, 6),
            "envelope_window_ms": round(config.envelope_window_ms, 6),
            "rms_window_ms": round(config.rms_window_ms, 6),
            "gate_hold_ms": round(config.gate_hold_ms, 6),
            "gate_threshold": round(config.gate_threshold, 6),
            "cfagc_enabled": config.cfagc_enabled,
            "cf_low_db": round(config.cf_low_db, 6),
            "cf_high_db": round(config.cf_high_db, 6),
            "cf_rise_ms": round(config.cf_rise_ms, 6),
            "cf_fall_ms": round(config.cf_fall_ms, 6),
            "rms_activity_floor": round(config.rms_activity_floor, 6),
            "compressor_enabled": config.compressor_enabled,
            "compressor_threshold_dbfs": round(config.compressor_threshold_dbfs, 6),
            "compressor_ratio": round(config.compressor_ratio, 6),
            "compressor_knee_db": round(config.compressor_knee_db, 6),
        },
        "input_samples": summarize_samples(input_samples),
        "output_samples": summarize_samples(output_samples),
        "frames": frames,
    }

    output_json.parent.mkdir(parents=True, exist_ok=True)
    output_json.write_text(json.dumps(payload, indent=2), encoding="utf-8")

    if output_wav is not None:
        output_wav.parent.mkdir(parents=True, exist_ok=True)
        write_wav_mono_s16(output_wav, output_samples, sample_rate)

    print(f"Exported debug trace: {output_json}")
    if output_wav is not None:
        print(f"Exported output wav: {output_wav}")


def main():
    parser = argparse.ArgumentParser(description="Export AGC debug trace JSON for a WAV input.")
    parser.add_argument("input_wav")
    parser.add_argument("output_json")
    parser.add_argument("--output-wav", default=None)
    parser.add_argument("--target-rms-percent", type=int, default=80)
    parser.add_argument("--max-gain-db", type=int, default=18)
    parser.add_argument("--gate-threshold-percent", type=int, default=5)
    parser.add_argument("--mode", default="am")
    args = parser.parse_args()

    export_debug(
        Path(args.input_wav),
        Path(args.output_json),
        Path(args.output_wav) if args.output_wav else None,
        args.target_rms_percent,
        args.max_gain_db,
        args.gate_threshold_percent,
        args.mode,
    )


if __name__ == "__main__":
    main()

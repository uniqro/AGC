#ifndef AGC_FIXED_TYPES_H
#define AGC_FIXED_TYPES_H

#include <stdint.h>

/*
 * Core fixed-point types for the first DSP-oriented AGC port.
 *
 * Signal samples follow the project decision:
 * - external input/output: int16
 * - internal signal domain: Q15
 *
 * Gain needs headroom above 1.0, so it uses a wider dedicated format.
 */

typedef int16_t agc_q15_t;
typedef int32_t agc_q31_t;
typedef int32_t agc_gain_q12_20_t;
typedef int32_t agc_accum32_t;
typedef int64_t agc_accum40_t;
typedef uint16_t agc_frame_count_t;
/*
 * Host-side stand-in for C54x 40-bit accumulator style paths.
 * Critical internal sums can use this wider type without changing the
 * public Q15/Q20 interface.
 */

enum {
    AGC_FIXED_MAX_FRAME_SAMPLES = 512
};

enum {
    AGC_Q15_SHIFT = 15,
    AGC_Q31_SHIFT = 31,
    AGC_GAIN_Q12_20_SHIFT = 20
};

enum {
    AGC_Q15_ONE = 1 << AGC_Q15_SHIFT,
    AGC_GAIN_Q20_ONE = 1 << AGC_GAIN_Q12_20_SHIFT
};

/*
 * Detector state is intentionally small and persistent.
 *
 * - envelope_q15:
 *   short-term absolute-amplitude follower used mainly by the gate path
 *
 * - smoothed_rms_q15:
 *   slower RMS-like level estimate used by the AGC gain law
 */
typedef struct AgcFixedDetectorState {
    agc_q15_t envelope_q15;
    agc_q15_t smoothed_rms_q15;
} AgcFixedDetectorState;

/*
 * Gate state tracks whether the AGC is currently allowed to react to the
 * signal and how many frames the gate should remain open after activity drops.
 */
typedef struct AgcFixedGateState {
    int16_t is_open;
    int16_t hold_frames_remaining;
} AgcFixedGateState;

/*
 * Gain state stores both the target and currently applied gain.
 *
 * - desired_gain_q20:
 *   instantaneous gain request computed from target RMS and measured RMS
 *
 * - applied_gain_q20:
 *   smoothed gain after attack/release filtering
 *
 * - overflow_detected:
 *   debug/metrics aid showing whether the wide gain path exceeded int16 range
 *   before final saturation
 */
typedef struct AgcFixedGainState {
    agc_gain_q12_20_t desired_gain_q20;
    agc_gain_q12_20_t applied_gain_q20;
    int16_t overflow_detected;
} AgcFixedGainState;

/*
 * Peak protector state records whether the frame-level safety scaler was used.
 * This block is separate from the slower AGC gain loop.
 */
typedef struct AgcFixedPeakProtectorState {
    int16_t is_active;
    agc_q15_t applied_scale_q15;
} AgcFixedPeakProtectorState;

/*
 * Limiter state records whether the final hard safety clamp was touched.
 * In the current baseline, limiter activity should stay rare.
 */
typedef struct AgcFixedLimiterState {
    int16_t is_active;
    agc_q15_t last_gain_reduction_q15;
} AgcFixedLimiterState;

/*
 * Full pipeline state for one AGC instance.
 *
 * - gain_buffer_q15:
 *   wide intermediate frame buffer after gain application and before final
 *   peak protection / limiting / int16 conversion
 *
 * - frame_index:
 *   monotonically increasing frame counter, useful for metrics and debugging
 */
typedef struct AgcFixedPipelineState {
    AgcFixedDetectorState detector;
    AgcFixedGateState gate;
    AgcFixedGainState gain;
    AgcFixedPeakProtectorState peak_protector;
    AgcFixedLimiterState limiter;
    agc_accum40_t gain_buffer_q15[AGC_FIXED_MAX_FRAME_SAMPLES];
    uint32_t frame_index;
} AgcFixedPipelineState;

#endif

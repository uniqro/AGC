#ifndef AGC_FIXED_MATH_H
#define AGC_FIXED_MATH_H

#include <stdint.h>

#include "agc_fixed_types.h"

static inline agc_q15_t agc_sat_q15(int32_t x) {
    if (x > 32767) {
        return 32767;
    }
    if (x < -32768) {
        return -32768;
    }
    return (agc_q15_t)x;
}

static inline int16_t agc_sat_s16(int32_t x) {
    if (x > 32767) {
        return 32767;
    }
    if (x < -32768) {
        return -32768;
    }
    return (int16_t)x;
}

static inline agc_q15_t agc_ratio_to_q15(int32_t num, int32_t den) {
    int32_t scaled;

    if (den <= 0) {
        return 0;
    }

    scaled = (int32_t)(((int64_t)num << AGC_Q15_SHIFT) / den);
    return agc_sat_q15(scaled);
}

static inline agc_gain_q12_20_t agc_ratio_to_gain_q20(int32_t num, int32_t den) {
    if (den <= 0) {
        return 0;
    }
    return (agc_gain_q12_20_t)(((int64_t)num << AGC_GAIN_Q12_20_SHIFT) / den);
}

static inline int32_t agc_q15_mul_q15(agc_q15_t a, agc_q15_t b) {
    return ((int32_t)a * (int32_t)b) >> AGC_Q15_SHIFT;
}

static inline agc_accum40_t agc_q15_mul_gain_q20(agc_q15_t a, agc_gain_q12_20_t b) {
    return ((agc_accum40_t)a * (agc_accum40_t)b) >> AGC_GAIN_Q12_20_SHIFT;
}

static inline agc_accum40_t agc_accum40_mul_q15(agc_accum40_t a_q15, agc_q15_t b_q15) {
    return (agc_accum40_t)((a_q15 * (agc_accum40_t)b_q15) >> AGC_Q15_SHIFT);
}

static inline agc_gain_q12_20_t agc_q15_div_to_gain_q20(agc_q15_t num_q15, agc_q15_t den_q15) {
    if (den_q15 <= 0) {
        return 0;
    }
    return (agc_gain_q12_20_t)(((int64_t)num_q15 << AGC_GAIN_Q12_20_SHIFT) / den_q15);
}

static inline agc_q15_t agc_gain_q20_to_q15(agc_gain_q12_20_t gain_q20) {
    return agc_sat_q15((int32_t)(gain_q20 >> (AGC_GAIN_Q12_20_SHIFT - AGC_Q15_SHIFT)));
}

#endif

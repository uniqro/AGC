#ifndef AGC_FIXED_BUILD_H
#define AGC_FIXED_BUILD_H

/*
 * Build-time switches for DSP-oriented builds.
 *
 * Keep metrics enabled by default for development and reference comparison.
 * Release-oriented builds can set this to 0 at compile time.
 */
#ifndef AGC_FIXED_ENABLE_METRICS
#define AGC_FIXED_ENABLE_METRICS 1
#endif

#endif

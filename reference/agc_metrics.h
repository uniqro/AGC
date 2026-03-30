#ifndef AGC_METRICS_H
#define AGC_METRICS_H

#include "agc_types.h"

FrameMetrics agc_collect_metrics(const LevelInfo *level_info,
                                 int gate_open,
                                 const GainState *gain_state,
                                 const PeakProtectorState *peak_protector_state,
                                 const LimiterState *limiter_state,
                                 float output_peak,
                                 float output_energy);

#endif

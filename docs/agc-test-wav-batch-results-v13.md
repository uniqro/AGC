# AGC Test WAV Batch Results V13

## Tarih

- 2026-04-04

## Kullanilan Parametreler

- `mode = AM`
- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- `CFAGC enabled`
- `cf_low_db = 6.0`
- `cf_high_db = 10.5`
- `cf_rise_ms = 8`
- `cf_fall_ms = 40`
- `rms_activity_floor = 0.05`
- `target_peak = target_level + (1 - target_level) * 2/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`
- `compressor = passive`
- `blend = convex (w^2), log-gain domain`
- `crest smoothing update = gate_open && smoothed_rms > 0.05`

## Output

- `test_wav/agc_output_v13/test_-01_agc_output_v13.wav`
- `test_wav/agc_output_v13/test_-02_agc_output_v13.wav`
- `test_wav/agc_output_v13/test_-03_agc_output_v13.wav`
- `test_wav/agc_output_v13/test_-04_agc_output_v13.wav`
- `test_wav/agc_output_v13/test_-05_agc_output_v13.wav`

## Sonuclar

### test_-01
- `output_rms = 0.2367`
- `headroom_limited_frames = 1126`
- `limiter_frames = 199`

### test_-02
- `output_rms = 0.0926`
- `headroom_limited_frames = 155`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.4098`
- `headroom_limited_frames = 1165`
- `limiter_frames = 795`

### test_-04
- `output_rms = 0.2730`
- `headroom_limited_frames = 1122`
- `limiter_frames = 298`

### test_-05
- `output_rms = 0.2170`
- `headroom_limited_frames = 1066`
- `limiter_frames = 137`

## V12 Ile Karsilastirma

- `test_-01`: `rms 0.3035 -> 0.2367`, `headroom 852 -> 1126`, `limiter 369 -> 199`
- `test_-02`: `rms 0.0926 -> 0.0926`, `headroom 83 -> 155`, `limiter 0 -> 0`
- `test_-03`: `rms 0.4651 -> 0.4098`, `headroom 766 -> 1165`, `limiter 956 -> 795`
- `test_-04`: `rms 0.3700 -> 0.2730`, `headroom 950 -> 1122`, `limiter 566 -> 298`
- `test_-05`: `rms 0.2658 -> 0.2170`, `headroom 794 -> 1066`, `limiter 277 -> 137`

## Kisa Yorum

- `rise-fast / fall-slow` crest smoothing, gec kalan CFAGC problemini limiter yukune yansiyacak kadar iyilestirdi.
- Sicak dosyalarda limiter sayisi belirgin bicimde dustu.
- Ancak bunun bedeli, headroom-aware davranisin cok daha fazla frame'de devreye girmesi ve RMS'in bariz azalmasi oldu.
- Yani yon dogru, fakat bu ilk asimetrik ayar fazla korumaci kaldi.
- Sonraki mantikli adim, `cf_rise_ms` / `cf_fall_ms` dengesini yumusatmak veya blend agirligini biraz geri acmak olur.

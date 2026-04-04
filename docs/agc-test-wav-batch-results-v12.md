# AGC Test WAV Batch Results V12

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
- `cf_smoothing_ms = 40`
- `rms_activity_floor = 0.05`
- `target_peak = target_level + (1 - target_level) * 2/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`
- `compressor = passive`
- `blend = convex (w^2), log-gain domain`
- `crest smoothing update = gate_open && smoothed_rms > 0.05`

## Output

- `test_wav/agc_output_v12/test_-01_agc_output_v12.wav`
- `test_wav/agc_output_v12/test_-02_agc_output_v12.wav`
- `test_wav/agc_output_v12/test_-03_agc_output_v12.wav`
- `test_wav/agc_output_v12/test_-04_agc_output_v12.wav`
- `test_wav/agc_output_v12/test_-05_agc_output_v12.wav`

## Sonuclar

### test_-01
- `output_rms = 0.3035`
- `headroom_limited_frames = 852`
- `limiter_frames = 369`

### test_-02
- `output_rms = 0.0926`
- `headroom_limited_frames = 83`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.4651`
- `headroom_limited_frames = 766`
- `limiter_frames = 956`

### test_-04
- `output_rms = 0.3700`
- `headroom_limited_frames = 950`
- `limiter_frames = 566`

### test_-05
- `output_rms = 0.2658`
- `headroom_limited_frames = 794`
- `limiter_frames = 277`

## V11 Ile Karsilastirma

- `test_-01`: `rms 0.3004 -> 0.3035`, `headroom 889 -> 852`, `limiter 362 -> 369`
- `test_-02`: `rms 0.0926 -> 0.0926`, `headroom 265 -> 83`, `limiter 0 -> 0`
- `test_-03`: `rms 0.4619 -> 0.4651`, `headroom 902 -> 766`, `limiter 955 -> 956`
- `test_-04`: `rms 0.3588 -> 0.3700`, `headroom 1001 -> 950`, `limiter 538 -> 566`
- `test_-05`: `rms 0.2628 -> 0.2658`, `headroom 857 -> 794`, `limiter 276 -> 277`

## Kisa Yorum

- `rms_activity_floor = 0.05` ile crest smoothing daha secici hale geldi.
- Bu degisiklik `headroom_limited_frames` sayisini belirgin azaltirken, RMS'i hafif yukari tasidi.
- Ancak limiter yukunde beklenen rahatlama gelmedi; sicak dosyalarda limiter ya ayni kaldı ya da bir miktar artti.
- Bu da problemin yalniz dusuk enerjili frame'lerin CF state'ini kirletmesi olmadigini gosteriyor.
- Sonraki tuning ekseni buyuk olasilikla `crest rise / fall asymmetry` veya `compressor`in kontrollu geri donusu olacak.

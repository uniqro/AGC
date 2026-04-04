# AGC Test WAV Batch Results V10

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
- `target_peak = target_level + (1 - target_level) * 4/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`
- `compressor = passive`
- `blend = convex (w^2), log-gain domain`

## Output

- `test_wav/agc_output_v10/test_-01_agc_output_v10.wav`
- `test_wav/agc_output_v10/test_-02_agc_output_v10.wav`
- `test_wav/agc_output_v10/test_-03_agc_output_v10.wav`
- `test_wav/agc_output_v10/test_-04_agc_output_v10.wav`
- `test_wav/agc_output_v10/test_-05_agc_output_v10.wav`

## Sonuclar

### test_-01
- `output_rms = 0.3020`
- `headroom_limited_frames = 882`
- `limiter_frames = 370`

### test_-02
- `output_rms = 0.0926`
- `headroom_limited_frames = 263`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.4627`
- `headroom_limited_frames = 895`
- `limiter_frames = 956`

### test_-04
- `output_rms = 0.3622`
- `headroom_limited_frames = 993`
- `limiter_frames = 545`

### test_-05
- `output_rms = 0.2646`
- `headroom_limited_frames = 852`
- `limiter_frames = 278`

## V9 Ile Karsilastirma

- Log-gain blend, `v9`a gore limiter yukunu az da olsa dusurdu:
  - `test_-01`: `376 -> 370`
  - `test_-03`: `958 -> 956`
  - `test_-04`: `563 -> 545`
  - `test_-05`: `283 -> 278`
- RMS seviyesi buyuk olcude korundu:
  - `test_-01`: `0.3062 -> 0.3020`
  - `test_-03`: `0.4645 -> 0.4627`
  - `test_-04`: `0.3700 -> 0.3622`
  - `test_-05`: `0.2687 -> 0.2646`

## Kisa Yorum

- Bu deneme, `target_peak`e dokunmadan gain law tarafinda daha korumaci bir adim oldu.
- Kazanim sinirli ama yon dogru: limiter yukunde kucuk bir dusus, RMS'te ise gorece kucuk kayip var.
- Bu nedenle `log-gain blend`, `CFAGC` icin onceki lineer kazanc blend'inden daha iyi bir ara aday olarak gorulebilir.

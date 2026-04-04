# AGC Test WAV Batch Results V11

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
- `target_peak = target_level + (1 - target_level) * 2/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`
- `compressor = passive`
- `blend = convex (w^2), log-gain domain`

## Output

- `test_wav/agc_output_v11/test_-01_agc_output_v11.wav`
- `test_wav/agc_output_v11/test_-02_agc_output_v11.wav`
- `test_wav/agc_output_v11/test_-03_agc_output_v11.wav`
- `test_wav/agc_output_v11/test_-04_agc_output_v11.wav`
- `test_wav/agc_output_v11/test_-05_agc_output_v11.wav`

## Sonuclar

### test_-01
- `output_rms = 0.3004`
- `headroom_limited_frames = 889`
- `limiter_frames = 362`

### test_-02
- `output_rms = 0.0926`
- `headroom_limited_frames = 265`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.4619`
- `headroom_limited_frames = 902`
- `limiter_frames = 955`

### test_-04
- `output_rms = 0.3588`
- `headroom_limited_frames = 1001`
- `limiter_frames = 538`

### test_-05
- `output_rms = 0.2628`
- `headroom_limited_frames = 857`
- `limiter_frames = 276`

## V10 Ile Karsilastirma

- `target_peak` daha asagi cekilmesine ragmen etkisi sinirli kaldi:
  - `test_-01`: `limiter 370 -> 362`, `rms 0.3020 -> 0.3004`
  - `test_-03`: `limiter 956 -> 955`, `rms 0.4627 -> 0.4619`
  - `test_-04`: `limiter 545 -> 538`, `rms 0.3622 -> 0.3588`
  - `test_-05`: `limiter 278 -> 276`, `rms 0.2646 -> 0.2628`

## Kisa Yorum

- `target_peak`i daha muhafazakar yapmak beklenen kadar etkili olmadi.
- Limiter yukunde yalniz kucuk iyilesme goruldu.
- Bu, mevcut durumda limiter yukunun ana nedeninin yalniz peak hedefinden degil, genel gain tasima davranisindan ve son koruma zincirinden geldigini dusunduruyor.

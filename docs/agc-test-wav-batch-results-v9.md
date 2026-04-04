# AGC Test WAV Batch Results V9

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
- `blend = convex (w^2)`

## Output

- `test_wav/agc_output_v9/test_-01_agc_output_v9.wav`
- `test_wav/agc_output_v9/test_-02_agc_output_v9.wav`
- `test_wav/agc_output_v9/test_-03_agc_output_v9.wav`
- `test_wav/agc_output_v9/test_-04_agc_output_v9.wav`
- `test_wav/agc_output_v9/test_-05_agc_output_v9.wav`

## Sonuclar

### test_-01
- `output_rms = 0.3062`
- `headroom_limited_frames = 882`
- `limiter_frames = 376`

### test_-02
- `output_rms = 0.0926`
- `headroom_limited_frames = 263`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.4645`
- `headroom_limited_frames = 895`
- `limiter_frames = 958`

### test_-04
- `output_rms = 0.3700`
- `headroom_limited_frames = 993`
- `limiter_frames = 563`

### test_-05
- `output_rms = 0.2687`
- `headroom_limited_frames = 852`
- `limiter_frames = 283`

## V8 Ile Karsilastirma

- Convex blend, `v8`e gore RMS'i bir miktar geri kazandirdi:
  - `test_-01`: `0.2928 -> 0.3062`
  - `test_-03`: `0.4550 -> 0.4645`
  - `test_-04`: `0.3467 -> 0.3700`
  - `test_-05`: `0.2574 -> 0.2687`
- Bunun bedeli limiter yukunde kismi geri artis oldu:
  - `test_-03`: `940 -> 958`
  - `test_-04`: `503 -> 563`
  - `test_-05`: `263 -> 283`
- Buna ragmen `v9`, limiter yukunu `v7` seviyesine kadar geri tasimadan RMS'i toparlamayi basardi.

## Kisa Yorum

- `v9`, su ana kadar `v7` ve `v8` arasinda daha dengeli bir ara nokta gibi gorunuyor.
- Erken CFAGC devreye giriyor, ama lineer blend'e gore RMS'i biraz daha iyi koruyor.
- Hala limiter yuksek; yani optimum nokta bulunmus degil.
- Ancak bu deneme, esiklerden sonra blend fonksiyonunu sekillendirmenin gercekten etkili oldugunu gosterdi.

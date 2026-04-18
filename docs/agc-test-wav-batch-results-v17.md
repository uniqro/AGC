# AGC Test WAV Batch Results V17

## Tarih

- 2026-04-04

## Kullanilan Parametreler

- `mode = AM`
- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- `release_ms = 100`
- `CFAGC enabled`
- `cf_low_db = 6.0`
- `cf_high_db = 10.5`
- `cf_rise_ms = 8`
- `cf_fall_ms = 40`
- `rms_activity_floor = 0.05`
- `target_peak = target_level + (1 - target_level) * 2/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`
- `compressor = passive`
- `peak-biased weight = 1 - (1 - w)^4`

## Output

- `test_wav/agc_output_v17/test_-01_agc_output_v17.wav`
- `test_wav/agc_output_v17/test_-02_agc_output_v17.wav`
- `test_wav/agc_output_v17/test_-03_agc_output_v17.wav`
- `test_wav/agc_output_v17/test_-04_agc_output_v17.wav`
- `test_wav/agc_output_v17/test_-05_agc_output_v17.wav`

## Sonuclar

### test_-01
- `output_rms = 0.2248`
- `limiter_frames = 135`

### test_-02
- `output_rms = 0.1100`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.3770`
- `limiter_frames = 519`

### test_-04
- `output_rms = 0.2591`
- `limiter_frames = 218`

### test_-05
- `output_rms = 0.2159`
- `limiter_frames = 112`

## V14 Ile Karsilastirma

- `test_-01`: `rms 0.1999 -> 0.2248`, `limiter 88 -> 135`
- `test_-02`: `rms 0.0922 -> 0.1100`, `limiter 0 -> 0`
- `test_-03`: `rms 0.3539 -> 0.3770`, `limiter 316 -> 519`
- `test_-04`: `rms 0.2282 -> 0.2591`, `limiter 146 -> 218`
- `test_-05`: `rms 0.1933 -> 0.2159`, `limiter 73 -> 112`

## Kisa Yorum

- `release_ms = 100`, `v14`e gore RMS'i anlamli bicimde toparladi.
- Limiter yukunde de buna karsilik belirgin bir artis oldu.
- Buna ragmen son durum genel olarak `v13` kadar agresif degil; yani daha sicak ama halen kontrollu bir ara aday gibi gorunuyor.

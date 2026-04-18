# AGC Test WAV Batch Results V18

## Tarih

- 2026-04-04

## Kullanilan Parametreler

- `mode = AM`
- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- `release_ms = 150`
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

- `test_wav/agc_output_v18/test_-01_agc_output_v18.wav`
- `test_wav/agc_output_v18/test_-02_agc_output_v18.wav`
- `test_wav/agc_output_v18/test_-03_agc_output_v18.wav`
- `test_wav/agc_output_v18/test_-04_agc_output_v18.wav`
- `test_wav/agc_output_v18/test_-05_agc_output_v18.wav`

## Sonuclar

### test_-01
- `output_rms = 0.2147`
- `limiter_frames = 113`

### test_-02
- `output_rms = 0.1028`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.3679`
- `limiter_frames = 455`

### test_-04
- `output_rms = 0.2470`
- `limiter_frames = 185`

### test_-05
- `output_rms = 0.2067`
- `limiter_frames = 97`

## V14 ve V17 Ile Karsilastirma

- `test_-03`
  - `v14`: `rms 0.3539`, `limiter 316`
  - `v17`: `rms 0.3770`, `limiter 519`
  - `v18`: `rms 0.3679`, `limiter 455`

- `test_-04`
  - `v14`: `rms 0.2282`, `limiter 146`
  - `v17`: `rms 0.2591`, `limiter 218`
  - `v18`: `rms 0.2470`, `limiter 185`

- `test_-05`
  - `v14`: `rms 0.1933`, `limiter 73`
  - `v17`: `rms 0.2159`, `limiter 112`
  - `v18`: `rms 0.2067`, `limiter 97`

## Kisa Yorum

- `release_ms = 150`, bekledigimiz gibi `v14` ile `v17` arasinda orta bir nokta verdi.
- RMS toparlanmasi var.
- Limiter yukundeki artis da `v17` kadar yuksek degil.
- Bu nedenle `150 ms`, mevcut arayista dengeli bir aday olarak gorunuyor.

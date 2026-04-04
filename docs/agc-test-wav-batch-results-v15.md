# AGC Test WAV Batch Results V15

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
- `headroom-limited frame'lerde peak-biased weight = 1 - (1 - w)^3`

## Output

- `test_wav/agc_output_v15/test_-01_agc_output_v15.wav`
- `test_wav/agc_output_v15/test_-02_agc_output_v15.wav`
- `test_wav/agc_output_v15/test_-03_agc_output_v15.wav`
- `test_wav/agc_output_v15/test_-04_agc_output_v15.wav`
- `test_wav/agc_output_v15/test_-05_agc_output_v15.wav`

## Sonuclar

### test_-01
- `output_rms = 0.2023`
- `headroom_limited_frames = 1126`
- `limiter_frames = 92`

### test_-02
- `output_rms = 0.0923`
- `headroom_limited_frames = 155`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.3592`
- `headroom_limited_frames = 1165`
- `limiter_frames = 387`

### test_-04
- `output_rms = 0.2310`
- `headroom_limited_frames = 1122`
- `limiter_frames = 160`

### test_-05
- `output_rms = 0.1954`
- `headroom_limited_frames = 1066`
- `limiter_frames = 77`

## V14 Ile Karsilastirma

- `test_-01`: `rms 0.1999 -> 0.2023`, `limiter 88 -> 92`
- `test_-03`: `rms 0.3539 -> 0.3592`, `limiter 316 -> 387`
- `test_-04`: `rms 0.2282 -> 0.2310`, `limiter 146 -> 160`
- `test_-05`: `rms 0.1933 -> 0.1954`, `limiter 73 -> 77`

## Kisa Yorum

- Peak-biased agirligi `^4`ten `^3`e yumusatmak RMS'i cok hafif geri kazandirdi.
- Buna karsilik limiter sayisi tekrar biraz artti.
- Ancak genel davranis hala `v13`e gore daha dusuk RMS, daha dusuk limiter yukune sahip.
- Yani yon ayni: peak-biased blend ise yariyor, ama bu aile halen agresif kalıyor.

# AGC Test WAV Batch Results V14

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
- `headroom-limited frame'lerde peak-biased weight = 1 - (1 - w)^4`

## Output

- `test_wav/agc_output_v14/test_-01_agc_output_v14.wav`
- `test_wav/agc_output_v14/test_-02_agc_output_v14.wav`
- `test_wav/agc_output_v14/test_-03_agc_output_v14.wav`
- `test_wav/agc_output_v14/test_-04_agc_output_v14.wav`
- `test_wav/agc_output_v14/test_-05_agc_output_v14.wav`

## Sonuclar

### test_-01
- `output_rms = 0.1999`
- `headroom_limited_frames = 1126`
- `limiter_frames = 88`

### test_-02
- `output_rms = 0.0922`
- `headroom_limited_frames = 155`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.3539`
- `headroom_limited_frames = 1165`
- `limiter_frames = 316`

### test_-04
- `output_rms = 0.2282`
- `headroom_limited_frames = 1122`
- `limiter_frames = 146`

### test_-05
- `output_rms = 0.1933`
- `headroom_limited_frames = 1066`
- `limiter_frames = 73`

## V13 Ile Karsilastirma

- `test_-01`: `rms 0.2367 -> 0.1999`, `limiter 199 -> 88`
- `test_-03`: `rms 0.4098 -> 0.3539`, `limiter 795 -> 316`
- `test_-04`: `rms 0.2730 -> 0.2282`, `limiter 298 -> 146`
- `test_-05`: `rms 0.2170 -> 0.1933`, `limiter 137 -> 73`

## Kisa Yorum

- `desired_gain`i `gain_peak`e daha cok yaklastirma hedefi amacina ulasti.
- Ancak bu ilk peak-biased weight fazla sert kaldi.
- Limiter yukunde buyuk kazanc var, ama RMS seviyesi AM hedefi icin belirgin bicimde fazla geriye cekiliyor.
- Bir sonraki mantikli adim, bu weight'i yumusatmak veya sadece cok yuksek crest-factor bolgelerinde devreye girecek sekilde kosullamak olur.

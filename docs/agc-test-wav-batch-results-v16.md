# AGC Test WAV Batch Results V16

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
- `peak_bias_low_db = 8.5`
- `peak_bias_high_db = 10.5`
- `rms_activity_floor = 0.05`
- `target_peak = target_level + (1 - target_level) * 2/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`
- `compressor = passive`
- `headroom-limited frame'lerde peak-biased weight yalnız daha yuksek crest bandinda guclenir`

## Output

- `test_wav/agc_output_v16/test_-01_agc_output_v16.wav`
- `test_wav/agc_output_v16/test_-02_agc_output_v16.wav`
- `test_wav/agc_output_v16/test_-03_agc_output_v16.wav`
- `test_wav/agc_output_v16/test_-04_agc_output_v16.wav`
- `test_wav/agc_output_v16/test_-05_agc_output_v16.wav`

## Sonuclar

### test_-01
- `output_rms = 0.2298`
- `headroom_limited_frames = 1126`
- `limiter_frames = 173`

### test_-02
- `output_rms = 0.0926`
- `headroom_limited_frames = 155`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.4022`
- `headroom_limited_frames = 1165`
- `limiter_frames = 749`

### test_-04
- `output_rms = 0.2637`
- `headroom_limited_frames = 1122`
- `limiter_frames = 269`

### test_-05
- `output_rms = 0.2124`
- `headroom_limited_frames = 1066`
- `limiter_frames = 117`

## V13 ve V14 Ile Karsilastirma

- `test_-03`
  - `v13`: `rms 0.4098`, `limiter 795`
  - `v14`: `rms 0.3539`, `limiter 316`
  - `v16`: `rms 0.4022`, `limiter 749`

- `test_-04`
  - `v13`: `rms 0.2730`, `limiter 298`
  - `v14`: `rms 0.2282`, `limiter 146`
  - `v16`: `rms 0.2637`, `limiter 269`

- `test_-05`
  - `v13`: `rms 0.2170`, `limiter 137`
  - `v14`: `rms 0.1933`, `limiter 73`
  - `v16`: `rms 0.2124`, `limiter 117`

## Kisa Yorum

- Kosullu peak-biased blend, `v14`e gore RMS'i anlamli bicimde geri kazandirdi.
- Buna karsilik limiter yukunu da `v14`e gore tekrar arttirdi.
- Yine de sonuc, `v13` ile `v14` arasinda daha dengeli bir ara nokta gibi gorunuyor.
- Bu yon, peak'i tamamen bozmeden RMS recovery arayisi icin faydali bir aday oldu.

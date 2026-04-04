# AGC Test WAV Batch Results V7

## Tarih

- 2026-04-04

## Kullanilan Parametreler

- `mode = AM`
- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- `CFAGC enabled`
- `cf_low_db = 9`
- `cf_high_db = 15`
- `cf_smoothing_ms = 40`
- `target_peak = target_level + (1 - target_level) * 4/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`
- `compressor = passive`

## Output

- `test_wav/agc_output_v7/test_-01_agc_output_v7.wav`
- `test_wav/agc_output_v7/test_-02_agc_output_v7.wav`
- `test_wav/agc_output_v7/test_-03_agc_output_v7.wav`
- `test_wav/agc_output_v7/test_-04_agc_output_v7.wav`
- `test_wav/agc_output_v7/test_-05_agc_output_v7.wav`

## Sonuclar

### test_-01
- `output_peak = 1.0000`
- `output_rms = 0.3156`
- `headroom_limited_frames = 5`
- `limiter_frames = 396`

### test_-02
- `output_peak = 0.8745`
- `output_rms = 0.0926`
- `headroom_limited_frames = 0`
- `limiter_frames = 0`

### test_-03
- `output_peak = 1.0000`
- `output_rms = 0.4690`
- `headroom_limited_frames = 1`
- `limiter_frames = 964`

### test_-04
- `output_peak = 1.0000`
- `output_rms = 0.3896`
- `headroom_limited_frames = 18`
- `limiter_frames = 613`

### test_-05
- `output_peak = 1.0000`
- `output_rms = 0.2779`
- `headroom_limited_frames = 8`
- `limiter_frames = 300`

## Kisa Yorum

- Ilk CFAGC denemesi teorik olarak dogru yone gittiyse de, compressor pasifken limiter yukunu beklenen kadar azaltmadi.
- `headroom_limited_frames` goruluyor, yani peak-aware gain planning devreye giriyor.
- Ancak salt bu mekanizma, sicak dosyalarda limiter ihtiyacini anlamli bicimde dusurmek icin yeterli olmadi.
- Bu sonuc, CFAGC'nin reddi anlamina gelmez; fakat blend fonksiyonu veya peak tarafi agirliginin daha guclu hale getirilmesi gerektigini gosterir.

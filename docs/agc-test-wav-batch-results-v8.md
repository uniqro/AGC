# AGC Test WAV Batch Results V8

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

## Output

- `test_wav/agc_output_v8/test_-01_agc_output_v8.wav`
- `test_wav/agc_output_v8/test_-02_agc_output_v8.wav`
- `test_wav/agc_output_v8/test_-03_agc_output_v8.wav`
- `test_wav/agc_output_v8/test_-04_agc_output_v8.wav`
- `test_wav/agc_output_v8/test_-05_agc_output_v8.wav`

## Sonuclar

### test_-01
- `output_rms = 0.2928`
- `headroom_limited_frames = 882`
- `limiter_frames = 345`

### test_-02
- `output_rms = 0.0926`
- `headroom_limited_frames = 263`
- `limiter_frames = 0`

### test_-03
- `output_rms = 0.4550`
- `headroom_limited_frames = 895`
- `limiter_frames = 940`

### test_-04
- `output_rms = 0.3467`
- `headroom_limited_frames = 993`
- `limiter_frames = 503`

### test_-05
- `output_rms = 0.2574`
- `headroom_limited_frames = 852`
- `limiter_frames = 263`

## V7 Ile Karsilastirma

- CFAGC daha erken devreye girdi; `headroom_limited_frames` belirgin sekilde artti.
- Bazi sicak dosyalarda limiter yukunde azalma goruldu:
  - `test_-03`: `964 -> 940`
  - `test_-04`: `613 -> 503`
  - `test_-05`: `300 -> 263`
- Bunun bedeli, RMS seviyesinin asagi gelmesi oldu:
  - `test_-01`: `0.3156 -> 0.2928`
  - `test_-03`: `0.4690 -> 0.4550`
  - `test_-04`: `0.3896 -> 0.3467`
  - `test_-05`: `0.2779 -> 0.2574`

## Kisa Yorum

- `6.0 / 10.5 dB` araligi, `9 / 15 dB` araligina gore daha anlamli calisti.
- CFAGC artik limiter'a dusen frame'leri daha erken gormeye basladi.
- Ancak kazanc: limiter yukunde orta duzey iyilesme.
- Bedel: RMS hedefinden daha fazla uzaklasma.
- Yani yeni aralik dogru yone gitti, fakat optimum nokta henuz bulunmus degil.

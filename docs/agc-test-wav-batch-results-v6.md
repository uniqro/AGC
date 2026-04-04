# AGC Test WAV Batch Results V6

## Tarih

- 2026-04-04

## Kullanilan Parametreler

- `mode = AM`
- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- `compressor_threshold_dbfs = -2`
- `control sidechain HPF = 300 Hz`
- diger parametreler: `AM` preset

## Uretim Notu

- Bu batch kosusu `tools/agc_batch_wav_reference.py` ile uretilmistir.
- `v6`, detector yoluna eklenen `300 Hz` high-pass weighting ile ilk denemedir.

## Output

- `test_wav/agc_output_v6/test_-01_agc_output_v6.wav`
- `test_wav/agc_output_v6/test_-02_agc_output_v6.wav`
- `test_wav/agc_output_v6/test_-03_agc_output_v6.wav`
- `test_wav/agc_output_v6/test_-04_agc_output_v6.wav`
- `test_wav/agc_output_v6/test_-05_agc_output_v6.wav`

## Sonuclar

### test_-01

- `output_peak = 1.0000`
- `output_rms = 0.3213`
- `compressor_frames = 470`
- `limiter_frames = 359`

### test_-02

- `output_peak = 0.8518`
- `output_rms = 0.0852`
- `compressor_frames = 0`
- `limiter_frames = 0`

### test_-03

- `output_peak = 1.0000`
- `output_rms = 0.5013`
- `compressor_frames = 1089`
- `limiter_frames = 1008`

### test_-04

- `output_peak = 1.0000`
- `output_rms = 0.4152`
- `compressor_frames = 754`
- `limiter_frames = 639`

### test_-05

- `output_peak = 1.0000`
- `output_rms = 0.2812`
- `compressor_frames = 365`
- `limiter_frames = 272`

## V4 Ile Karsilastirma

- `v6` ile ortalama seviye bir miktar daha yukari cikti.
- Ancak limiter yukunde beklenen dusus gorulmedi.
- Aksine, sicak dosyalarda limiter aktivasyonu belirgin sekilde artti:
  - `test_-03`: `17 -> 1008`
  - `test_-04`: `24 -> 639`

## Kisa Yorum

- Ilk `control sidechain HPF` denemesi bu haliyle hedefe hizmet etmedi.
- Detector dusuk frekansi daha az gordugu icin AGC/compressor daha fazla gain verdi.
- Bu da final limiter'a daha fazla yuk bindirdi.
- Yani fikir halen degerli olsa da, yalniz `300 Hz HPF` eklemek tek basina dogru cozum gibi gorunmuyor.

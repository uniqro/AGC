# AGC Test WAV Batch Results V5

## Tarih

- 2026-04-04

## Kullanilan Parametreler

- `mode = AM`
- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- `compressor_threshold_dbfs = -1`
- diger parametreler: `AM` preset

## Uretim Notu

- Bu batch kosusu, ortamda `.exe` calistirma engeli oldugu icin `tools/agc_batch_wav_reference.py` ile uretilmistir.
- Python araci mevcut reference zincirin mantigini aynalamak icin eklendi.

## Output

- `test_wav/agc_output_v5/test_-01_agc_output_v5.wav`
- `test_wav/agc_output_v5/test_-02_agc_output_v5.wav`
- `test_wav/agc_output_v5/test_-03_agc_output_v5.wav`
- `test_wav/agc_output_v5/test_-04_agc_output_v5.wav`

## Sonuclar

### test_-01

- `output_peak = 1.0000`
- `output_rms = 0.3142`
- `compressor_frames = 455`
- `limiter_frames = 345`
- `max_compressor_gain_reduction_db = 11.58`

### test_-02

- `output_peak = 0.8745`
- `output_rms = 0.0926`
- `compressor_frames = 0`
- `limiter_frames = 0`
- `max_compressor_gain_reduction_db = 0.00`

### test_-03

- `output_peak = 1.0000`
- `output_rms = 0.4655`
- `compressor_frames = 1027`
- `limiter_frames = 917`
- `max_compressor_gain_reduction_db = 11.44`

### test_-04

- `output_peak = 1.0000`
- `output_rms = 0.3874`
- `compressor_frames = 687`
- `limiter_frames = 558`
- `max_compressor_gain_reduction_db = 12.78`

## V4 Ile Karsilastirma

- `-1 dBFS` threshold, `v4`e gore compressor'u daha gec devreye soktu.
- Bu da cikis RMS'ini bir miktar yukseltti:
  - `test_-01`: `0.3029 -> 0.3142`
  - `test_-03`: `0.4411 -> 0.4655`
  - `test_-04`: `0.3699 -> 0.3874`
- Ancak bunun bedeli cok belirgin:
  - `test_-01`: `limiter_frames 12 -> 345`
  - `test_-03`: `limiter_frames 17 -> 917`
  - `test_-04`: `limiter_frames 24 -> 558`

## Kisa Yorum

- `compressor_threshold_dbfs = -1`, AM preset icin fazla gec frenleyen bir secim gibi gorunuyor.
- Ortalama seviyeyi yukseltiyor, ama final limiter'a yuklenmeyi ciddi artiriyor.
- Peak bolgelerde limiter'i son carede tutma hedefi acisindan bu sonuc iyi degil.

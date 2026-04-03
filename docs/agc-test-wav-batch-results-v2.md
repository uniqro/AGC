# AGC Test WAV Batch Results V2

## Tarih

- 2026-04-03

## Kullanilan Parametreler

- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- diger parametreler: `reference` default + `high-rms v2` zinciri

## Kullanilan Zincir

- `slow RMS AGC`
- `peak-headroom aware gain cap`
- `soft-knee compressor`
- `final limiter`

## Islenen Dosyalar

Input:

- `test_wav/test_-01_input.wav`
- `test_wav/test_-02_input.wav`
- `test_wav/test_-03_input.wav`
- `test_wav/test_-04_input.wav`

Output:

- `test_wav/agc_output_v2/test_-01_agc_output_v2.wav`
- `test_wav/agc_output_v2/test_-02_agc_output_v2.wav`
- `test_wav/agc_output_v2/test_-03_agc_output_v2.wav`
- `test_wav/agc_output_v2/test_-04_agc_output_v2.wav`

## Sonuclar

### test_-01

- `sample_rate = 48000`
- `samples = 352508`
- `frames = 1836`
- `input_peak = 0.8115`
- `input_rms = 0.0745`
- `output_peak = 0.6594`
- `output_rms = 0.1666`
- `gate_frames = 1335`
- `headroom_limited_frames = 1196`
- `compressor_frames = 771`
- `peak_protector_frames = 0`
- `limiter_frames = 0`
- `max_gain = 6.1426`
- `max_compressor_gain_reduction_db = 10.77`

### test_-02

- `sample_rate = 48000`
- `samples = 254288`
- `frames = 1325`
- `input_peak = 0.2156`
- `input_rms = 0.0210`
- `output_peak = 0.4739`
- `output_rms = 0.0900`
- `gate_frames = 683`
- `headroom_limited_frames = 619`
- `compressor_frames = 134`
- `peak_protector_frames = 0`
- `limiter_frames = 0`
- `max_gain = 6.8624`
- `max_compressor_gain_reduction_db = 5.03`

### test_-03

- `sample_rate = 48000`
- `samples = 367833`
- `frames = 1916`
- `input_peak = 1.0000`
- `input_rms = 0.3398`
- `output_peak = 0.6866`
- `output_rms = 0.2404`
- `gate_frames = 1648`
- `headroom_limited_frames = 1441`
- `compressor_frames = 1184`
- `peak_protector_frames = 0`
- `limiter_frames = 0`
- `max_gain = 4.5044`
- `max_compressor_gain_reduction_db = 11.47`

### test_-04

- `sample_rate = 48000`
- `samples = 352508`
- `frames = 1836`
- `input_peak = 1.0000`
- `input_rms = 0.1321`
- `output_peak = 0.6665`
- `output_rms = 0.1816`
- `gate_frames = 1427`
- `headroom_limited_frames = 1289`
- `compressor_frames = 866`
- `peak_protector_frames = 0`
- `limiter_frames = 0`
- `max_gain = 4.9454`
- `max_compressor_gain_reduction_db = 10.95`

## Onceki Batch Ile Karsilastirma

- `v1` zincirinde yuksek RMS hedefi, ozellikle `test_-03` ve `test_-04` icin peak protector'a cok sik dayaniyordu.
- `v2` zincirinde peak protector tamamen devre disi kaldi; onun yerine kontrol `headroom_limited_frames` ve `compressor_frames` tarafina tasindi.
- `v2` cikis peak'leri bilincli olarak daha dusuk kaldi:
  - `test_-01`: `0.9505 -> 0.6594`
  - `test_-03`: `0.9505 -> 0.6866`
  - `test_-04`: `0.9505 -> 0.6665`
- `v2` cikis RMS'i de daha muhafazakar hale geldi:
  - `test_-01`: `0.2397 -> 0.1666`
  - `test_-03`: `0.3619 -> 0.2404`
  - `test_-04`: `0.2772 -> 0.1816`

## Kisa Yorum

- `v2` zinciri peak bolgesindeki lineerligi korumak icin beklenen yone gitti.
- En sert fark, `frame-level peak protector` kullaniminin sifira inmesi oldu.
- Bunun bedeli, `%80 RMS` hedefinin pratikte daha az agresif uygulanmasi oldu; yani bazi sicak dosyalarda hedefe koruma ve compressor tarafindan erken fren konuyor.
- Bu sonuc, daha once konustugumuz tasarim kararini dogruluyor: `%80 FS RMS` hedefi korunuyor, ama koruma zinciri bunu "mumkun olan en temiz sekilde" yorumluyor.

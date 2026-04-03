# AGC Test WAV Batch Results V3

## Tarih

- 2026-04-03

## Kullanilan Parametreler

- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold_percent = 5`
- diger parametreler: `reference` default + `high-rms v3` zinciri

## Kullanilan Zincir

- `slow RMS AGC`
- `peak headroom cap = disabled`
- `soft-knee compressor`
- `frame-level peak protector = disabled`
- `final limiter`

## Islenen Dosyalar

Input:

- `test_wav/test_-01_input.wav`
- `test_wav/test_-02_input.wav`
- `test_wav/test_-03_input.wav`
- `test_wav/test_-04_input.wav`

Output:

- `test_wav/agc_output_v3/test_-01_agc_output_v3.wav`
- `test_wav/agc_output_v3/test_-02_agc_output_v3.wav`
- `test_wav/agc_output_v3/test_-03_agc_output_v3.wav`
- `test_wav/agc_output_v3/test_-04_agc_output_v3.wav`

## Sonuclar

### test_-01

- `sample_rate = 48000`
- `samples = 352508`
- `frames = 1836`
- `input_peak = 0.8115`
- `input_rms = 0.0745`
- `output_peak = 0.9950`
- `output_rms = 0.3029`
- `gate_frames = 1335`
- `headroom_limited_frames = 0`
- `compressor_frames = 523`
- `peak_protector_frames = 0`
- `limiter_frames = 12`
- `max_gain = 6.8450`
- `max_compressor_gain_reduction_db = 12.41`

### test_-02

- `sample_rate = 48000`
- `samples = 254288`
- `frames = 1325`
- `input_peak = 0.2156`
- `input_rms = 0.0210`
- `output_peak = 0.8493`
- `output_rms = 0.0926`
- `gate_frames = 683`
- `headroom_limited_frames = 0`
- `compressor_frames = 3`
- `peak_protector_frames = 0`
- `limiter_frames = 0`
- `max_gain = 6.8624`
- `max_compressor_gain_reduction_db = 0.25`

### test_-03

- `sample_rate = 48000`
- `samples = 367833`
- `frames = 1916`
- `input_peak = 1.0000`
- `input_rms = 0.3398`
- `output_peak = 0.9950`
- `output_rms = 0.4411`
- `gate_frames = 1648`
- `headroom_limited_frames = 0`
- `compressor_frames = 1080`
- `peak_protector_frames = 0`
- `limiter_frames = 17`
- `max_gain = 5.4583`
- `max_compressor_gain_reduction_db = 12.27`

### test_-04

- `sample_rate = 48000`
- `samples = 352508`
- `frames = 1836`
- `input_peak = 1.0000`
- `input_rms = 0.1321`
- `output_peak = 0.9950`
- `output_rms = 0.3699`
- `gate_frames = 1427`
- `headroom_limited_frames = 0`
- `compressor_frames = 759`
- `peak_protector_frames = 0`
- `limiter_frames = 24`
- `max_gain = 6.5232`
- `max_compressor_gain_reduction_db = 13.62`

## V2 Ile Karsilastirma

- `v3`, `v2`'ye gore cikis ortalamasini daha yukariya tasidi:
  - `test_-01`: `0.1666 -> 0.3029`
  - `test_-03`: `0.2404 -> 0.4411`
  - `test_-04`: `0.1816 -> 0.3699`
- `v3`'te `headroom_limited_frames = 0`; yani AGC artik hedefe gitmeden once erken frenlenmiyor.
- `v3`'te peak'ler `0.995` civarina kadar cikabiliyor; bu, kullanici istegine daha yakin.
- Bunun bedeli, artik az sayida da olsa `limiter_frames > 0`.

## Kisa Yorum

- `v3`, onceki `v2` zincirine gore "ortalamayi daha yukari tasiyip peak'i sona birak" hedefine daha yakin.
- En belirgin kazanc, RMS seviyesinin yukselmesi ve peak protector kaynakli frame-toplu sekil bozmanin tamamen kalkmasi oldu.
- Risk tarafi su: limiter artik az sayida da olsa devreye giriyor. Bu, lineerlik sorununu `peak protector`dan alip `final limiter` cephesine tasiyor.
- Yani `v3` dogru yone gitti, ama "hic distorsiyon olmasin" hedefi icin bir sonraki mantikli iyilestirme hard limiter yerine daha yumusak bir final koruma davranisi olur.

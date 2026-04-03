# AGC Test WAV Batch Results

## Tarih

- 2026-04-02

## Kullanilan Parametreler

- `max_gain_db = 18`
- `target_rms_percent = 80`
- `gate_threshold = 5`
- diger parametreler: UI default / fixed baseline default

## Islenen Dosyalar

Input:

- `test_wav/test_-01_input.wav`
- `test_wav/test_-02_input.wav`
- `test_wav/test_-03_input.wav`
- `test_wav/test_-04_input.wav`

Output:

- `test_wav/agc_output/test_-01_agc_output.wav`
- `test_wav/agc_output/test_-02_agc_output.wav`
- `test_wav/agc_output/test_-03_agc_output.wav`
- `test_wav/agc_output/test_-04_agc_output.wav`

## Sonuclar

### test_-01

- `sample_rate = 48000`
- `samples = 352508`
- `frames = 1836`
- `input_peak = 0.8115`
- `input_rms = 0.0745`
- `output_peak = 0.9505`
- `output_rms = 0.2397`
- `gate_frames = 1335`
- `peak_protector_frames = 393`
- `limiter_frames = 0`
- `max_gain = 6.8729`

### test_-02

- `sample_rate = 48000`
- `samples = 254288`
- `frames = 1325`
- `input_peak = 0.2156`
- `input_rms = 0.0210`
- `output_peak = 0.7910`
- `output_rms = 0.0858`
- `gate_frames = 683`
- `peak_protector_frames = 0`
- `limiter_frames = 0`
- `max_gain = 6.7439`

### test_-03

- `sample_rate = 48000`
- `samples = 367833`
- `frames = 1916`
- `input_peak = 1.0000`
- `input_rms = 0.3398`
- `output_peak = 0.9505`
- `output_rms = 0.3619`
- `gate_frames = 1648`
- `peak_protector_frames = 993`
- `limiter_frames = 0`
- `max_gain = 5.2066`

### test_-04

- `sample_rate = 48000`
- `samples = 352508`
- `frames = 1836`
- `input_peak = 1.0000`
- `input_rms = 0.1321`
- `output_peak = 0.9505`
- `output_rms = 0.2772`
- `gate_frames = 1427`
- `peak_protector_frames = 635`
- `limiter_frames = 0`
- `max_gain = 6.5160`

## Kisa Yorum

- `target RMS %80` bu test seti icin agresif gorunuyor.
- Limiter hic devreye girmedi; bunun anlami peak protector onceden devreye girip tasmayi engellemis.
- `test_-03` ve `test_-04` dosyalarinda peak protector aktivitesi yuksek.
- `test_-02` en temiz davranan dosya gibi gorunuyor; peak protector'a hic ihtiyac duymamis.
- Burst/patlama hissi varsa bunun bir kismi yuksek hedef RMS nedeniyle peak protector'a sik dayanilmasindan geliyor olabilir.

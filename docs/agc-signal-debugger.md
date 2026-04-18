# AGC Signal Debugger

## Amac

Bu arac, AGC'yi sadece dinlemek yerine sinyal ve karar zinciri boyunca incelemek icin tasarlandi.

Hedef:

- giris ve cikis sinyalini ayni pencerede gormek
- secili frame icin olusan degerleri okumak
- `gate`, `CFAGC`, `limiter` gibi kararlarin ne zaman devreye girdigini anlamak
- yazilim debug eder gibi sinyali debug etmek

## Bilesenler

- Export araci:
  - `tools/agc_debug_export.py`
- UI:
  - `ui/agc-debugger.html`
  - `ui/agc-debugger.css`
  - `ui/agc-debugger.js`
- WAV-first UI:
  - `ui/agc-wav-debugger.html`
  - `ui/agc-wav-debugger.js`

## Onerilen Yol

Artik en pratik yol:

1. `ui/agc-wav-debugger.html` dosyasini ac.
2. Input WAV dosyasini sec.
3. `mode`, `target RMS`, `max gain`, `gate threshold` degerlerini ayarla.
4. `Analyze` ile ayni tarayici icinde frame-frame debug trace uret.
5. `Metric timeline` seciminden metric'i degistir.
6. `Selected Frame` altindaki `Prev/Next Frame` ve `Prev/Next Limiter` ile gezin.
7. `Prev/Next Metric Peak` ile secili metric'in lokal tepe noktalarina zipla.
8. Whole signal veya decision timeline uzerine tiklayarak dogrudan frame sec.
9. `Filters` ile `gate/headroom/limiter` gorunurlugunu ayarla.
10. `Issue-only mode` aciksa `Prev/Next Frame` sadece secili filtrelerle eslesen frame'lerde gezinir.
11. `Export Frame JSON` ile secili frame'i disa aktar.
12. `Download Output WAV` ile islenmis cikisi indir.

Bu yol ayrica JSON export adimi gerektirmez.

## Export Komutu

Ornek:

```powershell
python .\tools\agc_debug_export.py `
  .\test_wav\test_-01_input.wav `
  .\artifacts\debug\test_-01_v11_debug.json `
  --output-wav .\artifacts\debug\test_-01_v11_output.wav `
  --target-rms-percent 80 `
  --max-gain-db 18 `
  --gate-threshold-percent 5 `
  --mode am
```

## JSON UI Kullanimi

1. `ui/agc-debugger.html` dosyasini tarayicida ac.
2. `Load Debug JSON` ile export edilen JSON dosyasini sec.
3. `Signal window` ile butun sinyal penceresini degistir.
4. `Metric timeline` ile hangi karar metrigini izlemek istedigini sec.
5. `Frame` slider'i ile tek tek frame incele.

## Ne Gosterir

- Whole Signal:
  - butun giris ve cikis waveform'u
  - secili frame highlight
  - decision timeline'in altinda ayrica yer alir
  - secilen pencere degil, WAV dosyasinin tamami gosterilir
  - waveform uzerine tiklayarak frame secimi
- Decision Timeline:
  - secilen metrik
  - secili frame icin `Current Crest` ve `Smoothed Crest` hizli okumalari
  - `gate`, `headroom_limited`, `limiter_active`, `attack`, `release` marker'lari
  - metric aciklamasi
  - chip tabanli legend
  - sari golgelendirme ile `headroom_limited` cluster'lari
  - limiter cluster bolgeleri icin kirmizi golgelendirme
  - secili frame'in metric degeri icin baloncuk etiketi
  - metric uzerine tiklayarak frame secimi
  - filtrelerle event katmanlarini ac/kapa yapabilme
- Selected Frame:
  - frame bazinda input ve output waveform
  - altinda frame gezinme butonlari
  - output uzerinde `target_peak` ve `limiter_threshold` referans cizgileri
- Decision Trace:
  - `desired_gain` formulu
  - `applied_gain` smoothing formulu
  - secili frame icin:
    - onceki gain
    - secilen `attack/release` modu
    - kullanilan `alpha`
    - formülun sayisal acilimi
- Frame Metrics:
  - sticky ve daha kompakt inspector icinde gorunur
  - `envelope`
  - `smoothed_rms`
  - `crest_factor_db`
  - `smoothed_crest_factor_db`
  - `CF Update`
  - `desired_gain`
  - `applied_gain`
  - `cf_blend_weight`
  - `limiter_active`
- Decision Trace:
  - secili frame icin metin tabanli karar ozeti
- Stage Peaks:
  - sticky inspector icinde `Frame Metrics` ile birlikte gorunur
  - input
  - gain sonrasi
  - compressor sonrasi
  - peak protector sonrasi
  - output

## Not

- Bu arac offline analiz icindir.
- Canli UI'nin yerine gecmez; daha cok karar mantigini incelemek icin tasarlandi.
- JSON buyuk olabilir; tum sinyal ve tum frame metrikleri bilerek tutulur.
- `ui/agc-wav-debugger.html`, JSON uretmeden ayni incelemeyi dogrudan WAV dosyasi uzerinden yapar.
- Event marker anlamlari:
  - yesil = `gate_open`
  - sari = `headroom_limited`
  - kirmizi = `limiter_active`
  - mavi = `attack`
  - mor = `release`

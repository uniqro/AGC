# AGC Web UI Test

Bu not, tarayici tabanli AGC test ortamini aciklar.

## Dosyalar

- `ui/agc-live-ui.html`
- `ui/agc-live-ui.css`
- `ui/agc-live-ui.js`

## Amac

- C diline bagli kalmadan hizli canli test yapmak
- AGC parametrelerini elle degistirmek
- Input/output seviye degisimlerini gormek
- Waveform akislarini izlemek

## Saglanan Ozellikler

- mikrofon girisini canli alma
- kulakliga canli monitor verme
- runtime `AGC on/off` secimi
- runtime `Record` dugmesi ile input ve output'u ayni anda WAV alma
- target RMS, max gain, gate, attack, release, envelope, RMS ve limiter ayarlari
- opsiyonel compressor denemesi
- input/output peak ve RMS meter
- input/output waveform canvas gorunumu
- waveform width/height ayari
- secilebilir tek waveform penceresi:
  - `10 ms`
  - `100 ms`
  - `500 ms`
  - `1 s`
  - `2 s`
- signal flow degerleri:
  - envelope
  - smoothed RMS
  - desired gain
  - applied gain
  - peak protector durumu
  - limiter durumu

## Calistirma

Basit yol:

- `ui/agc-live-ui.html` dosyasini bir tarayicida ac

Oneri:

- Chrome veya Edge kullan
- kulaklik ile test et
- mikrofon iznine izin ver
- patlamalari daha rahat incelemek icin waveform width/height slider'larini buyut
- input ve output waveform artik alt alta gosterilir
- zaman penceresi combo box ile secilir
- boyut slider'lari secili pencereye uygulanan gorunumu buyutur

## Onemli Not

Bu UI:

- hizli deney ve tuning yardimcisidir
- mevcut DSP baseline ile ayni blok mantigini tasir
- ancak fixed-point C kodunun bit-exact kopyasi degildir

Dolayisiyla:

- trend ve davranis karsilastirmasi icin cok faydalidir
- ama DSP final dogrulamasi yerine gecmez

## AGC On / Off

UI icindeki `AGC enabled` secimi ile:

- acikken:
  - gate + AGC gain + peak protector + limiter akisi izlenir
- kapaliyken:
  - AGC gain yolu bypass edilir
  - peak protector ve limiter yine aktif kalir

Bu sayede:

- ayni input uzerinde AGC'nin faydasi daha net duyulabilir
- waveform ve meter farki dogrudan izlenebilir

## Record

UI icindeki `Record` dugmesi:

- waveform panelinin ustunde, kirmizi kayit butonu olarak durur
- audio calisiyorken input ve output bloklarini biriktirir
- tekrar basildiginda kaydi durdurur
- iki ayri WAV indirir:
  - `agc_input_<timestamp>.wav`
  - `agc_output_<timestamp>.wav`

Not:

- Kayit, buton aktif oldugu surece devam eder
- Audio durdurulursa kayit varsa otomatik kapanir ve indirme tetiklenir

## Bilinen Sinirlar

- Tarayicida output device secimi her zaman desteklenmeyebilir
- Tarayici ses yolu ve `ScriptProcessorNode` nedeniyle latency, DSP hedefinden farkli olur
- Browser audio stack kendi icinde ek buffering uygulayabilir

## Stabilite Notu

UI tarafinda dusuk riskli stabilite icin:

- `ScriptProcessorNode` blok boyu buyutuldu
- waveform/meter cizimi her animation frame'de degil, daha seyrek guncellenir

Bunun etkisi:

- cizim kaynakli audio glitch riski azalir
- latency bir miktar artar

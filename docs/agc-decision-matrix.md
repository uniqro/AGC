# AGC Decision Matrix

Bu dosya, proje icin AGC seceneklerini mevcut gereksinimlere gore karsilastirir ve secim mantigini kaydeder.

## 1. Proje Profili Ozeti

- Sinyal: AM Radio konusmasi
- Kanal: mono
- Calisma: gercek zamanli
- Hedef platform: TMS320VC5416 DSP
- Tasarim: Fs-genel, ilk dogrulama 8/16/64 kHz
- Uretim yonu: fixed-point uyumlu
- Oncelik: konusma netligi once, gurultu kontrolu zorunlu
- Sessizlik davranisi: gain belirgin artmamali
- Koruma zinciri: AGC + basit gate + emniyet limiter

## 2. Secenek Karsilastirmasi

| Secenek | Uygunluk | Neden | Risk |
|---|---|---|---|
| Sabit gain | Dusuk | Degisen seviye ve SNR kosullarina uyum saglamaz | Konusma/gurultu dengesi bozulur |
| Peak tabanli basit AGC | Orta | Hizli tepki verir, ama algisal olarak oynak olabilir | Pumping ve gereksiz gain degisimi |
| Envelope/enerji tabanli tek bant AGC | Yuksek | Konusma takibi, sadelik ve DSP maliyeti arasinda dengeli | Kisa pencere seciminde oynaklik |
| RMS agirlikli AGC | Orta-Yuksek | Daha dogal seviye takibi saglayabilir | Hesap ve tuning maliyeti artabilir |
| Multi-band AGC | Dusuk | Bu asama icin gereksiz karmasik | DSP maliyeti ve debug zorlugu |
| Uyarlamali gurultu tabanli AGC/gate | Orta | Degisken SNR icin ileride faydali olabilir | Ilk surum icin riskli ve daha karmasik |

## 3. Proje Icin Secilen Yon

### Secilen cekirdek algoritma ailesi
- Tek bant, enerji/envelope tabanli AGC
- Hizli attack, yavas release
- Kisa enerji penceresi
- Basit enerji esikli gate
- Emniyet limiter

### Neden secildi
- Gercek zamanli DSP hedefi icin hesap acisindan makul.
- AM konusmasi icin konusma envelope'unu takip etmeye uygun.
- Sessizlikte gain sismesini gate ile sinirlayabilir.
- Baslangic seviyesinde anlasilir ve kontrollu tuning yapmaya elverisli.
- Daha sonra uyarlamali gate veya farkli level detector yapilarina evrilebilir.

## 4. Elenen veya Ertelenen Yonler

### Sabit gain
- Elenme nedeni: giris seviyesi ve gurultu kosullari degisken oldugu icin yetersiz.

### Multi-band AGC
- Elenme nedeni: ilk surum icin yuksek riskli ve gereksiz karmasik.

### Uyarlamali gurultu tabani
- Erteleme nedeni: degisken SNR icin ilgi var, ancak ilk surumde basit gate ile baslamak daha dusuk riskli.
- Durum: V2 adayi, tetik kosulu henuz acik soru.

## 5. Karari Etkileyen Ana Sorular

- Konusma netligi ne kadar agresif gain degisimine izin veriyor?
- Basit gate degisken SNR kosullarinda yeterli olacak mi?
- Kisa enerji penceresi fixed-point ortamda ne kadar kararli olacak?
- Limiter ne kadar sik devreye girerse hala emniyet blogu sayilacak?

## 6. Sonuc

Bu proje icin en dusuk riskli karar, tek bant enerji/envelope tabanli AGC + basit gate + emniyet limiter yapisidir. Bu secim, performans ve kalite arasinda muhafazakar ama urune donusebilir bir baslangic noktasi sunar.

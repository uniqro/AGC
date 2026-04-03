# AGC Multi-Mode Target Level Decision

## Tarih

- 2026-04-03

## Baglam

Proje baslangicinda AGC daha cok genel audio leveling bakisiyla ele alindi. Daha sonra iki ek gereksinim netlesti:

- AGC hem `AM` haberlesmede hem de `DIGITAL` haberlesmede kullanilmak isteniyor.
- Kullanici, peak bolgelerde zorunlu olmadikca limiter vurulmasini istemiyor; mumkunse bu bolgelerde daha az gain uygulanmasi tercih ediliyor.

Bu nedenle hedefin ve mod farklarinin tek bir cati altinda netlestirilmesi gerekiyor.

## Kesinlesen Kararlar

### 1. Ortak cekirdek korunacak

Algoritma iki ayri AGC'ye bolunmeyecek.

Tek cekirdek kalacak:

- seviye detector
- gate / speech-aware karar
- slow AGC
- istege bagli compressor
- final protection

`AM` ve `DIGITAL` farklari ayri algoritma ile degil, konfigurasyon profili ile yonetilecek.

### 2. Target level iki modda da ayni kavram olacak

`target level`, hem `AM` hem `DIGITAL` modda su anlama gelecek:

- `konusma aktif frame'lerde hedef ortalama seviye`

Bu hedefin olcumu:

- `RMS + gate ile konusma aktif frame'leri`

Yani sessizlik, gap ve yalniz gurultu bolgeleri hedef seviyeyi tanimlayan ana metrikten dislanacak.

### 3. Mode farki hedefin anlaminda degil, uygulanisinda olacak

`AM` ve `DIGITAL` arasindaki fark:

- compressor davranisi
- final protection yumusakligi
- attack / release
- target weighting

gibi alanlarda ortaya cikacak.

Ama `target level` iki modda da ayni kavrami koruyacak.

Ilk resmi mod profili olarak `AM` preset'i ayri bir dokumanda sabitlenmistir:

- `docs/agc-am-preset.md`

### 4. Peak bolgelerde tercih edilen strateji

Peak bolgelerde istenen davranis:

- limiter neredeyse son care olmali
- gain daha erken ve daha yumusak sekilde frenlenebilmeli
- hafif compressor yardimci rol oynayabilmeli

Bu nedenle tercih edilen yon:

- `peak-aware gain planning + compressor`
- `final limiter = son emniyet`

### 5. Kullanici arayuzu mantigi

Kullanici tarafi:

- once `mode` secimi
- sonra temel ayarlar
- ileri parametreler opsiyonel

Bu, hem sade kullanim hem de ileride ayrintili tuning ihtiyacini birlikte karsilar.

## 1. Soru Icin Yapilan Analiz ve Secilen Yorum

Degerlendirilen secenekler:

- `A. RMS`
- `B. Envelope ortalamasi`
- `C. Kisa vadeli loudness benzeri olcu`
- `D. RMS + gate ile konusma aktif frame'leri`

Secilen:

- `D`

Neden:

- `RMS`in kararliligini koruyor
- sessizlik/gap bolgelerinin hedefi bozmasini engelliyor
- "konusma aktifken hedef ortalama seviye" tanimina en iyi uyan secenek bu
- hem `AM` hem `DIGITAL` icin ortak bir hedef dili sagliyor
- fixed-point DSP uygulamasina uygun

## AM ve DIGITAL Modlar Arasinda Beklenen Farklar

### Ortak kalanlar

- hedef seviye kavrami
- gate tabanli aktif frame ayrimi
- slow AGC cekirdegi
- final protection blogu olmasi

### Mode'a bagli hale gelecekler

- compressor default acik/kapali durumu
- compressor threshold / ratio / knee
- attack / release davranisi
- target weighting
- final protection esigi ve yumusakligi

## Yan Etkiler

Bu kararlarin dogal sonuclari:

- `%80` hedefi her durumda birebir garanti edilmeyebilir; peak lineerligini korumak icin bazi bolgelerde daha dusuk kalinabilir.
- `AM` ve `DIGITAL` icin ayri preset/test matrisi gerekecek.
- "tek ideal ayar" yerine "mode'a gore iyi ayar" yaklasimi gerekir.
- Gate kararinin kalitesi, hedef seviyenin dogru yorumlanmasinda daha kritik hale gelir.

## Sonuc

Projede bundan sonra `target level` su sekilde yorumlanacak:

- `konusma aktif frame'lerde hedef RMS ortalamasi`

Ve bu yorum:

- `AM` modunda da
- `DIGITAL` modunda da

ayni kalacak.

Mode farklari, bu hedefe nasil ulasildiginda ortaya cikacak.

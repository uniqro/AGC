# AGC Technical Spec Summary

Bu dosya, projede su ana kadar netlesen AGC teknik kararlarini tek bir implementasyon-odakli ozet halinde toplar.

## 1. Amac

- AM Radio konusmasi icin gercek zamanli AGC yapisini tanimlamak
- Reference ve hedef DSP implementasyonu icin ortak teknik zemin saglamak
- Karar gecmisi yerine uygulanabilir mevcut resmi sunmak

## 2. Hedef Profil

- Sinyal tipi: AM Radio konusmasi
- Kanal: mono
- Hedef platform: TMS320VC5416 DSP
- Calisma modu: gercek zamanli
- Sayisal yon: fixed-point uyumlu
- Giris veri tipi: int16
- Hedef ic temsil: Q15

## 3. Secilen Algoritma Cekirdegi

- Tek bant AGC
- Enerji/envelope tabanli seviye takibi
- Basit enerji esikli gate
- Emniyet limiter
- Hizli attack, yavas release
- Sessizlikte gain sisirmeyen davranis

## 4. AGC Siniri

`AGC -> limiter -> cikis`

Not:
- Filtreleme artik AGC blogunun parcasi degil.
- Gerekirse AGC oncesinde bagimsiz bir on isleme blogu olarak dusunulecek.
- Bu ayrim, AGC tuning'ini filtre tasarimindan bagimsiz hale getirir.

Bagimsiz filtreleme icin mevcut yon:
- Gercekleme: FIR
- Yaklasim: orta derece HP + orta derece LP
- HP baslangici: `250 Hz`
- LP baslangici: `0.4125 * Fs`

## 5. Zaman ve Frame Yapisi

- Sabit frame suresi: `4 ms`
- Frame kurali: `frame_samples = Fs * 0.004`
- Ornek karsiliklari:
  - `8 kHz -> 32`
  - `16 kHz -> 64`
  - `64 kHz -> 256`
- Diger Fs degerleri ayni kuralla desteklenecek

## 6. Baslangic AGC Parametre Yonleri

- Attack: `5-20 ms`
- Release: `200-800 ms`
- Envelope/enerji penceresi: `2-10 ms`
- Gate hold: `50-150 ms`
- Hedef seviye: kullanici ayarli `%FS`
- Ornek hedef: `%80 FS`
- Parametre degisimi: yalnizca init sirasinda

## 7. Fixed-Point ve Koruma Kararlari

- Ic temsil: `Q15`
- Ara hesap: baslangicta `32-bit`
- 40-bit gecis yonu: overflow veya ilgili hata logu gorulurse
- Saturation: gain ve cikista uygulanacak
- Ara adim olcekleme: peak tabanli, sinyal seviyesine gore uyarlamali

## 8. Kabul ve Davranis Yonleri

- Konusma netligi birincil oncelik
- Gurultu konusma kadar yukselmemeli
- Sessizlikte gain belirgin sekilde artmamali
- Clipping: kisa sureli ve gorulur seviyede belli olcude kabul edilebilir
- Limiter: konusma patlamalarinda kisa sureli kumelenme seklinde devreye girmesi normal kabul edilir
- Basari: dinleme testi agirlikli, temel guvenlik loglari ile destekli

## 9. Implementasyon Sirasinda Dogrulanacak Ayrintilar

- FIR tap sayilari ve pencere secimi
- Clipping icin net sayisal oran/esik
- Limiter icin net sayisal oran/esik
- Overflow logundan 40-bit'e gecis uygulama ayrintisi
- Peak tabanli olcekleme katsayi/esik kurallari
- Farkli Fs degerlerinde performans etkisi
- Bagimsiz filtreleme blogunun kullanilip kullanilmayacagi

## 10. Sonuc

Bu teknik ozet, reference implementasyon ve sonraki DSP implementasyonu icin yeterli karar zeminini saglar. Kalan maddeler, temel mimariyi degistiren kararlar degil; implementasyon ve tuning sirasinda dogrulanacak ayrintilardir.

## 11. Dondurulen Reference Baz

DSP portu icin sabitlenen cekirdek:

- RMS AGC
- gate
- peak protector
- limiter

Bu bazda noise-aware gain benzeri ek karar katmanlari yoktur. DSP portu once bu sade davranis uzerinden ilerletilecektir.

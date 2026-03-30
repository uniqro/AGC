# AGC Reference Algorithm Notes

Bu dosya, ilk reference algoritma yaklasimini tanimlar. Amac, hedef urun kodunu yazmak degil; davranisi anlasilir, olculebilir ve sonra hedef ortama tasinabilir bir referans tanimlamaktir.

## 1. Reference Amaci

- AGC davranisini gozle gorulebilir hale getirmek
- Parametre etkilerini ayirt edebilmek
- Sessizlik, konusma ve gurultu gecislerini izlemek
- Hedef DSP implementasyonu icin tasinabilir bir davranis tanimi cikarmak

## 2. Onerilen Blok Yapi

1. Giris frame'ini al
2. Kisa sureli enerji veya envelope olc
3. Gate kararini olustur
4. Hedef seviye ile mevcut seviye farkindan istenen gain'i hesapla
5. Attack/release kurallariyla gain'i yumusat
6. Max gain ve diger sinirlari uygula
7. Emniyet limiter ile patlamalari sinirla
8. Cikisi ve ara metrikleri kaydet

## 3. Reference Davranis Kurallari

- Konusma baslangicinda gain hizli toparlanmali.
- Konusma bitisinde gain yavas dusmeli.
- Sessizlikte veya gurultu agirlikli bolumlerde gain belirgin sekilde sisirilmemeli.
- Limiter ana seviye kontrol mekanigi olmamali; esas davranis AGC + gate tarafinda kalmali.
- Hedef seviye kullanici ayarli FS yuzdesi olarak ele alinmali.

## 4. Baslangic Parametre Yonleri

- Frame: 5-10 ms
- Attack araligi: 5-20 ms
- Release araligi: 200-800 ms
- Enerji/envelope penceresi: 2-10 ms
- Gate hold: 50-150 ms
- Max gain: ayarlanabilir, ama default siki
- Gate kontrolu: kullaniciya basit hassasiyet/esik olarak acilacak

Not: Bunlar final tuning degerleri degil, koruyucu baslangic araliklaridir.

## 5. Loglanacak Ara Metrikler

- Giris seviye olcumu
- Gate durumu
- Ham istenen gain
- Yumusatilmis gain
- Max gain clamp durumu
- Limiter aktivasyonu
- Cikis envelope/enerji olcumu

## 6. Test Senaryolari

- Sessizlikten konusmaya gecis
- Konusmadan sessizlige donus
- Degisken SNR konusma
- Ani patlama iceren konusma
- Uzun sessizlikte gain birikimi kontrolu
- 8/16/64 kHz davranis esdegerligi kontrolu

## 7. Bilincli Olarak Acik Birakilanlar

- Nadir clipping'in sayisal tanimi
- Limiter'in kabul edilebilir aktivasyon sikligi
- Gate histerezisinin ayrica disari acilip acilmayacagi
- Uyarlamali gate'e gecis kosulu
- Gercek DSP zincirindeki tam entegrasyon noktasi

## 8. Sonraki Adim

Bu reference notunun ardindan:
- karar matrisi ile secim gerekcesi esitlenmeli
- reference davranis daha teknik kabul kriterlerine baglanmali
- daha sonra hedef DSP ortaminda fixed-point implementasyon gereksinimleri ayri dokumante edilmeli

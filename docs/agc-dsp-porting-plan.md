# AGC DSP Porting Plan

Bu dosya, reference C implementasyonundan TMS320VC5416 odakli DSP implementasyonuna gecis planini tutar.

## 1. Amaç

- Floating-point reference davranisini fixed-point DSP tasarimina tasimak
- Q15 temsil ve 32-bit ara hesaplarla baslamak
- Davranis sapmalarini kontrollu sekilde izlemek

## 2. Port Hedefi

- Hedef platform: `TMS320VC5416`
- Giris: `int16`
- Ic temsil: `Q15`
- Baslangic ara hesap: `32-bit`
- Gerekirse: `40-bit accumulator` gecisi

## 3. Portlanacak Cekirdek Bloklar

1. Detector
   - `envelope`
   - `smoothed_rms`
2. Gate
   - acma/esik
   - hold
3. Gain control
   - desired gain
   - attack/release smoothing
4. Peak protector
5. Limiter

## 4. Kodlama Stratejisi

### A. Parametre Temsili

- Kullanici parametreleri float degil, sabitlenmis katsayi veya Q-format sabitler olarak tutulacak
- `attack_ms`, `release_ms`, `envelope_window_ms`, `rms_window_ms` runtime hesap yerine once katsayi olarak hazirlanabilir

### B. Detector Portu

- `envelope` ve `smoothed_rms` state'leri Q-format ile tutulacak
- `sqrt` kullanan RMS olcumu icin:
  - ya integer karekok
  - ya da LUT/approximation
  - ya da test kolayligi icin ilk portta daha pahali ama dogru bir gecici yol

### C. Gain Temsili

- Gain lineer tutulacak
- Q15 veya daha genis bir gain formati secilecek
- `max_gain = 4.0` oldugu icin saf Q15 yetmez; gain state icin ayri sabit nokta olcegi gerekecek

### D. Saturation Kurali

- Gain carpiminda ara hesap saturate edilmeden once yeterli headroom korunacak
- Cikis sinyali `int16/Q15` sinirina donerken saturate edilecek
- Peak protector ve limiter ayrimi korunacak

## 5. Onerilen Uygulama Sirasi

1. `agc_fixed_config`
   - DSP sabitleri ve Q-format secimleri
2. `agc_fixed_detector`
   - envelope ve RMS state
3. `agc_fixed_gate`
4. `agc_fixed_gain`
5. `agc_fixed_peak_protector`
6. `agc_fixed_limiter`
7. `agc_fixed_pipeline`
8. reference ile karsilastirma testleri

## 6. Referans ile Karsilastirma Plani

- Her blok portlandiginda ayni WAV test seti ile karsilastirma yap
- Asagidaki metrikleri izle:
  - gate frame sayisi
  - peak protector frame sayisi
  - limiter frame sayisi
  - cikis peak
  - segment RMS karsilastirmasi

## 7. En Riskli Noktalar

- RMS hesaplama maliyeti
- Gain formatinin dogru secilmemesi
- `max_gain` ve peak protector oraninin fixed-pointte hassasiyet kaybi
- Attack/release katsayilarinin float referanstan sapmasi

## 8. Ilk Somut Kod Hedefi

Ilk DSP odakli adim olarak sunlar yazilabilir:

- `fixed/agc_fixed_types.h`
- `fixed/agc_fixed_config.h`
- `fixed/agc_fixed_config.c`
- `fixed/agc_fixed_math.h`

Bu ilk adim, tum algoritmayi degil; veri temsili ve katsayi olcegini sabitlemeyi hedeflemeli.

## 9. Mevcut Durum

Su anda sabitlenen baseline:

- `fixed/agc_fixed_detector`
- `fixed/agc_fixed_gate`
- `fixed/agc_fixed_gain`
- `fixed/agc_fixed_peak_protector`
- `fixed/agc_fixed_limiter`
- `fixed/agc_fixed_pipeline`
- `fixed/agc_fixed_compare`
- `fixed/agc_fixed_runtime`

Ilk compare turu sonrasi fixed detector enerji olcegi duzeltildi ve fixed-reference uyumu anlamli bicimde yaklasti.

## 10. Sonraki Düşük Riskli DSP Adımları

1. Runtime katmanini TMS320 cagri modeline gore sabitlemek
2. Metrics ve debug akislarini kosullu derleme ile ayirmak
3. `size_t` ve genel masaustu varsayimlarini hedefe uygun tiplere indirmek
4. 32-bit / 40-bit accumulator gecis noktalarini acik isaretlemek

Bu maddelerin ilki baslatildi:

- `fixed/agc_fixed_build.h`
- `AGC_FIXED_ENABLE_METRICS`

Bir sonraki madde de baslatildi:

- fixed cekirdekte frame arayuzu `agc_frame_count_t`
- ilk tip secimi `uint16_t`

40-bit gecis acisindan ilk aday noktalar da isaretlendi:

- detector enerji toplami
- gain smoothing carpimi
- genis gain buffer yolu

Bu noktalar artik host tarafinda genis tip ile calisiyor:

- `agc_accum40_t` (`int64_t` stand-in)

Hedefe ozel entegrasyon hook'lari denendi ancak su asamada geri alindi.

Gerekce:

- Hedef arac-zinciri baglantisi henuz somut degil
- Davranisi degistirmeyen ama ek karmaşıklık getiren bir katman olarak erken bulundu

Mevcut fixed-point durum artik baseline olarak sabitlendi:

- bkz. `docs/agc-fixed-baseline.md`

Cagiran taraf sozlesmesi de ayri dokumanda sabitlendi:

- bkz. `docs/agc-c54x-runtime-contract.md`

Kaynak ve cycle tahmini de ayri notta toplandi:

- bkz. `docs/agc-c54x-resource-estimate.md`

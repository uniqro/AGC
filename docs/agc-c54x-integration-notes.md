# AGC C54x Integration Notes

Bu not, mevcut fixed-point baseline'in `TMS320VC5416` entegrasyonuna yaklasirken hangi cagirimi ve veri modelini hedefledigini kaydeder.

## Amaç

- Algoritma davranisini bozmadan, cagiran tarafi DSP-gercekci bir arayuze yaklastirmak
- `init -> sabit frame process` modelini netlestirmek
- Sonraki cycle, bellek ve accumulator iyilestirmeleri icin temiz bir taban olusturmak

## Mevcut DSP-Yakın Model

- Giris: `int16`
- Cikis: `int16`
- Ic sinyal: `Q15`
- Gain: `Q12.20`
- Frame: init sirasinda sabitlenir
- Runtime parametre degisimi yok

## Runtime Katmanı

`fixed/agc_fixed_runtime.h` ve `fixed/agc_fixed_runtime.c` ile su model tanimlandi:

1. `agc_fixed_runtime_init(...)`
   - config'i olusturur
   - pipeline state'i sifirlar
2. `agc_fixed_runtime_frame_samples(...)`
   - cagiran tarafa beklenen sabit frame boyunu verir
3. `agc_fixed_runtime_process(...)`
   - tam bir frame alir
   - tek cagrida detector -> gate -> gain -> peak protector -> limiter akisini calistirir

Bu model, ISR veya codec callback tarafinda tipik su kullanim sekline uygundur:

```c
agc_fixed_runtime_init(&runtime, 16000);
frame_samples = agc_fixed_runtime_frame_samples(&runtime);
agc_fixed_runtime_process(&runtime, input_frame, output_frame, &metrics);
```

## Build-Time Ayrimi

`fixed/agc_fixed_build.h` ile ilk derleme anahtari eklendi:

- `AGC_FIXED_ENABLE_METRICS=1`
  - compare ve tuning derlemeleri
- `AGC_FIXED_ENABLE_METRICS=0`
  - release yolunda frame metric doldurmayi kapatmak icin

Bu ayirimda algoritma akisi ayni kalir; yalnizca metrik doldurma ve compare odakli ek maliyetler kapanir.

## Neden Ayrı Runtime Katmanı?

- `AgcFixedConfig` ve `AgcFixedPipelineState` dis koddan ayrisir
- Ust katman sadece `runtime` nesnesini gorur
- Hedefe ozel optimizasyonlar ayni arayuz altinda ilerleyebilir

## Sonraki TMS320 Odakli Adımlar

- `size_t` kullanimlarini hedefe uygun daha dar tiplerle gozden gecirmek
- Gerekirse `40-bit accumulator` kullanan varyant noktalarini isaretlemek
- `gain_buffer_q15` yerlesimini bellek bolgesi acisindan degerlendirmek
- Kritik ic dongulerde hedefe ozel `restrict` / unroll / intrinsics stratejisi belirlemek
- Metrics toplama yolunu release build icin kosullu hale getirmek

## Guncelleme - Frame Tipi

Bu adim sonrasi fixed cekirdekte frame uzunlugu arayuzu isimli bir dar tip ile tutuluyor:

- `agc_frame_count_t`

Ilk tanim:

- `typedef uint16_t agc_frame_count_t;`

Bu guncelleme detector, gain, limiter, peak protector, pipeline ve runtime arayuzlerine yansitildi.

## Guncelleme - 40-bit Aday Noktalari

Davranis degistirilmeden, ilk 40-bit accumulator adaylari kod icinde isaretlendi:

- detector enerji toplami
- gain smoothing icindeki `error * alpha`
- gain carpimindan sonra olusan genis ara degerler

Ayrica `gain_buffer_q15`, hedefe ozel bellek bolgesi veya scratch alanina tasinabilecek ilk tampon olarak not edildi.

## Guncelleme - 40-bit Yoluna Gecis

Ilk aday noktalar artik genis tipte calisiyor:

- detector enerji toplami `agc_accum40_t`
- gain smoothing ara carpimi `agc_accum40_t`
- gain sonrasi genis tampon `agc_accum40_t`
- peak protector ve limiter genis tampon uzerinden calisiyor

Not:

- masaustu tarafta bu tip `int64_t` ile temsil ediliyor
- hedef DSP'de bu yol daha sonra C54x'e ozel accumulator/intrinsic ile eslenebilir

## Guncelleme - Bellek ve Derleyici Hook'lari

Bu hook denemesi su asamada geri alindi.

Neden:

- Hedef derleyici/pragmalari henuz somutlastirilmis degildi
- Saglayacagi fayda teorik kalirken kod karmaşıklığı artıyordu

Mevcut durum:

- `gain_buffer_q15` genis tampon olarak korunuyor
- hedefe ozel section/alignment/restrict/hot baglantisi daha sonraki asamaya ertelendi

## Guncelleme - Runtime Sozlesmesi

Runtime kullanim beklentisi ayri dokumanda sabitlendi:

- `docs/agc-c54x-runtime-contract.md`

Kaynak ve cycle tahmini de ayri dokumanda toplandi:

- `docs/agc-c54x-resource-estimate.md`

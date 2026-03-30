# AGC C54x Runtime Contract

Bu dosya, mevcut fixed baseline icin C54x tarafinda beklenen cagirım sozlesmesini sabitler.

## Amaç

- Cagiran tarafin `init/process` kullanimini netlestirmek
- Buffer ve frame beklentilerini acik hale getirmek
- Port sirasinda entegrasyon kaynakli yanlis kullanimi azaltmak

## Kapsam

Bu sozlesme su baseline icin gecerlidir:

- `docs/agc-fixed-baseline.md`
- `fixed/agc_fixed_runtime.h`
- `fixed/agc_fixed_runtime.c`

## Cagirim Modeli

Beklenen kullanim:

1. `agc_fixed_runtime_init(&runtime, sample_rate_hz);`
2. `frame_samples = agc_fixed_runtime_frame_samples(&runtime);`
3. Her cagrida tam bir frame ver:
   - `agc_fixed_runtime_process(&runtime, input_frame, output_frame, metrics);`

## Giris/Cikis Sozlesmesi

- Giris veri tipi: `int16`
- Cikis veri tipi: `int16`
- Giris ve cikis mono frame buffer olarak ele alinir
- In-place isleme bu sozlesmede garanti edilmez
  - cagiran taraf ayri `input_frame` ve `output_frame` vermelidir

## Frame Sozlesmesi

- Runtime init sonrasi `frame_samples` sabittir
- Cagiran taraf her process cagrısında tam bu boyda frame vermelidir
- Eksik frame / parca frame bu baseline sozlesmesinde desteklenmez

## Ornekleme Hizi Sozlesmesi

- `sample_rate_hz`, init sirasinda verilir
- Frame boyu bu hiza gore bir kez hesaplanir
- Runtime sirasinda ornekleme hizi degismez

## Parametre Sozlesmesi

- Kullanici parametreleri runtime sirasinda degismez
- Bu baseline'da parametreler init-sonrasi sabit kabul edilir
- Parametre guncelleme ihtiyaci dogarsa, yeni bir runtime instance veya acik bir reconfigure akisi tasarlanmalidir

## Metrics Sozlesmesi

- `metrics` pointer'i `NULL` olabilir
- `AGC_FIXED_ENABLE_METRICS=0` iken metrics doldurma release yolunda compile-time kapanabilir
- Cagiran taraf metrics'e urun mantigi baglamamali; bu alan debug/tuning yardimcisidir

## Hata/Red Kosullari

`agc_fixed_runtime_process(...)` su durumda `0` donebilir:

- frame boyu gecersizse
- runtime icindeki sabit frame siniri asiliyorsa

Bu baseline'da daha ayrintili hata kodu yoktur; sadece basit kabul/red modeli vardir.

## Gecikme ve Durum

- Algoritma stateful'dur
- Detector, gate, gain, peak protector ve limiter state'i frame'ler arasinda korunur
- Gecikme ek bir look-ahead buffer'a dayanmaz; frame tabanli state ilerlemesi vardir

## Cagiran Taraf Icin Basit Kural Seti

- Init'i bir kez yap
- Frame boyunu sor ve sabitle
- Her cagrida tam frame ver
- Giris/cikis tamponlarini ayri tut
- Metrics'i yalnizca izleme amaciyla kullan

## Bu Sozlesmenin Bilincli Olarak Kapsamadigi Alanlar

- ISR/codec DMA detaylari
- hedefe ozel section/pragmas
- runtime parametre guncelleme
- stereo/cok kanalli akis
- parca frame veya overlap-akis

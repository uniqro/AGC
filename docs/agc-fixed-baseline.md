# AGC Fixed Baseline

Bu dosya, mevcut fixed-point DSP baseline'ini sabitler.

## Durum

- Baseline tarihi: `2026-03-30`
- Amaç: reference davranisina yakin, sade ve tasinabilir fixed-point cekirdegi sabitlemek

## Sabitlenen Yapı

- `Q15` sinyal yolu
- `Q12.20` gain yolu
- `agc_accum40_t` ile genis ara hesap yolu
- detector
- gate
- gain control
- peak protector
- limiter
- pipeline
- runtime wrapper

## Sabitlenen Ilkeler

- Runtime float yok
- Giris/cikis `int16`
- Frame uzunlugu sabit
- Metrics derleme anahtari ile kapatilabilir
- `target_rms_percent` ve `max_gain_db` runtime init arayuzunden verilebilir
- Tam-frame API yaninda parca input biriktiren buffered runtime arayuzu da bulunur
- Buna ek olarak tek-sample giris/cikis yapan streaming runtime arayuzu da bulunur
- Hedefe ozel bellek/derleyici hook'lari bu asamada baseline'a dahil degil

## Reference Uyum Ozeti

Son olculen karsilastirma duzeyi:

- `burst`: `mae=0.004022`
- `step`: `mae=0.001335`
- `step_hot`: `mae=0.001508`
- `speech_turns`: `mae=0.004312`
- `noisy_speech_turns`: `mae=0.004639`

Bu seviye, mevcut port asamasi icin yeterli yakinlik olarak kabul edildi.

## Bu Baseline'dan Sonra

Bu noktadan sonraki degisiklikler iki sinifa ayrilir:

1. Davranisi koruyan hedefe ozel entegrasyon/performans degisiklikleri
2. Davranisi etkileyen algoritmik degisiklikler

Ikinci sinif degisiklikler baseline ustunde yeni bir iterasyon olarak degerlendirilmelidir.

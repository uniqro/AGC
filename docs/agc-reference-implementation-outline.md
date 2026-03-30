# AGC Reference Implementation Outline

Bu dosya, reference implementasyonun dosya ve fonksiyon seviyesindeki taslak yapisini tanimlar.
Amac, gercek koda gecmeden once modulleri, veri akislarini ve sorumluluk sinirlarini netlestirmektir.

## 1. Hedef

- Reference implementasyonu okunabilir ve gozlemlenebilir tutmak
- DSP hedefinden once davranis dogrulamasi yapmak
- Sonraki implementasyon asamasinda karar boslugu birakmamak

## 2. Onerilen Klasor ve Dosya Yapisi

Asagidaki yapi reference implementasyon icin C diliyle reference implementasyon icin onerilir:

- `reference/README.md`
- `reference/agc_config.h`
- `reference/agc_config.c`
- `reference/agc_types.h`
- `reference/agc_level_detector.h`
- `reference/agc_level_detector.c`
- `reference/agc_gate.h`
- `reference/agc_gate.c`
- `reference/agc_gain_control.h`
- `reference/agc_gain_control.c`
- `reference/agc_limiter.h`
- `reference/agc_limiter.c`
- `reference/agc_pipeline.h`
- `reference/agc_pipeline.c`
- `reference/agc_metrics.h`
- `reference/agc_metrics.c`
- `reference/agc_test_vectors.h`
- `reference/agc_test_vectors.c`
- `reference/main.c`

Istege bagli bagimsiz on isleme blogu:
- `reference/agc_filters.h`
- `reference/agc_filters.c`

Not:
- Dil C olarak secildi.
- Amac dosya sorumluluklarini sabitlemek.
- Ilk iskelet bu dosya adlariyla olusturuldu.

## 3. Dosya Sorumluluklari

### `reference/README.md`
- Reference implementasyonun amacini anlatir.
- Calistirma akisini ve beklenen girdileri ozetler.
- Uretilen log ve grafiklerin ne anlama geldigini yazar.

### `reference/agc_config.c` / `reference/agc_config.h`
- Ornekleme hizi, frame suresi, hedef seviye, attack/release, gate ve limiter parametrelerini tutar.
- Baslangic varsayilanlarini tek yerde toplar.
- Farkli test profilleri tanimlanacaksa bunlari burada yapilandirir.

### `reference/agc_types.h`
- Ortak veri yapilarini tanimlar.
- Onerilen yapilar:
  - `AgcConfig`
  - `FrameMetrics`
  - `GateState`
  - `GainState`
  - `LimiterState`
  - `PipelineState`

### `reference/agc_filters.c` / `reference/agc_filters.h`
- HP ve LP filtre bloklarini uygular.
- AGC pipeline'ina zorunlu bagli degildir.
- Bagimsiz on isleme/ayri deney blogu olarak kullanilabilir.

### `reference/agc_level_detector.c` / `reference/agc_level_detector.h`
- Enerji/envelope olcumunu yapar.
- Peak tabanli ara olcekleme icin gerekli olcumleri de uretir.
- Ham seviye olcumu ile yumusatilmış seviye olcumunu ayirabilir.

### `reference/agc_gate.c` / `reference/agc_gate.h`
- Konusma/gurultu ayrimi icin basit enerji esikli gate mantigini uygular.
- Hold ve histerezis benzeri davranislari ic state ile yonetir.
- Sessizlikte gain sismesini engellemek icin gate kararini uretir.

### `reference/agc_gain_control.c` / `reference/agc_gain_control.h`
- Hedef seviyeye gore istenen gain'i hesaplar.
- Attack/release kurallarini uygular.
- Max gain ve gain saturation mantigini yönetir.
- 32-bit baslangic ve overflow gozlemi icin log noktalari burada bulunabilir.

### `reference/agc_limiter.c` / `reference/agc_limiter.h`
- AGC sonrasi emniyet limiter davranisini uygular.
- Limiter aktivasyonu ve clipping yakinligi gibi metrikleri uretir.
- Limiter ana kontrol blogu degil, son koruma adimi olarak kalir.

### `reference/agc_pipeline.c` / `reference/agc_pipeline.h`
- Tum bloklari birlestiren ana akistir.
- Bir frame icin sira su olur:
  1. Giris frame'ini al
  2. Seviye olc
  3. Gate karari uret
  4. Gain hesapla ve uygula
  5. Limiter uygula
  6. Metrikleri topla
- Disariya tek bir `process_frame(...)` arayuzu sunmasi onerilir.

### `reference/agc_metrics.c` / `reference/agc_metrics.h`
- Frame bazli ara metrikleri toplar.
- Cikti olarak tablo, log veya daha sonra cizime uygun seriler uretir.
- Onerilen metrikler:
  - giris seviye olcumu
  - filtrelenmis seviye
  - gate durumu
  - ham gain istegi
  - yumusatilmış gain
  - limiter aktivasyonu
  - cikis seviyesi
  - clipping veya saturation gozlemleri

### `reference/agc_test_vectors.c` / `reference/agc_test_vectors.h`
- Sentetik ve gercek veriye yakin test senaryolarini tanimlar.
- Onerilen senaryolar:
  - sessizlikten konusmaya gecis
  - konusmadan sessizlige donus
  - degisken SNR
  - ani patlama
  - uzun sessizlik
  - farkli Fs degerleri

### `reference/main.c`
- Reference calistirma giris noktasi olur.
- Test vektorlerini yukler.
- Pipeline'i frame frame calistirir.
- Sonuclari loglar ve ozet cikarir.

## 4. Onerilen Veri Yapilari

### `AgcConfig`
- `sample_rate`
- `frame_ms`
- `frame_samples`
- `target_rms_fs`
- `attack_ms`
- `release_ms`
- `envelope_window_ms`
- `gate_threshold`
- `gate_hold_ms`
- `max_gain`
- `limiter_threshold`

### `PipelineState`
- `gate_state`
- `gain_state`
- `limiter_state`
- `frame_index`

### `FrameMetrics`
- `input_peak`
- `input_energy`
- `filtered_energy`
- `gate_open`
- `desired_gain`
- `applied_gain`
- `limiter_active`
- `output_peak`
- `output_energy`
- `saturation_flag`
- `overflow_flag`

## 5. Onerilen Fonksiyon Imzalari

Dil bagimsiz kavramsal imzalar:

- `build_config(...) -> AgcConfig`
- `init_pipeline_state(config) -> PipelineState`
- `apply_filters(frame, state, config) -> filtered_frame, state`
- `measure_level(frame, state, config) -> level_info`
- `update_gate(level_info, state, config) -> gate_decision, state`
- `compute_desired_gain(level_info, gate_decision, config) -> desired_gain`
- `smooth_gain(desired_gain, state, config) -> applied_gain, state`
- `apply_gain(frame, applied_gain, state, config) -> gained_frame, state`
- `apply_limiter(frame, state, config) -> limited_frame, state, limiter_info`
- `collect_metrics(...) -> FrameMetrics`
- `process_frame(frame, state, config) -> output_frame, state, metrics`
- `run_test_vector(signal, config) -> all_metrics`

## 6. Veri Akisi

Bir frame icin veri akisi:

`raw input`
-> `level measurement`
-> `gate decision`
-> `desired gain`
-> `attack/release smoothing`
-> `gain application`
-> `limiter`
-> `output + metrics`

Bu akis reference implementasyonda acik ve adim adim izlenebilir tutulmali.

## 7. Reference Asamada Zorunlu Gozlemlenebilirlik

Reference uygulama su sorulara cevap verebilmelidir:

- Gain neden burada artti?
- Gate neden kapandi veya acildi?
- Limiter neden devreye girdi?
- Sessizlikte gain sisiyor mu?
- Farkli Fs degerlerinde davranis benzer mi?
- Overflow veya saturation nerede goruldu?

Bu nedenle her blok icin ara metrik uretmek zorunlu kabul edilmelidir.

## 8. Uygulama Sirasinda Hala Dogrulanacak Ayrintilar

- FIR tap sayilari ve pencere secimi
- Baslangic yonu olarak yaklasik `~50 tap` dusunulebilir
- Clipping icin net sayisal oran/esik
- Limiter icin net sayisal oran/esik
- Overflow logundan 40-bit'e gecis uygulama ayrintisi
- Peak tabanli ara olcekleme katsayilari

## 9. Sonraki Adim

Bu taslaktan sonra mantikli ilerleme sirasi:

1. Reference implementasyon dili/ortamini secmek
2. Dosya iskeletini olusturmak
3. Pipeline iskeletini bos ama calisir hale getirmek
4. Metrik/log altyapisini once kurmak
5. Ardindan bloklari tek tek uygulamak

Not:
- Filtre blogu reference AGC'nin zorunlu parcasi degil.
- Gerekirse ayri bir on isleme deneyi olarak kullanilabilir.

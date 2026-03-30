# AGC C54x Resource Estimate

Bu not, mevcut fixed baseline icin TMS320VC5416 tarafinda beklenen kaynak ve gecikme maliyetini kabaca tahmin eder.

## Amac

- DSP entegrasyonundan once RAM ve cycle butcesini gormek
- `agc_fixed_runtime_process_sample(...)` yolunun maliyetini anlamak
- Hangi maliyetlerin kesin bilgi, hangilerinin tahmin oldugunu ayirmak

## Kapsam

Bu tahmin su kod tabani icin gecerlidir:

- `fixed/agc_fixed_runtime.c`
- `fixed/agc_fixed_pipeline.c`
- `fixed/agc_fixed_detector.c`
- `fixed/agc_fixed_gate.c`
- `fixed/agc_fixed_gain.c`
- `fixed/agc_fixed_peak_protector.c`
- `fixed/agc_fixed_limiter.c`

## Kesin Bilgiler

- Frame suresi: `4 ms`
- Frame boyu: `N = Fs * 0.004`
- `process_sample(...)` frame bazli cekirdegi korur
- `process_sample(...)` tek sample input alir
- `process_sample(...)` tek sample output verir
- Sample API, input frame toplama ve output frame bosaltma icin dahili tampon kullanir
- Ilk frame tamamlanana kadar output sifirdir

## Varsayimlar

- Hedef build release odakli dusunulurse `AGC_FIXED_ENABLE_METRICS=0`
- C54x derleyici, temel donguleri beklenen sekilde optimize eder
- `memcpy` kucuk boylarda lineer kopya gibi davranir
- Host tarafindaki `agc_accum40_t = int64_t`, DSP tarafinda 40-bit accumulator mantigini temsil eden stand-in olarak dusunulur

## Belirsizlikler

- Exact cycle sayisi, TI C54x derleyicisi ve optimizasyon seviyesi olmadan bilinemez
- `sqrt` benzeri integer kok dongusunun derleyici tarafindaki gercek maliyeti farkli olabilir
- Bellek bank/cycle etkisi hedefteki gercek yerlesime baglidir

## Latency

### Frame API

- `agc_fixed_runtime_process(...)` icin algoritmik gecikme ek bir look-ahead kaynakli degildir
- Isleme bir frame ustunden yapildigi icin sistem entegrasyonunda dogal blok gecikmesi `4 ms` olarak dusunulmelidir

### Sample API

- `agc_fixed_runtime_process_sample(...)` ilk `N` sample boyunca output olarak `0` verir
- `N+1` inci cagridan itibaren ilk islenmis sample cikmaya baslar
- Bu nedenle sample API icin etkin gecikme yaklasik `1 frame = 4 ms` dir

Neden:

- Input once dahili pencereye dolar
- Frame dolunca AGC cekirdegi bir kez calisir
- Uretilen output frame daha sonra sample sample bosaltilir

## RAM Kullanim Tahmini

`AgcFixedRuntime` icindeki ana alanlar:

- `AgcFixedConfig`: yaklasik `32 byte`
- `AgcFixedPipelineState` kucuk state alanlari + sayaclar: yaklasik `34 byte`
- `gain_buffer_q15`: `6 * N byte`
- `input_window`: `2 * N byte`
- `output_window`: `2 * N byte`

Toplam yaklasik:

- `RAM ~= 66 + 10N byte`

### Ornekler

- `Fs = 8 kHz`, `N = 32`
  - `RAM ~= 66 + 320 = 386 byte`
- `Fs = 16 kHz`, `N = 64`
  - `RAM ~= 66 + 640 = 706 byte`
- `Fs = 64 kHz`, `N = 256`
  - `RAM ~= 66 + 2560 = 2626 byte`
- `N = 512` sinirinda
  - `RAM ~= 66 + 5120 = 5186 byte`

Not:

- Bu hesap kabadir ve derleyici padding farki olabilir
- `AgcFixedFrameMetrics` runtime icinde zorunlu tutulmaz; cagiranda ayrica verilirse ek alan gerekir

## Stack Tahmini

Frame isleme sirasinda lokal degiskenler:

- `AgcFixedLevelInfo`
- birkac scalar gecici degisken

Kaba stack beklentisi:

- `40-100 byte`

Bu deger derleyiciye bagli oldugu icin tahmindir.

## Cycle Modeli

`agc_fixed_runtime_process_sample(...)` iki farkli maliyet profiline sahiptir.

### A. Cogu cagrida

Frame henuz dolmamisken:

- bir sample input pencereye yazilir
- varsa output penceresinden bir sample okunur
- sayaclar guncellenir

Tahmin:

- `20-35 cycle`

### B. Her N sample'da bir

Frame dolar ve tam AGC cekirdegi calisir.

Bu durumda ek maliyet:

- detector
- gate
- gain hesap
- gain apply
- peak protector
- limiter
- final output sat ve peak takibi

## Blok Bazli Frame Cycle Tahmini

Asagidaki degerler kaba tahmindir.

### Detector

Isler:

- `N` sample absolute deger
- `N` sample enerji toplami
- `N` sample envelope update
- frame sonu RMS kok hesaplama
- RMS smoothing

Tahmin:

- `~12N + 150..250 cycle`

Neden:

- per-sample islem yogun
- integer square-root dongusu sabit ek maliyet getirir

### Gate

Isler:

- esik karsilastirma
- hold sayaci guncelleme

Tahmin:

- `~10-20 cycle`

### Gain Law ve Smoothing

Isler:

- RMS tabanli desired gain
- max gain sinirlama
- attack/release secimi
- bir adet genis ara carpim

Tahmin:

- `~20-30 cycle`

### Gain Apply

Isler:

- `N` sample input * gain
- genis tampon yazimi
- overflow izleme

Tahmin:

- `~5N cycle`

### Peak Protector

Isler:

- once frame peak taramasi
- gerekiyorsa tum frame olcekleme

Tahmin:

- peak koruma yoksa: `~3N cycle`
- peak koruma aktifse: `~7N cycle`

### Limiter

Isler:

- `N` sample esik kontrolu
- gerekiyorsa clamp

Tahmin:

- `~4N cycle`

### Final Output Convert

Isler:

- genis tampondan `int16` saturate
- output peak takibi

Tahmin:

- `~4N cycle`

## Toplam Frame Tahmini

Tipik frame:

- `C_frame_typical ~= 28N + 220 cycle`

Peak protector aktif frame:

- `C_frame_peak ~= 32N + 220 cycle`

Bu, limiter'in agir degil, peak protector'in ise ara sira devreye girdigi baseline icin makul ilk tahmindir.

## process_sample Ortalama Cycle Tahmini

Bir sample cagrisi ortalamada:

- `C_avg ~= C_light + (C_frame / N)`

Burada:

- `C_light ~= 20-35 cycle`

### 8 kHz Ornegi

- `N = 32`
- `C_frame_typical ~= 1116 cycle`
- ortalama: yaklasik `55-70 cycle/sample`
- en kotu tek cagrida: yaklasik `1100-1300 cycle`

### 16 kHz Ornegi

- `N = 64`
- `C_frame_typical ~= 2012 cycle`
- ortalama: yaklasik `50-67 cycle/sample`
- en kotu tek cagrida: yaklasik `2000-2300 cycle`

### 64 kHz Ornegi

- `N = 256`
- `C_frame_typical ~= 7388 cycle`
- ortalama: yaklasik `49-64 cycle/sample`
- en kotu tek cagrida: yaklasik `7400-8400 cycle`

## Mcycle/s Tahmini

Ortalama `60 cycle/sample` kabul edilirse:

- `8 kHz`: `~0.48 Mcycle/s`
- `16 kHz`: `~0.96 Mcycle/s`
- `64 kHz`: `~3.84 Mcycle/s`

## Neden-Sonuc Ozeti

- Sample API kullanimi entegrasyonu kolaylastirir ama `4 ms` gecikme getirir
- RAM maliyetini en cok `gain_buffer_q15`, `input_window` ve `output_window` belirler
- Ortalama cycle/sample, frame boyu buyudukce cok hizli buyumez; cunku frame maliyeti sample'lara yayilir
- En kotu tek cagrida cycle sivrilmesi olur; bu sivrilme frame'in doldugu cagridadir

## Bu Tahminin Sinirlari

Bu not exact profil sonucu degildir.

Daha kesin sonuc icin gerekir:

- TI C54x derleyicisi ile build
- optimizasyon seviyesi secimi
- generated assembly/listing incelemesi
- hedefte cycle olcumu

## Onerilen Sonraki Adim

- Bu notu butce referansi olarak kullan
- Gercek C54x build alininca detector ve square-root maliyetini assembly uzerinden dogrula
- Gerekirse `process_sample(...)` yerine frame veya buffered API secimi tekrar degerlendir

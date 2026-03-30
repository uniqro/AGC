# Task Plan

Bu dosya, proje icin aktif gorev planini ve karar odakli ilerleme sirasini tutar.

## Hedef

- AGC icin uygun algoritma ailesini secmek.
- Dusuk riskli bir reference implementasyon yaklasimi belirlemek.
- Hedef ortama gecis icin eksik teknik bilgileri netlestirmek.

## Aktif Plan

### Asama 1. Gereksinim Toplama
- Sinyal tipi, kullanim amaci ve calisma ortamini soru-cevap ile netlestir.
- Kesin bilgi, varsayim ve acik sorulari ayri kaydet.
- Celiskileri gorunur bicimde isaretle.
- Durum: Tamamlandi.

### Asama 2. Karar Cercevesi
- Toplanan cevaplara gore AGC seceneklerini daralt.
- Projeye ozel karar matrisi olustur.
- Dusuk riskli baslangic algoritmasini sec.
- Durum: Tamamlandi.
- Cikti: `docs/agc-decision-matrix.md`

### Asama 3. Reference Yaklasim
- Tek bant, hizli attack, yavas release, kisa enerji penceresi, basit gate ve emniyet limiter yapisini referans aday olarak tanimla.
- Olcum, loglama ve test yaklasimini tanimla.
- Parametre seti ve gozlem kriterlerini cikar.
- Sessizlikte gain sismesini onleyen davranisi referansin zorunlu parcasi olarak ele al.
- Durum: Tamamlandi.
- Cikti: `docs/agc-reference-algorithm.md`

### Asama 4. Hedef Ortama Gecis Hazirligi
- Platform kisitlarini fixed-point ve ornekleme hizi olceklemesi baglaminda netlestir.
- Gerekli ek bilgileri topla.
- Porting ve dogrulama planini olustur.
- Durum: Buyuk oranda netlesti; kalanlar implementasyon sirasinda dogrulanacak.
- Cikti: `docs/agc-target-environment-info.md`

### Asama 5. Teknik Ozet
- Dagitik karar kayitlarini tek bir implementasyon-odakli ozet dokumanda birlestir.
- Reference ve DSP implementasyonu icin kullanilacak guncel teknik resmi sabitle.
- Durum: Tamamlandi.
- Cikti: `docs/agc-technical-spec-summary.md`

### Asama 6. Reference Taslak
- Reference implementasyonun dosya ve fonksiyon seviyesindeki yapisini tanimla.
- Veri akislarini ve state sinirlarini netlestir.
- Koda gecmeden once moduller arasi sorumluluk dagilimini sabitle.
- Durum: Tamamlandi.
- Cikti: `docs/agc-reference-implementation-outline.md`

### Asama 7. Reference C Iskeleti
- C dilinde derlenebilir reference kod iskeletini olustur.
- Filtre, detector, gate, gain, limiter ve pipeline modullerini ayir.
- Ilk calisan frame bazli akisi dogrula.
- Durum: Tamamlandi.
- Cikti: `reference/`

### Asama 8. DSP Port Hazirligi
- Reference davranisi sabitle.
- DSP portu icin veri temsili ve Q-format kararlarini koda donuk hale getir.
- Fixed-point bloklara gecis sirasini belirle.
- Durum: Aktif. Temel veri tipi, config, detector, gate, gain, peak protector, limiter ve pipeline iskeleti olusturuldu; reference-fixed karsilastirma harness'i eklendi; DSP-facing runtime katmani baslatildi; mevcut fixed baseline sabitlendi.
- Cikti:
  - `docs/agc-reference-baseline.md`
  - `docs/agc-dsp-porting-plan.md`
  - `fixed/agc_fixed_types.h`
  - `fixed/agc_fixed_config.h`
  - `fixed/agc_fixed_config.c`
  - `fixed/agc_fixed_math.h`
  - `fixed/agc_fixed_detector.h`
  - `fixed/agc_fixed_detector.c`
  - `fixed/agc_fixed_gate.h`
  - `fixed/agc_fixed_gate.c`
  - `fixed/agc_fixed_gain.h`
  - `fixed/agc_fixed_gain.c`
  - `fixed/agc_fixed_peak_protector.h`
  - `fixed/agc_fixed_peak_protector.c`
  - `fixed/agc_fixed_limiter.h`
  - `fixed/agc_fixed_limiter.c`
  - `fixed/agc_fixed_pipeline.h`
  - `fixed/agc_fixed_pipeline.c`
  - `fixed/agc_fixed_compare.c`
  - `fixed/agc_fixed_build.h`
  - `fixed/agc_fixed_runtime.h`
  - `fixed/agc_fixed_runtime.c`
  - `docs/agc-fixed-baseline.md`
  - `docs/agc-c54x-integration-notes.md`
  - `docs/agc-c54x-runtime-contract.md`
  - `compare_*_reference_output.wav`
  - `compare_*_fixed_input.wav`
  - `compare_*_fixed_output.wav`

## Uygulama Sirasinda Dogrulanacak Ayrintilar

- Clipping icin net sayisal oran/esik
- Limiter aktivasyonu icin net sayisal oran/esik
- Uyarlamali gate/V2 ihtiyacinin tekrar degerlendirilmesi
- FIR filtre tap sayilari ve pencere secimi
- Overflow logundan 40-bit'e gecis uygulama ayrintisi
- Peak tabanli olcekleme katsayilari
- Farkli Fs degerlerinde performans etkisi
- Bagimsiz filtreleme blogunun ayrica kullanimi
- Buyuk step girislerinde peak-headroom cap davranisinin tuning'i

## Acik Riskler

- Kisa envelope penceresi nedeniyle gain oynakligi ve pompalanma riski.
- Degisken SNR altinda basit gate yaklasiminin yetersiz kalma riski.
- "Ara sira clipping" toleransinin sayisal tanimsizligi.
- Q15 davranis ayrintilari, gain/cikis saturation stratejisi ve 4 ms frame'in farkli ornekleme hizlarinda olcekleme ayrintilari.
- DSP entegrasyon yapisi net olmadigi icin arayuz kararlarinin gecikmesi.
- Fixed peak protector davranisinin reference'a gore ozellikle `burst/step` testlerinde fazla agresif kalma riski.

## Guncelleme Gecmisi

### 2026-03-29
- Ilk gorev plani olusturuldu.
- Gereksinim toplama asamasi tamamlandi olarak isaretlendi.
- Karar matrisi ve reference algoritma plani bir sonraki aktif adim olarak netlestirildi.
- Acik riskler AGC profiline gore guncellendi.
- Karar cercevesi ve reference yaklasim dokumanlari eklendi.
- Hedef ortama gecis icin bilgi toplama dokumani eklendi.

### 2026-03-30
- int16 giris, Q15 hedef temsil ve 4 ms sabit frame bilgileri plana yansitildi.
- Parametrelerin yalnizca init sirasinda ayarlanacagi eklendi.
- Bekleyen kararlar filtreleme blogu, saturation kurallari ve kabul esitleri etrafinda daraltildi.
- Temel bant filtresi, AGC -> limiter -> cikis zinciri ve 32-bit once tercihi eklendi.
- Yuksek geciren + alcak geciren filtre yapisi ve 4 ms frame sample karsiliklari eklendi.
- 250 Hz HP, `0.4125 * Fs` LP, overflow-log gecisi ve uyarlamali olcekleme yaklasimi eklendi.
- FIR, orta derece, peak tabanli olcekleme ve tum Fs'lerde ayni frame kuralı eklendi.
- Kalan maddeler bekleyen karar olmaktan cikarilip implementasyon sirasinda dogrulanacak ayrintilar olarak yeniden siniflandirildi.
- Tek dosyada implementasyon-odakli teknik ozet eklendi.
- Reference implementasyon icin dosya/fonksiyon seviyesinde taslak eklendi.
- C dilinde derlenebilir reference iskeleti eklendi ve ilk calisma dogrulandi.
- Filtre blogu AGC pipeline'indan ayrildi; bagimsiz on isleme olarak yeniden konumlandi.
- Tek bir step vektorune gore `max_gain` kisma yerine, peak-headroom cap ile daha genellenebilir step korumasi referansa eklendi.
- Fixed cekirdekte `size_t` yerine `agc_frame_count_t` kullanimi baslatildi ve DSP-facing arayuz daraltildi.

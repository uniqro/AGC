# Iteration Notes

Bu dosya, iterasyon bazli calisma notlarini ve ozetleri tutar.

## Iteration 001 - Baslangic Yapilandirmasi

### Tarih
- 2026-03-29

### Bu iterasyonda yapilanlar
- Proje icin kalici Markdown dokumantasyon yapisi olusturuldu.
- Gereksinim toplama icin soru-cevap dosyasi acildi.
- Kalici proje kurallari ayri dosyaya yazildi.
- Gorev plani ayri dosyaya yazildi.

### Ogrenilenler
- Depoda henuz AGC ile ilgili mevcut dokumantasyon bulunmuyor.
- Bu nedenle bilgi modeli sifirdan ve kontrollu bicimde kurulabiliyor.

### Acik Kalanlar
- Sinyal tipi
- Ana kullanim amaci
- Calisma modu
- Hedef platform
- Gurultu davranisi beklentisi

### Sonraki Adim
- Kullanicidan ilk gereksinim cevaplarini toplamak.

## Iteration 002 - Gereksinim Netlestirme

### Tarih
- 2026-03-29

### Bu iterasyonda yapilanlar
- AM Radio konusmasi icin AGC ihtiyaci soru-cevap ile netlestirildi.
- Hedef platformun TMS320VC5416 DSP oldugu kaydedildi.
- Tasarimin Fs-genel olmasi ve 8/16/64 kHz uzerinde dogrulanmasi kararlastirildi.
- Ilk algoritma ailesi olarak tek bant AGC + basit gate + emniyet limiter yapisi secildi.
- Kullaniciya acilacak parametreler ve baslangic zaman sabiti araliklari kaydedildi.
- Acik sorular ve tasarim gerilimleri gorunur bicimde ayrildi.

### Toplanan Soru-Cevap Ozetleri
- Sinyal AM Radio konusmasi ve mono.
- Islem gercek zamanli calisacak.
- Konusma net ve yuksek olmali, ancak gurultu konusma kadar yukselmemeli.
- Sessizlikte gain belirgin sekilde artmamali.
- Kullanici hedef seviyeyi FS yuzdesi olarak ayarlayabilmeli.
- Ilk gate yaklasimi basit enerji esigi olacak.
- Ilk surumde limiter emniyet blogu gibi davranacak.

### Kesinlesen Teknik Tercihler
- Frame: 5-10 ms
- Attack: 5-20 ms
- Release: 200-800 ms
- Envelope penceresi: 2-10 ms
- Gate hold: 50-150 ms
- Fixed-point uyumlu plan
- Fs-genel algoritma tanimi
- Dinleme testi agirlikli kabul

### Gorunur Celiski Listesi
- 64 kHz tek default varsayimindan Fs-genel tasarima gecildi.
- Guvenlik odakli ilk cizgiden, konusma netligi lehine daha agresif tercihlere kayis oldu.
- Kisa envelope penceresi secildi; bu karar hiz kazandirirken oynaklik riskini artiriyor.
- Limiter emniyet blogu olarak kalirken, nadir clipping toleransi ve ara sira limiter aktivasyonu kabul edildi.

### Acik Kalanlar
- Nadir clipping ve limiter aktivasyonunun sayisal esikleri.
- Gate histerezisinin parametrelesmesi.
- Uyarlamali gate/V2 tetik kosulu.
- AGC oncesi/sonrasi gercek DSP zinciri.

### Sonraki Adim
- Karar matrisi olusturmak.
- Dusuk riskli reference algoritma yaklasimini yazmak.

## Iteration 003 - Karar Matrisi ve Reference Notu

### Tarih
- 2026-03-29

### Bu iterasyonda yapilanlar
- Projeye ozel AGC karar matrisi olusturuldu.
- Reference algoritma notu ayri dosyada dokumante edildi.
- Gorev planinda karar cercevesi ve reference yaklasim tamamlandi olarak isaretlendi.

### Uretilen Dokumanlar
- `docs/agc-decision-matrix.md`
- `docs/agc-reference-algorithm.md`

### Ozet
- Tek bant enerji/envelope tabanli AGC secimi gerekceleriyle kaydedildi.
- Sabit gain, multi-band AGC ve uyarlamali gurultu tabani gibi alternatiflerin neden elendigi veya ertelendigi yazildi.
- Reference blok akisi, loglanacak metrikler ve test senaryolari belirlendi.

### Acik Kalanlar
- Sayisal clipping esigi
- Limiter aktivasyon toleransi
- Gate histerezisi parametresi
- Uyarlamali gate tetik kosulu
- DSP zinciri entegrasyon ayrintilari

### Sonraki Adim
- Hedef ortama gecis icin ek teknik bilgileri toplamak.
- Fixed-point implementasyon oncesi kabul ve entegrasyon ayrintilarini netlestirmek.

## Iteration 004 - Hedef Ortam Bilgi Ihtiyaclari

### Tarih
- 2026-03-29

### Bu iterasyonda yapilanlar
- Hedef ortama gecis icin gerekli ek teknik bilgi listesi ayri dosyada toplandi.
- Giris/cikis arayuzu, Q-format, DSP zinciri ve kabul esikleri gibi eksik alanlar kategorize edildi.
- Gorev planinda Asama 4 aktif olarak guncellendi.

### Uretilen Dokuman
- `docs/agc-target-environment-info.md`

### Ozet
- Reference davranistan urun implementasyonuna gecis icin hangi verilerin eksik oldugu netlestirildi.
- En kritik alanlarin fixed-point sayisal tasarim, DSP entegrasyonu ve kabul esikleri oldugu kaydedildi.

### Acik Kalanlar
- Gercek veri formati ve Q-format
- Saturation ve ara hesap genisligi kurallari
- AGC'nin zincirdeki kesin yeri
- Sayisal clipping/limiter kabul esikleri
- Runtime parametre kontrol modeli

### Sonraki Adim
- Hedef ortama gecis icin eksik teknik cevaplari soru-cevap ile toplamak.

## Iteration 005 - Hedef Ortam Teknik Netlestirme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- int16 giris formati ve Q15 hedef sayisal temsil kesinlestirildi.
- Frame yapisi 4 ms sabit frame olarak guncellendi.
- Parametrelerin yalnizca init sirasinda ayarlanacagi kaydedildi.
- DSP zinciri yonu filtreleme blogu -> AGC -> diger ses bloklari olarak daraltildi.
- Filtreleme blogunun AGC oncesindeki amaci dokumante edildi.
- Clipping, limiter ve gate histerezisi tercihleri guncellendi.

### Toplanan Teknik Cevap Ozeti
- AGC girisi int16 olacak.
- Hedef ic temsil Q15 olacak.
- Frame yapisi 4 ms olacak.
- Parametreler runtime'da degismeyecek, init sirasinda set edilecek.
- Clipping icin ara sira kabul var.
- Limiter'in ara sira devreye girmesi normal kabul ediliyor.
- Gate histerezisi kullaniciya acik olmayacak.

### Filtreleme Blogu Notu
- Filtreleme blogu, AGC'den once gereksiz frekans icerigini ve AGC olcumunu bozabilecek istenmeyen bilesenleri azaltmak icin dusunuluyor.
- Bu sayede AGC daha anlamli bir envelope/enerji olcumu uzerinden karar verebilir.

### Acik Kalanlar
- Filtreleme blogunun tam tipi ve parametreleri
- AGC sonrasindaki bloklar
- Q15 saturation ve ara hesap genisligi kurallari
- Ara sira clipping ve limiter aktivasyonunun sayisal esikleri
- Uyarlamali gate/V2 tetik kosulu

### Sonraki Adim
- Kalan teknik bosluklari soru-cevap ile kapatmak.

## Iteration 006 - Sayisal ve Zincir Tercihleri

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Filtreleme blogu tipi temel bant sinirlama filtresi olarak daraltildi.
- DSP zinciri AGC -> limiter -> cikis olacak sekilde sadeleştirildi.
- Q15 icin once 32-bit ara hesap tercih edildi; gerekirse 40-bit'e gecis not edildi.
- Saturation tercihi daha sonra gain ve cikista saturation olacak sekilde guncellendi.
- Clipping ve limiter tercihleri daha somut bicimde not edildi.
- Uyarlamali gate/V2 icin su asamada tetik tanimlanmayip sadece not olarak tutulacagi yazildi.

### Toplanan Teknik Cevap Ozeti
- Filtreleme: yuksek geciren + alcak geciren kombinasyonu
- Zincir: AGC -> limiter -> cikis
- Ara hesap: once 32-bit, hata gorulurse 40-bit
- Saturation: gain ve cikista
- Clipping: kisa sureli ve gorulur seviyede kabul olabilir, ama test sonucuna gore degisebilir
- Limiter: konusma patlamalarinda devreye girmesi normal
- V2 gate: simdilik sadece not

### Gorunur Tasarim Gerilimleri
- 32-bit once yaklasimi hesap maliyeti icin iyi, ancak sinir kosullarinda 40-bit gereksinimi dogabilir.
- Clipping kabulunun test sonucuna gore degisebilmesi, kabul kriterlerinin henuz tam kapanmadigini gosteriyor.
- Gain ve cikista saturation secimi daha guvenli bir cizgi saglasa da ara adim olcekleme kurallari hala acik.

### Acik Kalanlar
- Yuksek geciren ve alcak geciren filtrelerin tam parametreleri
- Clipping ve limiter'in sayisal kabul esikleri
- 32-bit'ten 40-bit'e gecis kosulu
- Ara adim olcekleme kurallari
- 8/16/64 kHz disindaki hizlar icin frame uyarlama kurali

### Sonraki Adim
- Kalan sayisal kabul ve fixed-point koruma kurallarini soru-cevap ile netlestirmek.

## Iteration 007 - Filtre ve Esik Yonleri

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Yuksek geciren filtre kesimi 250 Hz olarak kaydedildi.
- Alcak geciren filtre kesimi `0.4125 * Fs` olarak kaydedildi.
- Clipping ve limiter tercihleri daha sayisal bir yone daraltildi.
- 32-bit'ten 40-bit'e gecis kararinin overflow veya hata loglarina bakilarak verilecegi not edildi.
- Ara adim olcekleme yaklasimi sinyal seviyesine gore uyarlamali olarak secildi.
- 8/16/64 kHz disindaki hizlar icin `frame_samples = Fs * 0.004` ve en yakin tam sayiya yuvarlama kurali kaydedildi.

### Toplanan Teknik Cevap Ozeti
- HP: 250 Hz
- LP: `0.4125 * Fs`
- Clipping: kisa burst icinde kucuk oran kabul yonu
- Limiter: patlamali bolumlerde kisa sureli kumelenme normal
- 32-bit -> 40-bit: overflow/hata logu gorulurse
- Ara adim olcekleme: uyarlamali
- Frame kuralı: `Fs * 0.004`, en yakin tam sayiya yuvarla

### Acik Kalanlar
- FIR filtre tap sayilari ve pencere yaklasimi
- Clipping icin net sayisal oran/esik
- Limiter aktivasyonu icin net sayisal oran/esik
- Overflow logundan 40-bit'e gecis icin uygulama kuralı
- Peak tabanli olceklemenin tam katsayi/esik kurali
- 8/16/64 disindaki hizlarda ek sistem kisiti olup olmadigi

### Sonraki Adim
- Son kalan sayisal esik ve filtre gercekleme kararlarini soru-cevap ile kapatmak.

## Iteration 008 - Filtre Gercekleme ve Sayisal Kural Yonleri

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Filtre gercekleme tercihi FIR olarak secildi.
- HP ve LP icin orta derece yaklasimi secildi.
- 32-bit'ten 40-bit'e gecisin herhangi bir overflow/hata logu ile tetiklenmesi yonu kaydedildi.
- Ara adim olceklemede peak tabanli uyarlamali yaklasim secildi.
- 8/16/64 disindaki hizlarin da ayni 4 ms frame kuralıyla desteklenecegi kaydedildi.

### Toplanan Teknik Cevap Ozeti
- Filtre tipi: FIR
- Filtre derece yonu: HP orta, LP orta
- 40-bit gecis: herhangi bir overflow/hata logu
- Ara adim olcekleme: peak tabanli uyarlamali
- Diger Fs hizlari: desteklenir, `frame_samples = Fs * 0.004`

### Acik Kalanlar
- FIR tap sayilari ve pencere secimi
- Clipping icin net oran/esik
- Limiter icin net oran/esik
- Overflow logundan 40-bit'e geciste uygulama ayrintisi
- Peak tabanli olceklemenin tam katsayi/esik kurali
- Diger Fs hizlarinda ek performans kisiti olup olmadigi

### Sonraki Adim
- Son kalan esik ve implementasyon kurallarini soru-cevap ile kapatmak.

## Iteration 009 - Ayrintilari Yeniden Siniflandirma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Tekrarlanan soru-cevap dongusunu durdurmak icin kalan maddeler yeniden siniflandirildi.
- Acik soru gorunumu altinda kalan teknik ayrintilar, implementasyon sirasinda dogrulanacak tuning ve sayisal davranis basliklarina tasindi.
- Gorev plani, temel kararlar kapanmis kabul edilecek sekilde sadeleştirildi.

### Ozet
- Ana tasarim kararlari artik yeterli kabul edildi.
- Kalan konular urun yonunu degistiren temel belirsizlikler degil, implementasyon sirasinda dogrulanacak ayrintilar olarak ele alinacak.

### Uygulama Sirasinda Dogrulanacak Ayrintilar
- FIR tap sayilari ve pencere secimi
- Clipping ve limiter icin net sayisal esikler
- Overflow logundan 40-bit'e gecis uygulama ayrintisi
- Peak tabanli olcekleme katsayilari
- Farkli Fs degerlerinde performans etkisi

### Sonraki Adim
- Mevcut karar setini yeterli kabul edip reference implementasyon veya teknik taslak asamasina gecmek.

## Iteration 010 - Teknik Ozet Birlesimi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Dagitik kararlar tek bir implementation-ready teknik ozet dosyasinda birlestirildi.
- Gorev planina teknik ozet asamasi eklendi.

### Uretilen Dokuman
- `docs/agc-technical-spec-summary.md`

### Ozet
- Sinyal profili, algoritma cekirdegi, filtreleme zinciri, frame yapisi, fixed-point kararlari ve kabul yonleri tek dosyada toplandi.
- Bundan sonraki calismalar icin birincil basvuru dokumani olusturuldu.

### Sonraki Adim
- Mevcut teknik ozeti kullanarak reference implementasyon taslagi veya daha formal DSP implementasyon spesifikasyonu hazirlamak.

## Iteration 011 - Reference Taslak Yapisi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Reference implementasyonun dosya seviyesindeki modulleri tanimlandi.
- Fonksiyon sinirlari ve veri akis sirasi belirlendi.
- Pipeline state, config ve metrics yapilari kavramsal olarak sabitlendi.

### Uretilen Dokuman
- `docs/agc-reference-implementation-outline.md`

### Ozet
- Koda gecmeden once hangi dosyanin ne yapacagi netlesti.
- Reference implementasyonun gozlemlenebilir ve tuning dostu olmasi icin gerekli metrik noktalar belirginlestirildi.

### Sonraki Adim
- Istenirse bu taslaga gore reference kod iskeleti olusturmak.

## Iteration 012 - Reference C Iskeleti

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `reference/` altinda C dilinde moduler AGC iskeleti olusturuldu.
- Ortak config, types, filters, level detector, gate, gain control, limiter, metrics ve pipeline dosyalari eklendi.
- `main.c` ile tek frame calistiran basit bir giris noktasi eklendi.
- FIR filtreleme, temel seviye olcumu, gate, gain smoothing ve limiter icin ilk calisir davranis dolduruldu.

### Teknik Sonuc
- Kod `gcc reference\\*.c -Ireference -lm -o reference\\agc_ref.exe` ile derlendi.
- Cikti calistirilarak ilk frame bazli akisin yurudugu dogrulandi.

### Acik Kalanlar
- FIR tap sayilarinin son hale getirilmesi
- Clipping ve limiter icin net sayisal kabul esikleri
- Peak tabanli olceklemenin tuning katsayilari
- Overflow logundan 40-bit'e gecis mekanizmasinin ayrintilandirilmasi

### Sonraki Adim
- Reference iskeletini daha gercekci algoritma davranisiyla iteratif olarak doldurmak.

## Iteration 013 - Filtreyi AGC'den Ayirma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Filtre blogu AGC pipeline'inin zorunlu parcasi olmaktan cikarildi.
- AGC pipeline veri akisi dogrudan giris -> seviye olcumu -> gate -> gain -> limiter -> cikis olacak sekilde sadeleştirildi.
- Filtreleme dokumanlarda bagimsiz on isleme/ayri deney blogu olarak yeniden konumlandirildi.

### Ozet
- AGC'nin temel gorevi filtreleme ile karismayacak sekilde ayrildi.
- Bu sayede AGC tuning'i ve filtre tuning'i bagimsiz ilerleyebilir.

### Sonraki Adim
- Istenirse bagimsiz filtre blogu ayri test akisiyla degerlendirilir.
- Reference AGC davranisi filtre haric dogrudan iyilestirilmeye devam edilir.

## Iteration 014 - AGC Smoothing Yonunu Duzeltme ve WAV Yenileme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Gain smoothing mantigindaki yon hatasi duzeltildi.
- Ani yuksek seviye girislerde gain'in hizli dusmesi, dusuk seviyeye donuste ise daha yavas toparlanmasi saglandi.
- Mevcut test vector'leri yeniden calistirildi ve cikis WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_step` senaryosunda limiter baskisi ciddi bicimde azaldi.
- Guncel ozet:
  - `reference_burst`: `gate_frames=270`, `limiter_frames=0`, `max_gain=1.6956`, `max_output_peak=0.4238`
  - `reference_step`: `gate_frames=500`, `limiter_frames=2`, `max_gain=3.8930`, `max_output_peak=0.9800`
  - `reference_noisy`: `gate_frames=500`, `limiter_frames=0`, `max_gain=2.7121`, `max_output_peak=0.5353`

### Ozet
- Problem sadece parametre agresifligi degildi; gain artirma/azaltma yonleri AGC beklentisine gore ters eslenmisti.
- Bu duzeltme sonrasi step ve noisy senaryolari daha kontrollu hale geldi.

### Sonraki Adim
- Guncel WAV'lar dinlenerek pumping, gec tepki veya gereginden fazla seviye dusuklugu kontrol edilir.
- Gerekirse target, gate ve max gain uzerinde ikinci tuning turu yapilir.

## Iteration 015 - Muhafazakar Seviye Yukselme Tuning'i

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `target_rms_fs` `0.35`'ten `0.40`'a yukseltilerek cikis seviyesi bir miktar artirildi.
- `max_gain` `4.0`'tan `3.5`'e cekilerek yuksek gain senaryolari biraz daha sinirlandi.
- `gate_threshold` `0.05`'ten `0.06`'ya yukseltilerek sessizlik/dusuk seviye acilmasi daha muhafazakar hale getirildi.
- Test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_burst`: `gate_frames=270`, `limiter_frames=0`, `max_gain=1.9014`, `max_output_peak=0.4753`
- `reference_step`: `gate_frames=500`, `limiter_frames=2`, `max_gain=3.4108`, `max_output_peak=0.9800`
- `reference_noisy`: `gate_frames=500`, `limiter_frames=0`, `max_gain=2.9112`, `max_output_peak=0.5788`

### Ozet
- `burst` ve `noisy` senaryolari bir miktar daha yuksek cikis verdi.
- `step` senaryosunda limiter davranisi anlamli bicimde kotulesmedi; hala sadece az sayida frame'de etkin.
- Bu tur, daha yuksek duyulan seviye ile kontrol kaybi olmadan ilerlemek icin dengeli bir ara nokta sagladi.

### Sonraki Adim
- Dinleme sonucu gerekiyorsa hedef seviye veya gate esigi uzerinde bir ince tuning daha yapilabilir.
- `step` senaryosundaki kalan kucuk limiter aktivasyonu istenirse daha da azaltilabilir.

## Iteration 016 - Attack Hizini Artirma Denemesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `attack_ms` `15.0`'ten `8.0`'e cekilerek yuksek seviye artislarina daha hizli tepki denenmistir.
- Test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- Gozlenen metriklerde anlamli bir degisim olmadi:
  - `reference_burst`: `limiter_frames=0`, `max_output_peak=0.4753`
  - `reference_step`: `limiter_frames=2`, `max_output_peak=0.9800`
  - `reference_noisy`: `limiter_frames=0`, `max_output_peak=0.5788`

### Ozet
- Bu test setinde `attack_ms` tek basina belirleyici parametre olmadi.
- Kalan kucuk limiter aktivasyonunun daha cok hedef seviye, gain tavani veya test sinyal gecis yapisiyla iliskili oldugu dusunuluyor.

### Sonraki Adim
- Gerekirse `step` senaryosu icin `target_rms_fs` veya `max_gain` cok kucuk bir adimla yeniden ayarlanir.

## Iteration 017 - Max Gain Azaltma Denemesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `max_gain` `3.5`'ten `3.2`'ye cekilerek yuksek kazanc kosullari biraz daha sinirlandi.
- Test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_burst`: `limiter_frames=0`, `max_output_peak=0.4610`
- `reference_step`: `limiter_frames=0`, `max_output_peak=0.8348`
- `reference_noisy`: `limiter_frames=0`, `max_output_peak=0.5600`

### Ozet
- `step` senaryosundaki kalan limiter aktivasyonu sifirlandi.
- Buna karsilik `step` cikis seviyesi onceki tura gore belirgin bicimde dustu.
- Bu degisiklik, limiter temasi ile duyulan seviye arasinda net bir trade-off oldugunu gosterdi.

### Sonraki Adim
- Dinleme onceligi daha yuksekse `max_gain=3.5` seviyesine geri donup kucuk limiter temasini kabul etmek mantikli olabilir.
- Daha guvenli cikis oncelikliyse `max_gain=3.2` korunabilir.

## Iteration 018 - Peak Headroom Cap ve Ikinci Step Stres Testi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Cozum arayisi tek bir `step` vektorune gore `max_gain` kisma cizgisinden cikarildi.
- Hedef envelope kazancina ek olarak, frame icindeki giris tepesi uzerinden hesaplanan bir `headroom cap` eklendi.
- Gain uygulamasindaki adaptif sinirlama, limiter esigine gore hizalandi.
- Daha sert bir ikinci step vektoru (`reference_step_hot`) eklendi ve WAV ciktilari uretildi.

### Teknik Sonuc
- `reference_burst`: `limiter_frames=0`, `max_output_peak=0.4610`
- `reference_step`: `limiter_frames=0`, `max_output_peak=0.8323`
- `reference_step_hot`: `limiter_frames=0`, `max_output_peak=0.9604`
- `reference_noisy`: `limiter_frames=0`, `max_output_peak=0.5600`

### Ozet
- Yeni yaklasim, belirli bir test vektorune ozel `max_gain` kisma yerine, buyuk step'lerde de kendini koruyan yapisal bir cozum sagladi.
- `step_hot` gibi daha sert geciste bile limiter'e dusmeden tepe seviye kontrol altinda tutuldu.
- Trade-off tamamen kalkmadi; buyuk step geldikce cikis seviyesi dogal olarak daha fazla sinirlanacak. Ancak bu sinirlama artik tek bir test vektorune ozel degil, genellenebilir bir kuralla yapiliyor.

### Sonraki Adim
- Dinleme sonucuna gore `safe_peak` marji veya `target_rms_fs` ince ayari yapilabilir.
- Benzer sekilde sessizlikten konusmaya hizli gecis ve degisken SNR icin ek stres vektorlari eklenebilir.

## Iteration 019 - RMS Hedefine Gecis

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `target_level_fs` alani `target_rms_fs` olarak yeniden adlandirildi.
- Gain hesabi envelope yerine frame RMS olcumu uzerinden yapilacak sekilde guncellendi.
- `filtered_rms` metrigi eklendi.
- `max_gain` tekrar `4.0` yapildi.
- Test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_burst`: `limiter_frames=0`, `max_output_peak=0.5484`
- `reference_step`: `limiter_frames=0`, `max_output_peak=0.9604`
- `reference_step_hot`: `limiter_frames=0`, `max_output_peak=0.9604`
- `reference_noisy`: `limiter_frames=0`, `max_output_peak=0.6926`

### Ozet
- RMS hedefi ile `step` cikisi belirgin bicimde yukari geldi.
- Buna ragmen `step` ve `step_hot` seviyeleri tam ayni hedefe oturmadi; cunku peak-headroom cap buyuk adimlarda gain'i halen sinirliyor.
- Yani bu tur, hedef tanimini duzeltti ama AGC hedefi ile peak korumasi arasindaki mimari gerilimi tamamen ortadan kaldirmadi.

### Sonraki Adim
- Istenirse bir sonraki turda `step` davranisi icin AGC ve peak protector ayrimi daha belirgin hale getirilebilir.

## Iteration 020 - RMS AGC ve Peak Protector Ayrimi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Peak sinirlama mantigi AGC gain hesabindan cikarildi.
- Ayrı `peak protector` blogu eklendi.
- Pipeline sirasi `RMS AGC -> peak protector -> limiter` olacak sekilde guncellendi.
- Test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_burst`: `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5484`
- `reference_step`: `peak_protector_frames=1`, `limiter_frames=0`, `max_output_peak=0.9310`
- `reference_step_hot`: `peak_protector_frames=3`, `limiter_frames=0`, `max_output_peak=0.9310`
- `reference_noisy`: `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6926`

### Ozet
- Artik AGC hedef seviyesi ile peak guvenligi ayni hesapta karismiyor.
- Buyuk step'lerde koruma halen var, ancak bunun hangi bloga ait oldugu net: AGC degil, peak protector.
- `step` ve `step_hot` farkinin devam etmesi artik daha okunabilir hale geldi; bu fark RMS AGC'den degil, tepe koruma davranisindan geliyor.

### Sonraki Adim
- Istenirse bir sonraki turda RMS hedefi ile peak protector marji arasindaki iliski daha sistematik tuning ile incelenebilir.

## Iteration 021 - RMS Hedefi ve Peak Protector Marji Tuning'i

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `peak_protector_ratio` parametresi config'e tasindi.
- `target_rms_fs` `0.40`'tan `0.42`'ye yukseltilerek AGC hedef seviyesi biraz artirildi.
- `peak_protector_ratio` `0.97` olarak secilerek peak protector limiter esigine biraz daha yaklastirildi.
- Test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_burst`: `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5758`
- `reference_step`: `peak_protector_frames=1`, `limiter_frames=0`, `max_output_peak=0.9506`
- `reference_step_hot`: `peak_protector_frames=3`, `limiter_frames=0`, `max_output_peak=0.9506`
- `reference_noisy`: `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.7258`

### Ozet
- Genel cikis seviyesi bir miktar artti.
- Peak protector hala sinirli sayida frame'de devreye giriyor; limiter ise tamamen devre disi kaldi.
- Bu ayar, onceki tura gore biraz daha yuksek duyulan seviye ile benzer guvenlik seviyesini sagladi.

### Sonraki Adim
- Dinleme sonucuna gore `target_rms_fs` veya `peak_protector_ratio` icin bir ince tuning daha yapilabilir.

## Iteration 022 - Yeni Test Vector Seti

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Step odakli test setine ek olarak daha temsil gucu yuksek yeni vector'ler eklendi.
- Yeni vector'ler:
  - `reference_speech_turns`: farkli konusma bolumleri ve sessizlik aralari
  - `reference_noise_floor_step`: sabit konusma benzeri tasiyici uzerinde ani gurultu tabani artisı
  - `reference_pause_recovery`: konusma, uzun pause ve yeniden baslama
- Tum eski ve yeni vector'ler icin WAV dosyalari yeniden uretildi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=383`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5969`
- `reference_noise_floor_step`: `gate_frames=500`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.7906`
- `reference_pause_recovery`: `gate_frames=264`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5103`

### Ozet
- Yeni set, sadece step cevabini degil; gate davranisi, pause sonrasi toparlanma ve gurultu tabani artisina tepkiyi de gozlemlemeyi sagliyor.
- Bu vektorlerde peak protector ve limiter devreye girmeden AGC'nin temel davranisi izlenebilir hale geldi.

### Sonraki Adim
- Dinleme ve grafik incelemesinde hangi vektorun zayif nokta gosterdigi secilip tuning o eksen uzerinden devam ettirilebilir.

## Iteration 023 - Speech Turns Icin Onset Assist

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Gate kapali durumdan aciga gecerken, gain artisi gerekiyorsa hizli yukselis kullanildi.
- Bu sayede konusma baslangiclarinda gain'in yavas toparlanmasi yerine daha hizli reaksiyon denenmis oldu.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=383`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6223`
- `reference_pause_recovery`: `gate_frames=264`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5500`

### Speech Turns Segment Ozeti
- `speech_1`: `in_rms=0.0566`, `out_rms=0.1583`, `gain=2.797`
- `speech_2`: `in_rms=0.1221`, `out_rms=0.3100`, `gain=2.540`
- `speech_3`: `in_rms=0.0369`, `out_rms=0.1031`, `gain=2.794`
- `speech_4`: `in_rms=0.1578`, `out_rms=0.3398`, `gain=2.154`

### Ozet
- Dusuk seviyeli konusma bloklari artik daha hizli yukseliyor.
- Konusma bloklari arasindaki cikis seviyeleri hala tamamen esitlenmis degil, ancak onceki duruma gore daha tutarli.
- Bu iyilesme ozellikle pause sonrasi konusma baslangiclarinda goruluyor.

### Sonraki Adim
- Istenirse `speech_turns` uzerinde release, gate hold veya gate kapanis davranisi tuning'i ile devam edilebilir.

## Iteration 024 - Speech Turns Icin Release ve Hold Denemesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `release_ms` `450` ve `gate_hold_ms` `140` olacak sekilde daha uzun sureklilik denemesi yapildi.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=433`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6132`
- `reference_pause_recovery`: `gate_frames=294`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5102`

### Ozet
- Gate daha uzun acik kalsa da `speech_turns` bloklarinda beklenen duzeyde bir seviye esitlemesi saglanmadi.
- Buna ragmen gap bolgelerinde istenmeyen cikis yukselmesi gorulmedi.

### Sonraki Adim
- Detector state ile frame'ler arasi sureklilik eklemek.

## Iteration 025 - Stateful Detector ile Speech Turns Iyilestirmesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Level detector frame'ler arasi state tasiyacak sekilde guncellendi.
- Envelope olcumu onceki frame bilgisini devam ettirecek hale getirildi.
- RMS olcumu de frame RMS'inden uretilen `smoothed_rms` ile surekli hale getirildi.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=433`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6293`
- `reference_pause_recovery`: `gate_frames=294`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5110`

### Speech Turns Segment Ozeti
- `speech_1`: `out_rms=0.1484`, `gain=2.623`
- `speech_2`: `out_rms=0.3178`, `gain=2.603`
- `speech_3`: `out_rms=0.0969`, `gain=2.626`
- `speech_4`: `out_rms=0.3539`, `gain=2.244`

### Ozet
- Detector state eklendikten sonra ozellikle `speech_2` blogu diger konusma bloklarina daha yakin gain almaya basladi.
- Frame-bazli unutma etkisi azaldi ve seviye takibi daha tutarli hale geldi.
- Gap bolgelerinde istenmeyen tasma veya yalanci cikis yukselmesi gorulmedi.

### Sonraki Adim
- Istenirse `speech_turns` odaginda detector zaman sabiti ve gate esigi birlikte tuning edilebilir.

## Iteration 026 - Detector Penceresini Uzatma Denemesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `envelope_window_ms` `4` yerine `8` yapilarak detector davranisi daha yavas hale getirildi.
- Amaç `speech_turns` icinde daha kararlı gain davranisi elde etmekti.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=415`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6460`
- `reference_pause_recovery`: `gate_frames=294`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5120`

### Speech Turns Segment Ozeti
- `speech_1`: `out_rms=0.1465`, `gain=2.589`
- `speech_2`: `out_rms=0.3315`, `gain=2.716`
- `speech_3`: `out_rms=0.0857`, `gain=2.321`
- `speech_4`: `out_rms=0.3577`, `gain=2.268`

### Ozet
- Daha yavas detector tum konusma bloklarini daha iyi esitlemedi.
- `speech_2` ve `speech_4` biraz yukselirken `speech_3` geride kaldi; yani bloklar arasi tutarlilik net olarak iyilesmedi.
- Bu nedenle detector penceresini buyutmek `speech_turns` icin su an guclu bir tuning yonu gibi gorunmuyor.

### Sonraki Adim
- Istenirse bu deneme geri alinip `4 ms` detector penceresine donulebilir.
- Sonraki daha mantikli tuning ekseni gate esigi veya ayri RMS zaman sabiti olabilir.

## Iteration 027 - Detector Penceresini Geri Alma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `envelope_window_ms` tekrar `4 ms` degerine cekildi.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=433`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6293`
- `reference_pause_recovery`: `gate_frames=294`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5110`

### Ozet
- `speech_turns` davranisi daha onceki daha dengeli noktaya geri dondu.
- Bu geri donus, detector penceresini buyutmenin su an icin tercih edilmemesi gerektigini teyit etti.

### Sonraki Adim
- Bundan sonraki daha mantikli tuning ekseni gate esigi veya ayri RMS zaman sabiti olmaya devam ediyor.

## Iteration 028 - Speech Turns Icin Gate Esigi Denemesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `gate_threshold` `0.06`'dan `0.05`'e cekildi.
- Amaç dusuk seviyeli konusma bloklarinin daha kolay yakalanmasiydi.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=440`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6293`
- `reference_pause_recovery`: `gate_frames=294`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5110`

### Speech Turns Segment Ozeti
- `speech_1`: `out_rms=0.1540`, `gain=2.721`
- `speech_2`: `out_rms=0.3178`, `gain=2.603`
- `speech_3`: `out_rms=0.0969`, `gain=2.626`
- `speech_4`: `out_rms=0.3539`, `gain=2.244`

### Ozet
- Ilk dusuk seviyeli konusma blogu bir miktar iyilesti.
- Ancak genel bloklar arasi dengeyi kokten degistiren bir etki olusmadi.
- Bu nedenle `gate_threshold` tek basina en guclu tuning ekseni gibi gorunmuyor.

### Sonraki Adim
- Istenirse ayri bir RMS zaman sabiti veya gate/detector farkli zaman sabiti ayrimi denenebilir.

## Iteration 029 - Ayri RMS Zaman Sabiti Denemesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Envelope ve gate davranisini bozmadan, RMS olcumu icin ayri bir zaman sabiti eklendi.
- `rms_window_ms = 12` secildi; envelope penceresi `4 ms` olarak korundu.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=390`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6757`
- `reference_pause_recovery`: `gate_frames=264`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.5536`

### Speech Turns Segment Ozeti
- `speech_1`: `out_rms=0.1650`, `gain=2.916`
- `speech_2`: `out_rms=0.3343`, `gain=2.739`
- `speech_3`: `out_rms=0.1031`, `gain=2.794`
- `speech_4`: `out_rms=0.3680`, `gain=2.333`

### Ozet
- Bu deneme, `speech_turns` icin gate esigine gore daha guclu bir etki uretti.
- Dusuk ve orta seviye konusma bloklari bir miktar daha yukselirken genel davranis da daha canli hale geldi.
- Bloklar arasi tam esitleme hala yok, ancak bu eksen su ana kadarki daha umut verici tuning yonlerinden biri oldu.

### Sonraki Adim
- Istenirse `rms_window_ms` etrafinda ince tuning yapilabilir.

## Iteration 030 - RMS Pencere Secimi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `speech_turns` icin `rms_window_ms` degerleri `8/12/16 ms` olarak karsilastirildi.
- Bloklar arasi farki en az buyuten secim `8 ms` oldugu icin kod bu degere guncellendi.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_speech_turns`: `gate_frames=390`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.6553`
- `speech_1`: `out_rms=0.1650`, `gain=2.916`
- `speech_2`: `out_rms=0.3320`, `gain=2.720`
- `speech_3`: `out_rms=0.1031`, `gain=2.794`
- `speech_4`: `out_rms=0.3632`, `gain=2.302`

### Ozet
- `8 ms`, `12 ms` ve `16 ms` arasinda en dengeli RMS pencere secimi oldu.
- Tam esitleme saglanmasa da `speech_turns` icin daha kontrollu ve kabul edilebilir bir ara nokta elde edildi.

### Sonraki Adim
- Istenirse artik `speech_turns` yerine baska bir test vector odagina gecilebilir veya bu ayar seti gecici referans olarak sabitlenebilir.

## Iteration 031 - Noise Floor Step Icin Hafif Noise-Aware Gain Freni

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `envelope / rms` oranina dayali hafif bir gain freni eklendi.
- Amac, gürültü tabani yukselirken gain'in konusma ve gurultuyu ayni bicimde sisirmesini bir miktar azaltmakti.
- Ilk sert versiyon fazla agresif bulundugu icin daha yumusak bir agirlik ile tekrar denendi.
- Tum test vector'leri yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_noise_floor_step`: `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.7013`
- Kararlı pencereler:
  - `stable_low`: `out_rms=0.3430`, `gain=3.459`
  - `stable_high`: `out_rms=0.3405`, `gain=3.198`
- `reference_speech_turns`: `max_output_peak=0.6518`

### Ozet
- Gürültü artışından sonra `noise_floor_step` cikisi artik belirgin bicimde daha fazla buyumuyor; kararlı bolgede gain biraz geri cekiliyor.
- Buna karsilik `speech_turns` tamamen bozulmadi, ancak onceki saf RMS AGC durumuna gore bir miktar daha temkinli hale geldi.
- Bu eksen, `noise_floor_step` icin gercekten etkili bir kontrol kolu oldugunu gosterdi.

### Sonraki Adim
- Istenirse bu hafif noise-aware gain freni gecici referans kabul edilip baska bir test vector'e gecilebilir.

## Iteration 032 - Noisy Speech Turns Test Vector

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `speech_turns` vector'une arka plan gurultusu eklenmis yeni bir test vector tanimlandi.
- `reference_noisy_speech_turns` icin WAV dosyalari uretildi ve segment bazli olcumler alindi.

### Teknik Sonuc
- `reference_noisy_speech_turns`: `gate_frames=388`, `peak_protector_frames=0`, `limiter_frames=0`, `max_output_peak=0.7098`

### Segment Ozeti
- Konusma bloklari:
  - `speech_1`: `in_rms=0.0600`, `out_rms=0.1748`, `gain=2.911`
  - `speech_2`: `in_rms=0.1236`, `out_rms=0.3062`, `gain=2.478`
  - `speech_3`: `in_rms=0.0422`, `out_rms=0.1159`, `gain=2.743`
  - `speech_4`: `in_rms=0.1594`, `out_rms=0.3400`, `gain=2.133`
- Gap bloklari:
  - `gap_1`: `in_rms=0.0203`, `out_rms=0.0512`, `gain=2.517`
  - `gap_2`: `in_rms=0.0201`, `out_rms=0.0416`, `gain=2.068`
  - `gap_3`: `in_rms=0.0204`, `out_rms=0.0445`, `gain=2.185`

### Ozet
- Noise-aware gain freni, `noise_floor_step` icin fayda gosterse de `noisy_speech_turns` senaryosunda gap bolgelerindeki gurultu yukselmesini yeterince bastiramadi.
- Konusma bloklari makul seviyede kalirken, sessiz araliklarda gurultu hala anlamli bicimde buyuyor.
- Bu sonuc, eklenen ozellestirmenin getirdigi karmasikliga gore etkisinin sinirli oldugunu guclendiriyor.

### Sonraki Adim
- Istenirse noise-aware gain freni geri alinarak daha temiz cekirdek AGC surumune donulebilir.

## Iteration 033 - Noise-Aware Gain Frenini Geri Alma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `noise-aware gain` freni geri alindi.
- Cekirdek AGC tekrar su sade yapiya donduruldu:
  - RMS AGC
  - gate
  - peak protector
  - limiter
- Tum test vector'leri, `reference_noisy_speech_turns` dahil, yeniden calistirildi ve WAV dosyalari guncellendi.

### Teknik Sonuc
- `reference_noise_floor_step`:
  - `stable_low`: `out_rms=0.3848`, `gain=3.880`
  - `stable_high`: `out_rms=0.4080`, `gain=3.832`
- `reference_noisy_speech_turns`:
  - `speech_1`: `out_rms=0.1748`, `gain=2.911`
  - `speech_2`: `out_rms=0.3345`, `gain=2.707`
  - `speech_3`: `out_rms=0.1159`, `gain=2.743`
  - `speech_4`: `out_rms=0.3644`, `gain=2.286`
  - `gap_1`: `out_rms=0.0512`, `gain=2.517`
  - `gap_2`: `out_rms=0.0460`, `gain=2.286`
  - `gap_3`: `out_rms=0.0445`, `gain=2.185`

### Ozet
- `noise-aware gain` ozellestirmesi ek karmasiklik getirirken gürültülü konusma senaryosunda yeterince guclu fayda saglamadi.
- Bu nedenle sade cekirdek AGC surumu maliyet/fayda acisindan daha uygun bulundu.

### Sonraki Adim
- Istenirse artik bu sade referans davranisi sabitlenip baska test senaryolarina veya DSP portuna gecilebilir.

## Iteration 034 - Reference Bazini Sabitleme ve DSP Port Hazirligi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Sade cekirdek AGC davranisi baseline olarak sabitlendi.
- DSP portu icin ayri bir porting plani dokumani olusturuldu.
- Gorev planinda DSP port hazirligi aktif asama olarak eklendi.

### Uretilen Dokumanlar
- `docs/agc-reference-baseline.md`
- `docs/agc-dsp-porting-plan.md`

### Ozet
- Bundan sonraki DSP calismalari, sabitlenen reference davranistan tureyecek.
- Ilk hedef, fixed-point veri temsili ve Q-format kararlarini kod seviyesine tasimak olacak.

### Sonraki Adim
- `fixed/` odakli veri tipi ve konfig yapisini tanimlamak.

## Iteration 035 - Fixed-Point Temel Iskeleti

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/` altinda DSP odakli ilk dosyalar olusturuldu.
- Q15 sinyal temsili, gain icin daha genis sabit nokta formati ve pipeline state tipleri ayrildi.
- Reference baseline parametreleri fixed-point config yapisina tasindi.
- Attack/release ve detector katsayilari runtime float hesap yerine once Q15 katsayiya donusen yapiya cekildi.

### Uretilen Dosyalar
- `fixed/agc_fixed_types.h`
- `fixed/agc_fixed_math.h`
- `fixed/agc_fixed_config.h`
- `fixed/agc_fixed_config.c`

### Ozet
- Henuz fixed-point algoritma bloklari yazilmadi.
- Ama veri temsili, katsayi olcegi ve baseline parametrelerin DSP tarafi icin nasil tasinacagi somutlasmis oldu.

### Sonraki Adim
- `fixed/agc_fixed_detector` ve `fixed/agc_fixed_gate` bloklarini yazmak.

## Iteration 036 - Fixed Katmanda Float'tan Kacinma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/` altindaki config ve temel math yardimcilari float bagimliligindan arindirildi.
- Parametre donusumleri `oran -> Q15/Q20` seklinde tamsayi tabanli hale getirildi.
- DSP tarafinda dis girisin `signed 16-bit` oldugu varsayimi sabit kaldı.

### Ozet
- DSP portunun temel katmani artik float runtime'a dayanmiyor.
- Bu sayede sonraki `fixed detector` ve `fixed gate` calismalari dogrudan hedefe daha yakin bir zeminde ilerleyecek.

### Sonraki Adim
- `fixed/agc_fixed_detector` ve `fixed/agc_fixed_gate` bloklarini yazmak.

## Iteration 037 - Fixed Detector ve Gate Iskeleti

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/agc_fixed_detector` bloklari eklendi.
- `signed 16-bit` giristen Q15 olcum, envelope state ve `smoothed_rms` akisi kuruldu.
- `fixed/agc_fixed_gate` bloklari eklendi.
- Gate acma/kapatma ve hold davranisi Q15 esiklerle yazildi.

### Uretilen Dosyalar
- `fixed/agc_fixed_detector.h`
- `fixed/agc_fixed_detector.c`
- `fixed/agc_fixed_gate.h`
- `fixed/agc_fixed_gate.c`

### Ozet
- Fixed-point portun ilk gercek algoritma bloklari artik var.
- Henuz tum pipeline tamamlanmadi, ama detector ve gate davranisi DSP tarafi icin somut hale geldi.

### Sonraki Adim
- `fixed/agc_fixed_gain` ve `fixed/agc_fixed_peak_protector` bloklarini yazmak.

## Iteration 038 - Fixed Gain ve Peak Protector Iskeleti

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/agc_fixed_gain` bloklari eklendi.
- Desired gain, attack/release smoothing ve gain uygulama mantigi sabit nokta formatinda yazildi.
- `fixed/agc_fixed_peak_protector` bloklari eklendi.
- Peak tabanli koruma oran hesabiyla frame bazli olceklenebilir hale getirildi.

### Uretilen Dosyalar
- `fixed/agc_fixed_gain.h`
- `fixed/agc_fixed_gain.c`
- `fixed/agc_fixed_peak_protector.h`
- `fixed/agc_fixed_peak_protector.c`

### Ozet
- Fixed-point portta artik detector, gate, gain ve peak protector bloklari var.
- Limiter ve pipeline eklendiginde DSP odakli ilk uc bloktan tam akisa gecis saglanabilecek.

### Sonraki Adim
- `fixed/agc_fixed_limiter` ve `fixed/agc_fixed_pipeline` bloklarini yazmak.

## Iteration 039 - Fixed Limiter ve Pipeline Iskeleti

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/agc_fixed_limiter` bloklari eklendi.
- `fixed/agc_fixed_pipeline` ile detector, gate, gain, peak protector ve limiter birbirine baglandi.
- Fixed-point dosyalari derleme seviyesinde kontrol edildi.
- Reference WAV artefaktlari yeniden guncellendi.

### Uretilen Dosyalar
- `fixed/agc_fixed_limiter.h`
- `fixed/agc_fixed_limiter.c`
- `fixed/agc_fixed_pipeline.h`
- `fixed/agc_fixed_pipeline.c`

### Ozet
- Fixed-point tarafta artik ilk tam akisin iskeleti var.
- Bu asamada henuz reference ile birebir davranis karsilastirmasi yapilmadi; once iskelet butunlugu saglandi.

### Sonraki Adim
- Fixed pipeline icin karsilastirma/izleme yardimcilari eklemek veya dogrudan DSP hedefli optimizasyonlara gecmek.

## Iteration 040 - Reference-Fixed Karsilastirma Harness'i

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/agc_fixed_compare.c` ile reference ve fixed pipeline'i ayni test vector seti uzerinde yanyana calistiran ayri bir surucu eklendi.
- Ham `int16` giris/cikis WAV'lari yazabilmek icin `agc_write_wav_mono_s16_raw(...)` eklendi.
- Her compare kosusu icin reference cikisi, fixed input ve fixed output WAV artefaktlari uretilir hale getirildi.

### Uretilen Dosyalar
- `fixed/agc_fixed_compare.c`
- `compare_*_reference_output.wav`
- `compare_*_fixed_input.wav`
- `compare_*_fixed_output.wav`

### Ozet
- DSP portu artik sadece derlenebilir iskelet degil; reference davranisindan ne kadar saptigini olcebilecek bir karsilastirma aracina da sahip.
- Bu asama, sonraki fixed-point tuning ve TMS320VC5416 optimizasyonlari icin guvenli gozlem zemini sagliyor.

### Teknik Sonuc
- Ilk compare kosusu tamamlandi.
- Ozet farklar:
  - `compare_burst`: `mae=0.085025`, `max_err=0.361274`, `ref_pp=0`, `fixed_pp=75`
  - `compare_step`: `mae=0.174223`, `max_err=0.527221`, `ref_pp=3`, `fixed_pp=250`
  - `compare_step_hot`: `mae=0.236903`, `max_err=0.750934`, `ref_pp=5`, `fixed_pp=250`
  - `compare_speech_turns`: `mae=0.044701`, `max_err=0.282485`
- En belirgin sapma `peak protector` tarafinda goruldu; fixed surum, ozellikle `step` ailesinde reference'a gore cok daha fazla frame'de korumaya giriyor.

### Sonraki Adim
- Compare sonucundaki sapmalari inceleyip ilk olarak fixed `peak protector` ve gain/olcekleme esiklerini reference'a yaklastirmak.

## Iteration 041 - Fixed Ara Tampon ile Peak Protector Duzeltmesi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Fixed akista gain sonrasinda sinyalin dogrudan `int16`'ya satüre edilmesi kaldirildi.
- Pipeline state icine genis bir ara `Q15` tamponu eklendi.
- Peak protector ve limiter bu genis tampon uzerinde calisacak sekilde guncellendi.
- Compare ve reference WAV kosulari yeniden yapildi.

### Teknik Sonuc
- `step` ailesinde ortalama hata belirgin azaldi:
  - `compare_step`: `mae=0.174223 -> 0.116106`
  - `compare_step_hot`: `mae=0.236903 -> 0.115561`
- Ancak peak protector aktivasyonu hala reference'a gore cok yuksek:
  - `compare_step`: `ref_pp=3`, `fixed_pp=250`
  - `compare_step_hot`: `ref_pp=5`, `fixed_pp=250`

### Ozet
- Ara tampon duzeltmesi dogru yone gitti; fixed akisin tasma oncesi davranisi reference'a yaklasti.
- Buna ragmen asiri peak protector aktivasyonunun ana kaynagi muhtemelen gain/detector zaman sabiti ve katsayi esdegerligi tarafinda kaldi.

### Sonraki Adim
- Fixed attack/release ve detector alpha hesaplarini reference davranisina daha yakin hale getirmek.

## Iteration 042 - Fixed Detector Enerji Olcegini Duzeltme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Fixed detector enerji toplami Q15'e geri kaydirilmis carpim yerine dogrudan `x * x` ile Q30 olceginde duzeltildi.
- Compare ve reference ciktilari, ortamda `.exe` calistirma engeli oldugu icin ayni algoritma mantigini izleyen betik ile yeniden uretildi.
- Compare ve reference WAV artefaktlari yeniden yazildi.

### Teknik Sonuc
- Reference-fixed uyumu belirgin bicimde iyilesti:
  - `compare_burst`: `mae=0.004022`, `ref_pp=0`, `fixed_pp=0`
  - `compare_step`: `mae=0.001335`, `ref_pp=3`, `fixed_pp=4`
  - `compare_step_hot`: `mae=0.001508`, `ref_pp=5`, `fixed_pp=5`
  - `compare_speech_turns`: `mae=0.004312`
  - `compare_noisy_speech_turns`: `mae=0.004639`
- Peak protector sapmasi pratik olarak kapandi; `step` ailesindeki asiri fixed aktivasyonu ortadan kalkti.

### Ozet
- Asil sorun alpha esitliginden once detector enerji olceginin yanlis olmasiydi.
- Bu duzeltme sonrasinda fixed pipeline reference davranisina cok daha yakin hale geldi.

### Sonraki Adim
- Fixed-point baseline'i gecici olarak kabul edip TMS320VC5416 odakli veri yerlesimi, bellek ve performans optimizasyonuna gecmek.

## Iteration 043 - DSP-Facing Runtime Katmani

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/agc_fixed_runtime` katmani eklendi.
- `init -> frame_samples -> process` modeli ile DSP tarafina daha yakin bir cagirım deseni tanimlandi.
- C54x odakli entegrasyon notlari ayri dokumana yazildi.

### Uretilen Dosyalar
- `fixed/agc_fixed_runtime.h`
- `fixed/agc_fixed_runtime.c`
- `docs/agc-c54x-integration-notes.md`

### Ozet
- Algoritma davranisi degistirilmeden, fixed-point cekirdek daha DSP-gercekci bir ust katmana sarildi.
- Bu katman, sonraki bellek ve performans iyilestirmelerini uygulamadan once entegrasyon seklini netlestiriyor.

### Sonraki Adim
- Metrics/debug akislarini kosullu derleme ile ayirmak veya TMS320VC5416 icin tip/dongu daraltmalarina gecmek.

## Iteration 044 - Metrics/Debug Derleme Ayrimi

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `fixed/agc_fixed_build.h` eklendi.
- `AGC_FIXED_ENABLE_METRICS` anahtari ile fixed pipeline icinde metrik doldurma compile-time olarak kapatilabilir hale getirildi.
- `agc_fixed_compare` araci, metrikler kapaliyken derlenmemesi icin acik kontrol eklendi.
- C54x entegrasyon notlarina build-time ayrimi eklendi.

### Uretilen Dosyalar
- `fixed/agc_fixed_build.h`

### Ozet
- Algoritma davranisi degismeden, release yolundaki gereksiz metric maliyeti ayrisilabilir hale geldi.
- Bu adim, TMS320VC5416 tarafinda debug ve release build'leri ayirmak icin dusuk riskli bir hazirlik sagliyor.

### Sonraki Adim
- `size_t` ve benzeri masaustu agirlikli tipleri hedefe uygun daha dar tiplere indirmek.

## Iteration 045 - Frame Tipini Daraltma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Fixed cekirdekte frame uzunlugu arayuzu `size_t` yerine `agc_frame_count_t` ile ifade edildi.
- `agc_frame_count_t` ilk asamada `uint16_t` olarak tanimlandi.
- Detector, gain, limiter, peak protector, pipeline ve runtime arayuzleri bu tipe guncellendi.

### Ozet
- Algoritma davranisi degismeden, fixed-point cekirdek TMS320 entegrasyonuna daha yakin bir tip modeli kazandi.
- Ic dongu sayaclari ve frame API'si artik daha dar ve niyeti daha acik tiplerle ifade ediliyor.

### Sonraki Adim
- 40-bit accumulator gerektirebilecek noktalari kod icinde acikca isaretlemek veya `gain_buffer_q15` bellek yerlesimini hedefe gore planlamak.

## Iteration 046 - 40-bit Adaylarini Isaretleme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Algoritma davranisi degistirilmeden, fixed cekirdekte ilk 40-bit accumulator adaylari yorumlarla isaretlendi.
- `gain_buffer_q15` icin hedefe ozel bellek yerlesimi notu eklendi.

### Ozet
- Detector enerji toplami, gain smoothing carpimi ve genis gain yolu TMS320VC5416 icin ilk sayisal risk/optimizasyon noktalarinin basi olarak acikca belirlendi.
- Bu adim, sonraki hedefe ozel optimizasyonlarda nereden baslanacagini netlestiriyor.

### Sonraki Adim
- `gain_buffer_q15` icin hedefe ozel bellek yerlesimi stratejisi veya C54x odakli kosullu 40-bit varyant isaretleri eklemek.

## Iteration 047 - 40-bit Adaylarini Genis Tipe Tasima

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `agc_accum40_t` eklendi ve host tarafinda `int64_t` ile temsil edildi.
- Detector enerji toplami genis tipe tasindi.
- Gain smoothing icindeki ara carpim genis tipe tasindi.
- `gain_buffer_q15` genis tipe tasindi.
- Peak protector ve limiter genis tampon uzerinden calisacak sekilde guncellendi.

### Ozet
- 40-bit adaylari artik sadece yorum seviyesinde degil, kod seviyesinde genis tip kullaniyor.
- Public fixed-point arayuzu degismedi; genisleme yalnizca ic ara hesap yolunda yapildi.

### Sonraki Adim
- `gain_buffer_q15` icin hedefe ozel bellek yerlesimi veya C54x'e ozel accumulator esleme katmani eklemek.

## Iteration 048 - Hedefe Ozel Hook'lari Ekleme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Build katmanina hedefe ozel placement/hint makrolari eklendi.
- `gain_buffer_q15` hizalama ve scratch adayi olarak isaretlendi.
- Runtime ve pipeline process arayuzleri `restrict` ve `hot` hook'lari ile guncellendi.

### Ozet
- Algoritma degismeden, C54x tarafinda section/alignment ve derleyici ipucu uyarlamasi icin zemin olustu.
- Bu hook'lar masaustu derlemede no-op veya uyumlu attribute olarak kaliyor.

### Sonraki Adim
- C54x hedef dalinda `AGC_FIXED_SCRATCH` ve `AGC_FIXED_ALIGN` makrolarini arac-zincire gore somutlastirmak.

## Iteration 049 - Hedefe Ozel Hook'lari Geri Alma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `AGC_FIXED_SCRATCH`, `AGC_FIXED_ALIGN`, `AGC_FIXED_RESTRICT` ve `AGC_FIXED_HOT` denemesi geri alindi.
- `gain_buffer_q15` uzerindeki hook kullanimlari kaldirildi.
- Runtime ve pipeline arayuzleri sade haline donduruldu.

### Ozet
- Bu katman davranisi degistirmiyordu, bu nedenle guvenli sekilde geri alindi.
- Hedefe ozel yerlesim ve derleyici ipuclari, arac-zinciri netlestiginde daha somut bir asamada tekrar ele alinacak.

### Sonraki Adim
- Sayisal davranisi ve fixed-point baseline'i koruyarak ilerlemek; hedefe ozel yerlesimi daha sonra dogrudan C54x baglami ile eklemek.

## Iteration 050 - Fixed Baseline'i Sabitleme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Mevcut fixed-point durum resmi baseline olarak kabul edildi.
- Fixed baseline'i ayri dokumanda sabitlendi.
- Gorev plani ve DSP porting notlari bu baseline'a baglandi.

### Uretilen Dokuman
- `docs/agc-fixed-baseline.md`

### Ozet
- Reference ile yakinligi yeterli bulunan sade fixed cekirdek bu asamada kararlı taban olarak kabul edildi.
- Bundan sonraki calismalar baseline koruyan entegrasyon/perf iyilestirmeleri veya yeni davranis degisikligi iterasyonlari olarak ayrilacak.

### Sonraki Adim
- Bu baseline ustunden C54x entegrasyon detaylari veya performans/yerlesim calismalarina gecmek.

## Iteration 051 - C54x Runtime Sozlesmesini Sabitleme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- C54x tarafinda beklenen `init/process` kullanimini ayri dokumanda sabitledik.
- Frame, buffer, metrics ve parametre beklentileri acik yazildi.

### Uretilen Dokuman
- `docs/agc-c54x-runtime-contract.md`

### Ozet
- Bu adim algoritma degisikligi degil, entegrasyon riski azaltma adimidir.
- Cagiran tarafin nasil kullanmasi gerektigi artik daginik notlar yerine tek yerde toplandi.

### Sonraki Adim
- Bu sozlesme ustunden DMA/ISR veya gercek C54x task/callback baglamini tanimlamak.

## Iteration 052 - Runtime Init Parametrelerini Acma

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `agc_fixed_runtime_init(...)` arayuzune `target_rms_q15` ve `max_gain_q20` eklendi.
- Init sirasinda default config olusturulup bu iki alan disaridan override edilir hale geldi.

### Ozet
- Fixed runtime artik iki temel kullanici parametresini init aninda disaridan alabiliyor.
- Float kullanmadan, fixed-point tipleriyle kontrol korunuyor.

### Sonraki Adim
- Gerekirse ayni yaklasimla `gate_threshold_q15` gibi ikinci seviye parametrelerin init/config yolundan nasil acilacagini belirlemek.

## Iteration 053 - Runtime Init'i Kullanici Dostu Hale Getirme

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `agc_fixed_runtime_init(...)` arayuzu `Q15/Q20` yerine kullanici dostu tam sayi parametreler alacak sekilde guncellendi.
- Yeni girisler:
  - `target_rms_percent`
  - `max_gain_x100`
- Init icinde bu degerler fixed-point formata cevriliyor.

### Ozet
- Cagiran tarafin `Q15` ve `Q12.20` ayrintilarini bilmesi artik gerekmiyor.
- Ic fixed-point tasarim korunurken, ust arayuz daha okunur hale geldi.

### Sonraki Adim
- Gerekirse `gate_threshold` gibi diger parametreler icin de benzer kullanici dostu giris modeli tanimlamak.

## Iteration 054 - Max Gain'i dB Arayuzune Tasima

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `agc_fixed_runtime_init(...)` icindeki `max_gain` girisi `max_gain_db` olarak sadeleştirildi.
- Ic tarafta tam sayi tabanli dB->Q20 donusumu korunarak kullanici arayuzu basitleştirildi.

### Ozet
- Cagiran taraf artik dogrudan dB cinsinden tam sayi deger verebiliyor.
- Ic fixed-point yol korunuyor; runtime float kullanilmiyor.

### Sonraki Adim
- Gerekirse benzer kullanici dostu dB/yuzde arayuzleri diger parametrelere de acmak.

## Iteration 055 - Frame Window Buffered Runtime Arayuzu

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `AgcFixedRuntime` icine kismi input orneklerini biriktiren ic pencere tamponu eklendi.
- `agc_fixed_runtime_buffered_samples(...)` ile buffered doluluk izlenebilir hale geldi.
- `agc_fixed_runtime_process_buffered(...)` eklendi.
- Yeni arayuz, parca input bloklarini toplayip tam frame oldugunda mevcut fixed pipeline'i calistirir.

### Ozet
- Algoritma davranisi degismedi.
- Bu adim, entegrasyon tarafinda tam frame zorunlulugunu yumusatti.
- ISR/DMA benzeri parca veri akislari icin daha kullanisli bir runtime katmani saglandi.

### Sonraki Adim
- Gerekirse buffered API icin kucuk bir kullanim ornegi veya test harness eklemek.

## Iteration 056 - Sample-In Sample-Out Runtime Arayuzu

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- `AgcFixedRuntime` icine output pencere tamponu eklendi.
- `agc_fixed_runtime_process_sample(...)` eklendi.
- Yeni arayuz, her cagrida bir input sample alip bir output sample verirken icerde frame bazli AGC cekirdigini korur.

### Ozet
- Algoritma davranisi degismedi.
- Bu adim, sample tabanli entegrasyon isteyen cagiran taraflar icin kullanimi kolaylastirdi.
- Bunun karsiliginda yaklasik bir frame gecikme bilincli olarak kabul edildi.

### Sonraki Adim
- Gerekirse sample API icin kucuk bir kullanim ornegi veya hafif bir regresyon testi eklemek.

## Iteration 057 - C54x Kaynak ve Cycle Tahmini

### Tarih
- 2026-03-30

### Bu iterasyonda yapilanlar
- Fixed baseline icin RAM, latency ve cycle tahmini ayri teknik notta toplandi.
- `agc_fixed_runtime_process_sample(...)` icin ortalama ve en kotu durum maliyetleri kabaca hesaplandi.
- Bu hesaplarin exact degil, statik tahmin oldugu acikca ayristirildi.

### Uretilen Dokuman
- `docs/agc-c54x-resource-estimate.md`

### Ozet
- Sample API icin etkin gecikme yaklasik bir frame yani `4 ms` olarak kaydedildi.
- Runtime RAM maliyeti icin `66 + 10N byte` yaklasimi not edildi.
- `16 kHz` icin ortalama sample maliyeti yaklasik `50-67 cycle/sample` bandinda tutuldu.

### Sonraki Adim
- Gercek C54x build alindiginda detector ve square-root maliyetini assembly/listing uzerinden dogrulamak.

## Iteration 058 - Windows Canli Loopback Test Araci

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- `tools/agc_live_loopback_winmm.c` eklendi.
- Windows `waveIn/waveOut` tabanli canli loopback test araci olusturuldu.
- Arac, default capture cihazindan aldigi sesi mevcut fixed AGC cekirdeginden gecirip default output cihazina verir.
- Kullanim ve derleme notu ayri dokumanda sabitlendi.

### Uretilen Dokuman
- `docs/agc-live-loopback-test.md`

### Ozet
- Algoritma degistirilmedi.
- Kulaklik ile gercek zamanli dinleme testi yapmak icin dusuk riskli bir yerel arac eklendi.
- Bu arac DSP entegrasyonunun kendisi degil, masaustu dogrulama yardimcisidir.

### Sonraki Adim
- Gerekirse arac icine cihaz secimi, meter veya WAV kayit secenegi eklemek.

## Iteration 059 - Canli Loopback Aracini Genisletme

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- Canli loopback aracina cihaz listeleme ve secme eklendi.
- Console uzerine input/output peak meter eklendi.
- Input ve output akisini ayni anda WAV olarak kaydetme secenegi eklendi.

### Ozet
- Algoritma degismedi.
- Canli test araci artik sadece dinleme degil, gozlemleme ve kayit alma amacina da hizmet ediyor.
- Masaustu tuning ve kulakla karsilastirma icin daha kullanisli hale geldi.

### Sonraki Adim
- Gerekirse araca daha net cihaz isimleri, meter averaging veya otomatik dosya adlandirma eklemek.

## Iteration 060 - Browser Tabanli AGC Test UI

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- `ui/agc-live-ui.html`, `ui/agc-live-ui.css` ve `ui/agc-live-ui.js` eklendi.
- Tarayicida calisan canli AGC test arayuzu olusturuldu.
- Parametre secimi, input/output meter, waveform goruntuleme ve signal flow degerleri eklendi.
- Opsiyonel compressor denemesi de ayni UI icinde acilip kapatilabilir hale getirildi.

### Uretilen Dokuman
- `docs/agc-web-ui-test.md`

### Ozet
- Bu adim C tabanli test aracina alternatif, daha zengin bir masaustu deney ortami sagladi.
- UI davranis olarak mevcut AGC blok mantigini takip eder ama DSP fixed baseline ile bit-exact degildir.
- Hedefi hizli tuning, kulakla degerlendirme ve gorunsel gozlemdir.

### Sonraki Adim
- Gerekirse UI icine preset kaydetme, parametre export veya WAV import eklemek.

## Iteration 061 - Waveform Boyut Kontrolu

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- Browser UI icine waveform width ve height slider'lari eklendi.
- Input ve output scope canvas boyutu canli olarak degistirilebilir hale geldi.
- Scope basligina aktif boyut bilgisi eklendi.

### Ozet
- Algoritma degismedi.
- Bu adim, burst ve patlama gibi durumlari daha rahat gorsel inceleme amaciyla yapildi.
- Kullanici artik waveform gorunumunu daha buyuk alana yayarak detayli inceleyebilir.

### Sonraki Adim
- Gerekirse zaman ekseni zoom veya frozen capture ozelligi eklemek.

## Iteration 062 - Coklu Zaman Pencereli Waveform Gorunumu

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- Input ve output waveform alanlari alt alta duzenlendi.
- History tabanli waveform cizimi eklendi.
- `10 ms`, `100 ms`, `500 ms`, `1 s` ve `2 s` zaman pencereleri ayni anda gorulebilir hale geldi.
- Scope width kontrolu butun waveform canvas'larinda gercek genislik degisimi yapacak sekilde duzeltildi.

### Ozet
- Algoritma degismedi.
- Bu adim, burst ve patlama davranisini hem kisa hem uzun olcekte ayni anda incelemeyi kolaylastirdi.
- Input/output karsilastirmasi artik daha okunur hale geldi.

### Sonraki Adim
- Gerekirse frozen capture, trigger veya belirli bir burst anini dondurma ozelligi eklemek.

## Iteration 064 - Tek Aktif Waveform Penceresi

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- Birden cok waveform penceresi ayni anda gosterilmek yerine tek aktif pencere modeline gecildi.
- `10 ms`, `100 ms`, `500 ms`, `1 s`, `2 s` secimi combo box ile yapilabilir hale geldi.
- Input ve output waveform gorunumleri alt alta korunurken secili pencere uzerinden cizim yapacak sekilde sadeleştirildi.

### Ozet
- Algoritma degismedi.
- Bu adim, ekran kalabaligini azaltti ve secili zaman olcegine daha rahat odaklanmayi sagladi.
- Scope width sorunu da bu sade yapı ile daha tutarli hale geldi.

### Sonraki Adim
- Gerekirse freeze veya burst trigger ozelligi eklemek.

## Iteration 066 - Browser UI Kayit Dugmesi

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- UI toolbar'ina `Record` dugmesi eklendi.
- Record aktifken input ve output PCM bloklari ayri ayri biriktirilir hale geldi.
- Kayit durduruldugunda input ve output ayri WAV dosyalari olarak indirilecek sekilde export eklendi.

### Ozet
- Algoritma degismedi.
- Kullanici artik canli test sirasinda ayni anda hem input'u hem output'u kaydedebilir.
- Bu, AGC faydasini sonradan kulakla ve editor uzerinde karsilastirmayi kolaylastirir.

### Sonraki Adim
- Gerekirse browser UI icinde kayit suresi, dosya adi veya otomatik tekli zip/export duzenlemek.

## Iteration 067 - test_wav Batch Kosusu

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- `tools/agc_batch_wav_fixed.c` eklendi.
- `test_wav` altindaki mono PCM16 WAV dosyalari fixed AGC ile batch olarak islenebilir hale getirildi.
- Verilen parametrelerle toplu kosu yapildi ve output dosyalari `test_wav/agc_output` altina yazildi.
- Sonuclar ayri dokumanda toplandi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results.md`

### Ozet
- `max_gain_db = 18`, `target_rms_percent = 80`, `gate_threshold = 5` ile kosu yapildi.
- Bu ayar seti ozellikle daha sicak dosyalarda peak protector'a belirgin bicimde yukleniyor.
- Limiter devreye girmeden once peak protector bircok frame'de koruma yapiyor.

### Sonraki Adim
- Gerekirse ayni seti daha muhafazakar hedef RMS ile tekrar kosup karsilastirmak.

## Iteration 068 - Surumu Dondurma ve Artefact Duzeni

### Tarih
- 2026-04-03

### Bu iterasyonda yapilanlar
- Koku dağinik duran `.o` dosyalari `artifacts/objects` altina tasindi.
- Koku dağinik duran reference ve compare WAV artefact'lari `artifacts/audio/reference` ve `artifacts/audio/compare` altina tasindi.
- Artefact klasor yapisi icin `artifacts/README.md` eklendi.
- Gelecekte ayni tip dosyalarin tekrar kokte birikmemesi icin root `.gitignore` eklendi.

### Ozet
- Algoritma degismedi.
- Bu adim, mevcut iyi durumu dondurmadan once proje yapisini daha duzenli hale getirmek icin yapildi.
- test veri seti ve ona ait batch output'lar `test_wav` altinda tutulmaya devam etti.

### Sonraki Adim
- Bu duzenli durum ustunden commit ve tag ile surumu sabitlemek.

## Iteration 069 - High RMS Mimari Karari ve Ilk Reference Uygulamasi

### Tarih
- 2026-04-03

### Bu iterasyonda yapilanlar
- Yuksek RMS hedeflerinde lineerlik bozulmasini aciklayan tasarim karari dokumante edildi.
- Reference zincire `peak-headroom aware gain cap` mantigi eklendi.
- Reference zincire `soft-knee compressor` blogu eklendi.
- Metrics ve state yapilari yeni bloklari yansitacak sekilde guncellendi.

### Uretilen Dokuman
- `docs/agc-high-rms-architecture-decision.md`

### Ozet
- Dondurulan baseline korunuyor.
- Yeni zincir, `%80 RMS` gibi agresif hedeflerde peak bolgesindeki bozulmayi azaltmak icin reference tarafta denenmeye baslandi.
- Fixed baseline bu iterasyonda degistirilmedi.

### Sonraki Adim
- Reference derleme ve kontrollu test kosulari ile yeni zincirin davranisini incelemek.

## Iteration 070 - Test WAV Seti Icin V2 Batch Sonuclari

### Tarih
- 2026-04-03

### Bu iterasyonda yapilanlar
- Reference tarafta yeni `high-rms v2` zinciri icin ayri batch arac eklendi.
- `test_wav` altindaki dort input dosya `%80 RMS`, `18 dB max gain`, `%5 gate threshold` ile tekrar islendi.
- Yeni ciktilar eski dosyalarla karismamasi icin `agc_output_v2` altina ve `_v2` sonekli adlarla yazildi.
- Sonuclar ayri bir batch raporunda toplandi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v2.md`

### Ozet
- `v2` zincirinde peak protector aktivasyonu sifira indi.
- Koruma yukunun ana kismi `headroom_limited` ve `compressor` bloklarina kaydi.
- Peak lineerligini iyilestirme yonunde dogru hareket edildi, ancak efektif RMS seviyesi onceki agresif batch'e gore daha muhafazakar kaldi.

### Sonraki Adim
- Dinleme ve waveform incelemesiyle `v2` ciktilarin subjektif etkisini degerlendirmek.

## Iteration 071 - Ortalama %80 Hedefi Icin V3 Zinciri

### Tarih
- 2026-04-03

### Bu iterasyonda yapilanlar
- Erken `peak headroom cap` kapatildi.
- `frame-level peak protector` kapatildi.
- Yuku daha cok `soft-knee compressor + final limiter` tarafina veren yeni reference davranisi denendi.
- `test_wav` seti bu kez `v3` cikti adlariyla tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v3.md`

### Ozet
- `v3`, `v2`'ye gore ortalama cikis seviyesini belirgin yukseltti.
- Peak'ler `0.995` civarina kadar cikabildi.
- Peak protector tamamen devre disi kalirken, az sayida limiter aktivasyonu goruldu.

### Sonraki Adim
- Final limiter'i daha yumusak hale getirmenin gerekli olup olmadigini waveform ve dinleme uzerinden degerlendirmek.

## Iteration 072 - Soft Final Limiter Ile V4 Sonuclari

### Tarih
- 2026-04-03

### Bu iterasyonda yapilanlar
- Hard final limiter, soft saturation benzeri bir egriyle degistirildi.
- `test_wav` seti bu kez `v4` cikti adlariyla tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v4.md`

### Ozet
- `v4`, sayisal olarak `v3`e yakin kaldi.
- En kritik fark, limiter devreye girdiginde sert clamp yerine yumusak sinirlama uygulanmasi oldu.
- Bu nedenle asil kazanc waveform sekli ve subjektif dinleme tarafinda bekleniyor.

### Sonraki Adim
- `v3` ve `v4` ciktilari waveform/duyusal olarak karsilastirip soft limiter egrisinin yeterli olup olmadigini kararlastirmak.

## Iteration 073 - Multi-Mode Target Level Karari

### Tarih
- 2026-04-03

### Bu iterasyonda yapilanlar
- `AM` ve `DIGITAL` modlar icin ortak hedef seviye tanimi netlestirildi.
- `target level` kavraminin iki modda da ayni kalmasi kabul edildi.
- Hedefin olcumu olarak `RMS + gate ile konusma aktif frame'leri` secildi.
- Mod farklarinin hedef anlaminda degil, uygulama agirliklarinda ortaya cikmasi kararlastirildi.

### Uretilen Dokuman
- `docs/agc-multi-mode-target-level-decision.md`

### Ozet
- Proje artik tek cekirdek, cok modlu AGC olarak ele alinacak.
- `AM` ve `DIGITAL` farki, ayni hedefe farkli parametre/profil ile ulasma seklinde modellenecek.
- Bu karar, sonraki algoritma degisiklikleri icin daha saglam bir zemin sagladi.

### Sonraki Adim
- Bu hedef tanimina uygun olarak peak-aware gain planning ve compressor rolunu mode profilleri ekseninde yeniden tasarlamak.

## Iteration 074 - AM Preset'in Sabitlenmesi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Ortak cekirdekte kullanilacak ilk resmi `AM` preset tanimlandi.
- `reference/agc_config` icine `AgcMode` ve preset secim yapisi eklendi.
- Geriye uyumluluk icin mevcut `default` davranisi `AM` preset'e baglandi.
- Batch reference araci mode parametresi kabul edecek sekilde guncellendi.

### Uretilen Dokuman
- `docs/agc-am-preset.md`

### Ozet
- `AM` artik soyut fikir degil, secilebilir bir profil haline geldi.
- Bir sonraki `DIGITAL` preset icin de ayni cati hazirlandi.

### Sonraki Adim
- `AM` preset davranisini reference testleri uzerinde olgunlastirmak ve gerekirse limiter/kompresor dengesini bu profil icinde yeniden ince ayarlamak.

## Iteration 075 - AM Preset Icinde `-1 dBFS` Compressor Esigi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `AM` preset icinde `compressor_threshold_dbfs = -1` yapildi.
- Ortamda `.exe` kosma engeli oldugu icin ayni mantigi izleyen Python batch araci eklendi.
- `test_wav` seti bu yeni ayarla `v5` ciktilari olarak tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v5.md`

### Ozet
- Ortalama seviye bir miktar yukselirken limiter yukunde ciddi artis goruldu.
- Bu sonuc, `-1 dBFS` threshold'un AM preset icin fazla gec frenledigini gosteriyor.

### Sonraki Adim
- AM preset'te compressor threshold/ratio dengesini limiter yukunu azaltacak sekilde yeniden incelemek.

## Iteration 076 - AM Preset'te `-2 dBFS` Emegine Donus

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `AM` preset icindeki `compressor_threshold_dbfs`, `-1 dBFS` denemesinden sonra tekrar `-2 dBFS` degerine alindi.

### Ozet
- `-1 dBFS` denemesi ortalama seviyeyi biraz yukseltti, ancak limiter yukunu ciddi bicimde artirdi.
- Bu nedenle AM preset icin daha dengeli onceki esige geri donuldu.

### Sonraki Adim
- Gerekirse `-2 dBFS` temelinde `ratio` ve `knee` tarafini yeniden incelemek.

## Iteration 077 - Yeni AM Reference Zincirini Dondurma

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Yeni reference AGC zinciri icin ayri bir freeze dokumani eklendi.
- `AM` preset, `target level` karari ve batch sonuc dokumanlari tek freeze catisi altinda toplandi.
- Son kabul edilen `AM` compressor esigi tekrar `-2 dBFS` durumunda birakildi.

### Uretilen Dokuman
- `docs/agc-am-reference-freeze.md`

### Ozet
- Eski baseline korunurken, yeni cok-modlu reference yon de ayri bir dondurma noktasi olarak sabitlendi.
- Bundan sonraki uygulama adimlari bu freeze noktasini temel alabilir.

### Sonraki Adim
- Bu dondurulan reference davranisindan hareketle uygulama/DSP uyarlama adimlarini baslatmak.

## Iteration 078 - Control Sidechain HPF Ilk Denemesi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Detector yoluna `300 Hz` control sidechain HPF eklendi.
- Bu davranis `AM` preset icinde etkinlestirildi.
- `test_wav` seti bu yeni davranisla `v6` olarak tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v6.md`

### Ozet
- Teori dusuk frekans baskisini azaltmakti.
- Ancak ilk denemede AGC/compressor daha fazla gain verip final limiter yukunu belirgin sekilde artirdi.
- Bu nedenle control sidechain weighting fikri reddedilmedi, ancak ilk uygulama bicimi basarili sayilmadi.

### Sonraki Adim
- Sidechain weighting'in tamamen kaldirilmasi, yumusatilmmasi veya farkli weighting yapilarinin dusunulmesi.

## Iteration 079 - Control Sidechain HPF Geri Alinmasi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Detector yoluna eklenen `300 Hz` sidechain HPF geri alindi.
- Python batch aracindaki ayni weighting davranisi da geri alindi.
- `AM` preset tekrar tam-band detector davranisina donduruldu.

### Ozet
- Ilk sidechain HPF denemesi limiter yukunu belirgin bicimde artirdigi icin korunmadi.
- Bu fikir tamamen reddedilmedi; ancak bu ilk uygulama bicimiyle freeze sonrasina tasinmamasina karar verildi.

### Sonraki Adim
- Gerekirse daha hafif weighting veya farkli detector stratejileri daha kontrollu bir iterasyonda ele alinabilir.

## Iteration 080 - CFAGC Ilk Uygulamasi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `CFAGC` adi altinda crest-factor-aware gain planning karari dokumante edildi.
- `target_peak` ve `limiter_threshold` hedef formulleri kod ve batch aracina islendi.
- `cf_low_db = 9`, `cf_high_db = 15`, `cf_smoothing_ms = 40` ile ilk deneme yapildi.
- Compressor `AM` modunda pasif hale getirildi ve limiter sayaci ana veri olarak izlendi.
- `test_wav` seti `v7` ciktilari ile tekrar islendi.

### Uretilen Dokuman
- `docs/cfagc-decision.md`
- `docs/agc-test-wav-batch-results-v7.md`

### Ozet
- Ilk CFAGC denemesinde peak-aware gain planning devreye girdi.
- Ancak compressor olmadan limiter yukunde beklenen dusus gorulmedi.
- Bu nedenle bir sonraki adim, CFAGC blend fonksiyonunu veya peak agirligini yeniden incelemek olacak.

## Iteration 081 - CFAGC Esiklerini Veriyle Daraltma

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Test setindeki aktif-frame crest-factor dagilimi analiz edildi.
- Limiter'a dusen frame'lerin crest-factor dagilimi ayrica incelendi.
- Bu veriye gore `cf_low_db / cf_high_db` araligi `6.0 / 10.5` olarak guncellendi.
- `test_wav` seti `v8` ciktilari ile tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v8.md`

### Ozet
- Yeni esikler CFAGC'yi daha erken devreye soktu.
- Bazi sicak dosyalarda limiter yukunde iyilesme saglandi.
- Ancak bunun bedeli RMS seviyesinin dusmesi oldu.

### Sonraki Adim
- CFAGC ile limiter yukunu azaltirken RMS'i daha iyi koruyacak yeni blend veya target_peak stratejisini incelemek.

## Iteration 082 - CFAGC Convex Blend Denemesi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- CFAGC lineer blend yerine `convex (w^2)` blend ile tekrar denendi.
- `test_wav` seti `v9` ciktilari ile tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v9.md`

### Ozet
- `v8`e gore RMS bir miktar geri kazanildi.
- Limiter yukunde kismi geri artis olsa da `v7` kadar kotu seviyeye donulmedi.
- Bu sonuc, blend fonksiyonunu sekillendirmenin anlamli bir tuning ekseni oldugunu gosterdi.

### Sonraki Adim
- `v9`un yeterli ara nokta olup olmadigina karar vermek veya `target_peak` tarafini bir sonraki tuning ekseni olarak ele almak.

## Iteration 083 - CFAGC Log-Gain Blend Denemesi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `CFAGC` blend ayni `w^2` agirligi korunarak lineer gain alanindan log-gain alanina tasindi.
- `target_peak` ve `limiter_threshold` formullerine dokunulmadan yalniz gain law daha korumaci hale getirildi.
- `test_wav` seti `v10` ciktilari ile tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v10.md`

### Ozet
- Limiter yukunde kucuk ama tutarli bir dusus elde edildi.
- RMS seviyesi `v9`a gore buyuk olcude korundu.
- Bu sonuc, `target_peak`e gecmeden once gain law tarafinda hala kucuk ama anlamli kazanclar alinabildigini gosterdi.

### Sonraki Adim
- `v10` waveform davranisini inceleyip, gerekirse son kez gain law tarafinda bir adim daha atmak; ancak iyilesme doygunlasirsa `target_peak` eksenine gecmek.

## Iteration 084 - CFAGC Target Peak Daraltma Denemesi

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `target_peak` formulu `target + (1 - target) * 4/5` yerine `target + (1 - target) * 2/5` olarak daraltildi.
- `gain law` ve `limiter_threshold` ayni birakilarak yalniz peak hedefinin etkisi olculdu.
- `test_wav` seti `v11` ciktilari ile tekrar islendi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v11.md`

### Ozet
- `target_peak`in daha muhafazakar hale getirilmesi limiter yukunu yalniz kucuk bir miktar azaltti.
- RMS seviyesi de benzer olcude azaldi.
- Bu, sorunun ana ekseninin yalniz peak hedefi degil, genel gain tasima davraniÅŸi ve final koruma etkileÅŸimi oldugunu gostermeye basladi.

### Sonraki Adim
- `v11` waveform ile `v10`/`v9` davranisini birlikte yorumlayip, compressor'un yeniden kontrollu sekilde devreye alinmasi gerekip gerekmedigine karar vermek.

## Iteration 085 - Signal Debugger Araci

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- WAV girdisini frame-frame analiz edip karar izini JSON'a dokebilen `tools/agc_debug_export.py` eklendi.
- Bu JSON'u offline inceleyen yeni UI hazirlandi:
  - `ui/agc-debugger.html`
  - `ui/agc-debugger.css`
  - `ui/agc-debugger.js`
- Whole signal, decision timeline, secili frame waveform ve karar ozeti panelleri eklendi.

### Uretilen Dokuman
- `docs/agc-signal-debugger.md`

### Ozet
- Bu arac, AGC davranisini ses dosyasi bazinda yazilim debug eder gibi incelemeyi sagliyor.
- Limiter, headroom limit, crest factor ve gain kararlarini tek ekranda izlemek artik mumkun.

### Sonraki Adim
- Gercek sorunlu dosyalar icin debug trace ureterek `CFAGC`, limiter ve gerekirse compressor davranisini frame bazinda birlikte yorumlamak.

## Iteration 086 - WAV-First Signal Debugger

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- JSON yerine dogrudan input WAV secerek calisan yeni debugger arayuzu eklendi.
- Tarayici icinde:
  - WAV decode
  - reference-benzeri frame processing
  - karar izi uretimi
  - input/output waveform ve frame detay gosterimi
  tek adimda yapilir hale geldi.
- Output WAV'i dogrudan indirme secenegi eklendi.

### Uretilen Dosyalar
- `ui/agc-wav-debugger.html`
- `ui/agc-wav-debugger.js`

### Ozet
- Artik sinyali incelemek icin once JSON export uretmek zorunlu degil.
- En hizli debug yolu, WAV secip ayni arayuz icinde incelemek.

### Sonraki Adim
- Sorunlu WAV dosyalari uzerinde bu yeni debugger ile limiter, CFAGC ve waveform lineerligini frame bazinda birlikte yorumlamak.

## Iteration 087 - Crest Smoothing Activity Floor

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- CFAGC crest smoothing guncellemesi icin `rms_activity_floor = 0.05` eklendi.
- `smoothed crest factor` artik yalniz `gate_open` ve `smoothed_rms > 0.05` oldugunda guncelleniyor.
- Reference C kodu, Python batch/export araclari ve WAV debugger ayni mantiga hizalandi.
- Debugger icine `CF Update` bilgisi eklendi.

### Ozet
- Bu adim, dusuk enerjili gap ve kuyruk frame'lerinin crest smoothing state'ini kirletmesini azaltmak icin atildi.
- Sadece `gate_open` kosulu yerine, enerji tabanli ikinci bir guvenilirlik kapisi eklenmis oldu.

### Sonraki Adim
- Bu yeni crest smoothing kuralinin limiter yukune ve peak lineerligine etkisini test WAV'lari uzerinde birlikte incelemek.

## Iteration 089 - Asimetrik Crest Smoothing

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- CFAGC crest smoothing icin tek zaman sabiti yerine asimetrik zaman sabitleri tanimlandi.
- `cf_rise_ms = 8`, `cf_fall_ms = 40` ile crest factor yukselirken hizli, duserken daha yavas takip uygulanmaya baslandi.
- Reference C, Python batch/export ve WAV debugger ayni mantiga hizalandi.

### Ozet
- Bu adim, ani peak-dominant hecelerde `smoothed crest factor`in gercek crest factor'uun cok gerisinde kalmasi problemini azaltmak icin atildi.
- Ama limiter yukundeki gercek etkisi test kosusuyla dogrulanmali.

### Sonraki Adim
- `test_wav` seti ile `v13` batch kosusu alip limiter sayaci, RMS ve waveform davranisini onceki surumle karsilastirmak.

## Iteration 090 - Asimetrik Crest Smoothing Batch Sonuclari

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `cf_rise_ms = 8`, `cf_fall_ms = 40` ile asimetrik crest smoothing davranisi `test_wav` seti uzerinde olculdu.
- Sonuclar `v13` ciktilari ve batch raporu ile kaydedildi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v13.md`

### Ozet
- Sicak dosyalarda limiter sayisi belirgin bicimde dustu.
- Buna karsilik headroom-limited frame sayisi cok artti ve RMS belirgin azaldi.
- Bu, asimetrik crest smoothing yonunun dogru oldugunu ama ilk ayarin fazla korumaci kaldigini gosteriyor.

### Sonraki Adim
- `cf_rise_ms / cf_fall_ms` dengesini biraz yumusatmak veya blend agirligini kismen geri acmak.

## Iteration 091 - Headroom-Limited Frame'lerde Peak-Biased Blend

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `headroom_limited` frame'lerde CFAGC blend agirligi daha sert peak-biased hale getirildi.
- `desired_gain`in `gain_peak`e daha cok yaklasmasi hedeflendi.
- Sonuclar `v14` batch kosusu ile olculdu.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v14.md`

### Ozet
- Beklenen yonde, limiter sayisi belirgin bicimde azaldi.
- Ancak RMS seviyesi AM hedefleri icin fazla duserek bu ilk peak-biased agirligin fazla sert oldugunu gosterdi.

### Sonraki Adim
- Peak-biased weight'i yumusatmak veya yalniz daha yuksek crest-factor bolgelerinde devreye girecek kosullu bir karisima gecmek.

## Iteration 092 - Peak-Biased Blend'i Yumusatma

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Headroom-limited frame'lerde kullanilan peak-biased agirlik `1 - (1 - w)^4` yerine `1 - (1 - w)^3` yapildi.
- Sonuclar `v15` batch kosusu ile olculdu.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v15.md`

### Ozet
- RMS bir miktar geri kazanildi.
- Limiter yuku de tekrar bir miktar artti.
- Bu da peak-biased blend ailesinin etkili oldugunu ama hala fazla agresif kaldigini gosteriyor.

### Sonraki Adim
- Weight'i daha da yumusatmak yerine yalniz daha yuksek crest-factor bolgelerinde etkinlestirilen kosullu bir peak-biased blend denemek daha verimli olabilir.

## Iteration 093 - Peak-Biased Blend Yumusatmasini Geri Alma

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Son peak-biased blend yumusatmasi geri alindi.
- CFAGC agirligi tekrar `v14`teki daha sert `1 - (1 - w)^4` davranisina donduruldu.
- Debugger ve Python batch araci da ayni duruma hizalandi.

### Ozet
- `v15`teki yumusatma sinyali biraz toparlasa da limiter yukunu yeniden arttirdigi icin korunmadi.
- Calisma noktasi tekrar `v14` davranisina geri alindi.

### Sonraki Adim
- Sonraki arayis, tum headroom-limited frame'lerde degil yalniz daha yuksek crest-factor bolgelerinde etkinlesen kosullu peak bias yoni olacak.

## Iteration 094 - Kosullu Peak-Biased Blend

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- Peak-biased blend tum `headroom-limited` frame'lerde tam guclenmek yerine, yalniz daha yuksek crest-factor bandinda guclenecek sekilde kosullandi.
- `peak_bias_low_db = 8.5`, `peak_bias_high_db = 10.5` ile yeni ara davranis denendi.
- Sonuclar `v16` batch kosusu ile olculdu.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v16.md`

### Ozet
- `v14`e gore RMS anlamli bicimde geri kazanildi.
- Limiter yukunde de buna paralel bir geri artis oldu.
- Sonuc, `v13` ile `v14` arasinda daha dengeli bir ara nokta verdi.

### Sonraki Adim
- Bu dengenin dinleme ve waveform tarafinda tatmin edici olup olmadigini kontrol etmek; gerekirse kosullu peak bias bandi veya threshold'lari uzerinden ince tuning yapmak.

## Iteration 095 - Release Suresini 100 ms'e Cekme

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `AM` preset icinde `release_ms = 100` yapildi.
- Reference C, Python batch araci ve WAV debugger ayni degere hizalandi.
- Sonuclar `v17` batch kosusu ile olculdu.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v17.md`

### Ozet
- Daha hizli `release`, `v14`e gore RMS'i belirgin bicimde yukari tasidi.
- Limiter yukunde de buna paralel belirgin bir artis olustu.
- Sonuc, `v14`ten daha sicak ama daha zorlanan bir ara aday verdi.

### Sonraki Adim
- Dinleme ve debugger incelemesiyle `v14` ile `v17` arasindaki pratik farki degerlendirmek; gerekirse `release_ms` icin arada bir deger denemek.

## Iteration 096 - Release Suresini 150 ms'e Alma

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `release_ms = 150` denemesi yapildi.
- Reference C, Python batch araci ve WAV debugger ayni degere hizalandi.
- Sonuclar `v18` batch kosusu ile olculdu.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v18.md`

### Ozet
- `150 ms`, `v14` ile `v17` arasinda beklenen orta noktayi verdi.
- RMS toparlanmasi saglarken limiter yukundeki artis da `100 ms` kadar yuksek olmadi.
- Bu nedenle `150 ms` su an dengeli bir aday olarak gorunuyor.

### Sonraki Adim
- Dinleme ve debugger uzerinden `v14` ile `v18` karsilastirmasi yapip hangi yapiyi yeni calisma tabani yapacagimiza karar vermek.

## Iteration 088 - Crest Smoothing Activity Floor Batch Sonuclari

### Tarih
- 2026-04-04

### Bu iterasyonda yapilanlar
- `rms_activity_floor = 0.05` kuralinin etkisi `test_wav` seti uzerinde tekrar olculdu.
- Sonuclar `v12` ciktilari ve ayri batch raporu ile kaydedildi.

### Uretilen Dokuman
- `docs/agc-test-wav-batch-results-v12.md`

### Ozet
- Crest smoothing update daha secici hale gelince `headroom_limited_frames` belirgin azaldi.
- Buna karsilik limiter yukunde anlamli bir rahatlama gorulmedi; bazi dosyalarda limiter sayisi hafif artti.
- RMS seviyesi ise kucuk bir miktar yukselme eglimi gosterdi.

### Sonraki Adim
- Crest update kosulunun tek basina yeterli olmadigi goruldugu icin, bir sonraki aday eksen `crest rise/fall asimetrisi` veya compressor'un kontrollu geri donusu olacaktir.

## Iteration 065 - Browser UI Stabilite Iyilestirmesi

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- Browser UI icinde `ScriptProcessorNode` blok boyu `1024` sample'a cikartildi.
- Meter ve waveform cizimi her animation frame yerine daha seyrek guncellenecek sekilde seyreltildi.

### Ozet
- Algoritma degismedi.
- Ama audio yolundaki underrun/cizim kaynakli glitch riski azaltildi.
- Bunun karsiliginda browser test yolunda latency bir miktar artti.

### Sonraki Adim
- Gerekirse daha saglam cozum olarak `AudioWorklet` tabanli ses isleme yoluna gecmek.

## Iteration 063 - UI Uzerinden AGC On/Off Karsilastirmasi

### Tarih
- 2026-04-02

### Bu iterasyonda yapilanlar
- Browser UI icine `AGC enabled` secimi eklendi.
- Kapatildiginda AGC gain yolu bypass olacak sekilde isleme guncellendi.
- Signal flow alanina AGC mode bilgisi eklendi.

### Ozet
- Bu adim, AGC'nin sagladigi farki ayni test ortami icinde dogrudan duymayi ve gormeyi kolaylastirdi.
- Peak protector ve limiter korumasi korunurken AGC gain etkisi ac/kapa yapilabilir hale geldi.

### Sonraki Adim
- Gerekirse `limiter only`, `compressor only`, `full chain` gibi daha zengin karsilastirma preset'leri eklemek.

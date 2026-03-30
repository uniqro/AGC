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

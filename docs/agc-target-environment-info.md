# AGC Target Environment Information Needs

Bu dosya, reference yaklasimdan hedef ortama gecis icin hangi ek teknik bilgilerin gerekli oldugunu toplar. Amaç, implementasyona gecmeden once eksik karar ve entegrasyon alanlarini gorunur hale getirmektir.

## 1. Amac

- Reference AGC davranisini hedef DSP ortamina tasimak icin gerekli teknik bilgileri listelemek
- Bilinenler ile eksik kalanlari ayirmak
- Porting oncesi riskleri azaltmak

## 2. Halihazirda Bilinenler

- Hedef platform: TMS320VC5416 DSP
- Islem gercek zamanli olacak
- Sinyal tipi: AM Radio konusmasi
- Kanal yapisi: mono
- Tasarim Fs-genel olacak
- Uretim yonu fixed-point uyumlu olacak
- AGC giris veri tipi: int16
- Hedef sayisal temsil: Q15
- Frame yapisi: 4 ms sabit frame
- Parametre degisimi: yalnizca init sirasinda
- DSP zinciri yonu: yuksek geciren + alcak geciren filtreleme -> AGC -> limiter -> cikis
- Clipping tercihi: ara sira clipping kabul edilebilir
- Limiter aktivasyonu: konusma patlamalarinda kisa sureli kumelenme normal kabul ediliyor
- Gate histerezisi: dahili kalacak
- Filtreleme blogu tipi: yuksek geciren + alcak geciren kombinasyonu
- Filtre gercekleme tercihi: FIR
- Filtre derece yaklasimi: HP orta, LP orta
- Yuksek geciren filtre baslangici: 250 Hz
- Alcak geciren filtre baslangici: `0.4125 * Fs`
- Ara hesap genisligi tercihi: 32-bit ile basla, overflow veya hata logu gorulurse 40-bit'e gec
- Saturation tercihi: gain ve cikista saturate, ara adimlarda dikkatli olcekleme
- Ara adim olcekleme yaklasimi: peak tabanli, sinyal seviyesine gore uyarlamali
- Uyarlamali gate/V2: simdilik sadece not
- 4 ms frame karsiliklari: 8 kHz -> 32, 16 kHz -> 64, 64 kHz -> 256 sample
- Diger hizlar icin frame kuralı: `frame_samples = Fs * 0.004` ve en yakin tam sayiya yuvarlama
- Diger hizlar: ayni kural ile desteklenecek
- Ilk algoritma ailesi: tek bant AGC + basit gate + emniyet limiter

## 3. Gerekli Ek Teknik Bilgiler

### A. Giris ve Cikis Arayuzu
- AGC'ye giren veri formati int16 olacak; Q15 ic cevrimi icin donusum stratejisi nasil yapilacak?
- Giris ve cikis tampon yapisi nasil?
- 4 ms frame yapisinda 8/16/64 kHz icin sample sayilari netlesti; farkli Fs degerlerinde ek performans kisiti olacak mi?
- AGC'nin cikisi bir sonraki bloga nasil aktarilacak?

### B. Ornekleme ve Zamanlama
- Uygulamada gercek kullanilacak ornekleme hizlari hangileri?
- Ornekleme hizi degisirse frame boyu nasil secilecek?
- Sistemin izin verdigi maksimum toplam gecikme ne kadar?
- AGC frame zamanlamasi sabit scheduler ile mi, interrupt tabanli mi isleyecek?

### C. Fixed-Point Sayisal Tasarim
- Hedef Q-format Q15; herhangi bir overflow/hata logu 40-bit'e gecis icin yeterli mi, yoksa ek dogrulama aranacak mi?
- Gain ve cikista saturation uygulanirken peak tabanli uyarlamali olceklemenin esik ve katsayi kurali ne olacak?
- Gain, envelope ve limiter icin ayri olcekleme stratejisi gerekecek mi?
- LUT, yaklasik log veya lineer alan tercihi var mi?

### D. DSP Zinciri Entegrasyonu
- AGC yuksek geciren + alcak geciren FIR filtrelemeden sonra calisacak; filtrelerin kesin tap sayilari ve pencere yaklasimi ne olacak?
- AGC'den sonra limiter ve cikis var; limiter esigi ve cikis arayuzu nasil tanimlanacak?
- Filtreleme blogunun amaci: ilgisiz frekans bilesenlerini ve AGC'yi yaniltabilecek gereksiz gurultu yukunu azaltmak. Kesim frekanslari ve tip secildi; tap sayilari ve pencere yaklasimi ne olacak?

### E. Parametre ve Kontrol Arayuzu
- Kullaniciya acilacak parametreler sistemde nasil ayarlanacak?
- Parametreler runtime degismeyecek; init sirasinda hangi yapidan yüklenecek?
- Profil bazli kullanim olacak mi?
- Varsayilan degerler nerede tutulacak?

### F. Kabul ve Dogrulama
- Kisa sureli ama gozle gorulur clipping icin net sayisal oran/esik implementasyon sirasinda test ile netlestirilecek.
- Limiter'in konusma patlamalarinda kisa sureli kumelenme davranisi icin net sayisal oran/esik implementasyon sirasinda netlestirilecek.
- Dinleme testleri hangi veri seti ile yapilacak?
- Sayisal kabul metrikleri log dosyasi, grafik veya runtime istatistigi olarak mi tutulacak?

## 4. Uygulama Sirasinda Dogrulanacak Ayrintilar

- FIR filtrelerin kesin tap sayilari ve pencere secimi
- Overflow logundan 40-bit'e gecis uygulama kuralinin pratikte ne sekilde isletilecegi
- Peak tabanli uyarlamali olceklemenin katsayi ve esik secimi
- Clipping ve limiter icin net sayisal kabul esikleri
- Farkli Fs degerlerinde performans ve islem maliyeti etkisi

## 5. Acik Riskler

- Q15 olcekleme ve int16 giris donusumu gain kararliligini dogrudan etkileyebilir.
- Gain ve cikista saturation tercihinin uyarlamali ara adim olcekleme kurallari netlesmezse tasma riski surer.
- Kisa enerji penceresi DSP uzerinde oynak davranisa yol acabilir.
- Basit gate, degisken SNR altinda yetersiz kalabilir.
- Limiter esigi sayisallasmadan cikis guvenligi eksik kalabilir.

## 6. Onerilen Sonraki Bilgi Toplama Sirasi

1. Init sirasinda parametre yukleme modelini netlestir
2. Implementasyon sirasinda FIR tap sayilarini ve pencere secimini dogrula
3. Overflow logu ve peak tabanli olcekleme davranisini gozlemle
4. Clipping ve limiter esiklerini test verisi uzerinde ayarla
5. Farkli Fs degerlerinde islem maliyetini kontrol et

## 7. Sonuc

Reference algoritmadan hedef ortama gecis icin gerekli ana kararlar buyuk oranda kapanmistir. Kalan alanlar, implementasyon sirasinda dogrulanacak tuning ve sayisal davranis ayrintilari olarak ele alinabilir.

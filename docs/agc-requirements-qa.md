# AGC Requirements Q&A

Bu dosya, proje icin toplanan gereksinimleri soru-cevap seklinde tutar.

Kurallar:
- Kesin bilgi ile varsayim ayri tutulur.
- Belirsiz alanlar `Acik Soru` olarak isaretlenir.
- Celiskiler sessizce duzeltilmez; ayrica not edilir.
- Yeni bilgi eklenirken mevcut yapi korunur.

## 1. Kesin Bilgiler

- Proje amaci: ses sinyal isleme icin uygun bir AGC algoritmasini secmek ve kontrollu bicimde implemente etmek.
- Kullanicinin AGC seviyesi: baslangic duzeyi.
- Calisma yaklasimi: once konuyu anlamak, sonra kontrollu secim ve implementasyon plani olusturmak.
- Dokumantasyon tercihi: bilgi toplama ve kararlar Markdown dosyalarinda kalici tutulacak.
- Sinyal tipi: AM Radio konusmasi.
- Kanal yapisi: tek kanal mono.
- Isleme modu: gercek zamanli.
- Hedef platform: TMS320VC5416 DSP.
- Tasarim yaklasimi: ornekleme hizindan bagimsiz dusunulecek; ilk dogrulama 8 kHz, 16 kHz ve 64 kHz uzerinde yapilacak.
- Uretim yonu: fixed-point uyumlu.
- AGC giris veri tipi: int16.
- Hedef sayisal temsil: Q15.
- Ana hedef: envelope seviyesini kullanici ayarli FS yuzdesine dengelemek.
- Hedef seviye ifadesi: kullaniciya FS yuzdesi olarak sunulacak.
- Ornek hedef seviye: full scale'in yuzde 80'i gecerli bir ornek degerdir, ancak sabit gereksinim degildir.
- Ilk blok yapisi: tek bant AGC + basit enerji/envelope gate + emniyet limiter.
- Gate yaklasimi: basit enerji esigi.
- Limiter rolu: ilk surumde emniyet blogu.
- Frame yaklasimi: 4 ms sabit frame.
- Attack baslangic araligi: 5-20 ms.
- Release baslangic araligi: 200-800 ms.
- Envelope/enerji olcum penceresi baslangic araligi: 2-10 ms.
- Gate hold baslangic araligi: 50-150 ms.
- Kullaniciya acik ana ayarlar: hedef seviye, attack, release, max gain, gate hassasiyeti/esigi, limiter esigi/siddeti.
- Parametre degisimi: yalnizca init sirasinda ayarlanacak.
- Sessizlikte gain davranisi: belirgin artis olmamali.
- Basari yaklasimi: dinleme testi agirlikli + temel guvenlik esikleri.
- Tasarim onceligi: konusma netligi once, ancak gurultu siki kontrol altinda olmali.
- Clipping tercihi: ara sira clipping kabul edilebilir.
- Limiter aktivasyonu: ara sira normal kabul ediliyor.
- Gate histerezisi: kullaniciya acik olmayacak, dahili kalacak.
- DSP zinciri yonu: temel bant sinirlama filtresi -> AGC -> limiter -> cikis.
- Filtreleme blogunun amaci: AGC'ye girmeden once ilgisiz frekans bilesenlerini, istenmeyen tasmalari ve gereksiz gurultu yukunu azaltarak daha anlamli seviye olcumu saglamak.
- Filtreleme blogu tipi: yuksek geciren + alcak geciren kombinasyonu ile temel bant sinirlama.
- Yuksek geciren filtre baslangic kesimi: 250 Hz.
- Alcak geciren filtre baslangic kesimi: `0.4125 * Fs`.
- Filtre gercekleme tercihi: FIR.
- Filtre derece yaklasimi: yuksek geciren orta dereceli, alcak geciren orta dereceli.
- AGC sonrasi zincir: limiter ve dogrudan cikis.
- Ara hesap genisligi tercihi: uygulamaya 32-bit ile baslanacak; overflow veya ilgili hata loglari gorulurse yazilimda 40-bit'e gecilecek.
- Saturation tercihi: gain ve cikista saturate, ara adimlarda dikkatli olcekleme.
- Ara adim olcekleme yaklasimi: peak tabanli, sinyal seviyesine gore uyarlamali olcekleme.
- Clipping tercihi ayrintisi: kisa sureli ama gozle gorulur seviyede clipping kabul edilebilir, ancak test sonucuna gore degisebilir.
- Limiter davranisi ayrintisi: konusma patlamalarinda kisa sureli kumelenme seklinde devreye girmesi normal kabul ediliyor.
- Uyarlamali gate/V2: simdilik sadece not olarak tutulacak, tetik kosulu tanimli degil.
- 4 ms frame'in ornekleme hizlarina gore karsiligi: 8 kHz icin 32 sample, 16 kHz icin 64 sample, 64 kHz icin 256 sample.
- 8/16/64 kHz disindaki hizlarda frame uyarlama kuralı: `frame_samples = Fs * 0.004` ve en yakin tam sayiya yuvarlama.
- 8/16/64 kHz disindaki hizlar: ayni kural ile desteklenecek.

## 2. Varsayimlar

- Konusma bitisinde yavas release + kisa hold uygun baslangic davranisi kabul edilecek.
- Uyarlamali gate ilk surumde zorunlu degil.
- Ilk tuning koruyucu baslangic araliklariyla yapilacak; tek parametre seti dayatilmayacak.
- Zaman sabitleri ornekleme hizina gore olceklenecek.

## 3. Uygulama Sirasinda Dogrulanacak Ayrintilar

- Clipping icin net sayisal oran/esik uygulama ve test sirasinda ayarlanacak.
- Limiter'in konusma patlamalarinda kisa sureli kumelenmesi icin net sayisal kabul esigi test sirasinda netlestirilecek.
- Uyarlamali gate/V2 simdilik sadece not olarak kalacak; ilk implementasyonda zorunlu degil.
- FIR filtrelerin kesin tap sayilari ve pencere secimi hedef DSP maliyeti ve test sonuclarina gore netlestirilecek.
- Overflow logundan 40-bit'e gecis uygulama ayrintisi implementasyon sirasinda dogrulanacak.
- Peak tabanli uyarlamali ara adim olceklemenin tam katsayi ve esik kurallari implementasyon tuning asamasinda netlestirilecek.

## 4. Celiskiler ve Tasarim Gerilimleri

### C1. Ornekleme hizi karari evrimi
- Onceki gorus: 64 kHz bu proje icin default secim olabilir.
- Guncel karar: Tasarim Fs-genel olacak; ilk dogrulama 8/16/64 kHz uzerinde yapilacak.
- Not: Bu degisim sessizce duzeltilmedi; coklu proje kullanimi ihtiyaci nedeniyle tarihceli olarak kaydedildi.

### C2. Guvenlik odakli cizgi ile daha agresif son tercihler
- Onceki cizgi: siki max gain, limiter emniyet blogu, guvenlik esikleri oncelikli.
- Sonraki tercih: limiter'in ara sira devreye girmesi normal kabul edildi ve nadir clipping toleransi belirtildi.
- Not: Bu durum, tamamen konservatif guvenlikten ziyade konusma netligi lehine kontrollu risk kabulune isaret ediyor.

### C3. Envelope 2-10 ms tercihi
- Avantaj: konusma baslangicini daha hizli takip eder.
- Risk: gain oynakligi, yanlis tetik ve daha hissedilir pompalanma olasiligi artabilir.
- Not: Kisa pencere secimi bilincli olarak korunuyor ve tuning riski olarak izlenecek.

### C4. Limiter rolu ile clipping toleransi arasindaki gerilim
- Kesin bilgi: limiter ilk surumde emniyet blogu olarak dusunuluyor.
- Acik gerilim: nadir clipping toleransi ve ara sira limiter aktivasyonu, limiter'in pratikte ne kadar pasif kalacagini belirsizlestiriyor.

### C5. Clipping tercihi evrimi
- Onceki cizgi: clipping olabildigince guvenlik odakli ele alinmali.
- Guncel karar: ara sira clipping kabul edilebilir.
- Not: Bu degisim konusma netligi ve seviye onceligi lehine daha yuksek risk toleransina isaret ediyor.

### C6. Saturation stratejisi evrimi
- Onceki tercih: saturation yalnizca cikista uygulanacak.
- Guncel tercih: gain ve cikista saturate, ara adimlarda dikkatli olcekleme uygulanacak.
- Not: Bu degisim fixed-point guvenligini guclendirirken ara adim olcekleme kurallarini hala acik karar alani olarak birakiyor.

### C7. Filtreleme yapisi evrimi
- Onceki tercih: sadece temel bant sinirlama filtresi.
- Guncel karar: yuksek geciren + alcak geciren kombinasyonu.
- Not: Filtreleme amaci degismedi; sadece bunun daha acik uygulama karsiligi secildi.

### C8. 32-bit'ten 40-bit'e gecis politikasi
- Baslangic karari: 32-bit ile basla.
- Guncel kural: herhangi bir overflow veya ilgili hata logu gorulurse 40-bit'e gec.
- Not: Bu tercih muhafazakar guvenlik saglar, ancak loglama altyapisinin guvenilir olmasi gerekir.

## 5. Guncelleme Gecmisi

### 2026-03-29
- Ilk iskelet olusturuldu.
- Ilk kesin bilgiler ve baslangic varsayimlari eklendi.
- Ilk acik sorular tanimlandi.
- AM Radio konusmasi, gercek zamanli isleme, DSP hedefi ve AGC hedef davranisi eklendi.
- Fs-genel tasarim, fixed-point uyumlulugu ve ilk blok yapisi netlestirildi.
- Kullaniciya acik parametreler, zaman sabiti araliklari ve test odaklari kaydedildi.
- Acik kalan sayisal esitler ve celiski kayitlari gorunur bicimde eklendi.

### 2026-03-30
- AGC giris veri tipinin int16 ve hedef sayisal temsilin Q15 oldugu eklendi.
- Frame yapisi 4 ms sabit frame olarak netlestirildi.
- Parametrelerin yalnizca init sirasinda ayarlanacagi kaydedildi.
- Filtreleme blogu sonrasi AGC yerlesimi ve filtreleme amaci not edildi.
- Clipping, limiter ve gate histerezisi tercihleri guncellendi.
- Temel bant sinirlama filtresi, AGC -> limiter -> cikis zinciri ve 32-bit once yaklasimi eklendi.
- Saturation stratejisi gain ve cikista saturation olacak sekilde guncellendi.
- Filtreleme yapisi yuksek geciren + alcak geciren kombinasyonu olarak netlestirildi.
- 4 ms frame'in 8/16/64 kHz icin sample karsiliklari kaydedildi.
- HP kesimi 250 Hz, LP kesimi `0.4125 * Fs`, uyarlamali olcekleme ve yuvarlamali frame kuralı eklendi.
- FIR filtre tercihi, orta derece yaklasimi, peak tabanli olcekleme ve tum Fs'lerde ayni frame kurali eklendi.
- Kalan maddeler acik soru yerine uygulama sirasinda dogrulanacak ayrintilar olarak yeniden siniflandirildi.

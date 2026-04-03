# AGC High RMS Architecture Decision

## Tarih

- 2026-04-03

## Baglam

Mevcut baseline zinciri:

- slow RMS AGC
- gate
- frame-level peak protector
- final limiter

Bu zincir, daha muhafazakar hedeflerde iyi calisiyor. Ancak daha yuksek RMS hedeflerinde, ozellikle `%80 FS RMS` gibi gereksinimlerde peak bolgesinde lineerlik bozulmasi gozleniyor.

## Gozlem

Problem:

- RMS hedefi yukari cekildiginde AGC sinyali agresif bicimde buyutuyor
- crest factor yuksek bolgelerde peak headroom hizla tuketiliyor
- frame-level peak protector tum frame'i tek scale ile bastirdigi icin peak cevresinde sekil bukulmesi olusuyor

Sonuc:

- peak protector cok sik devreye giriyor
- limiter'e ulasmadan once bile lineerlik kaybi duyulabiliyor

## Karar

Yuksek RMS hedeflerini daha kontrollu tasimak icin zincir su yone evrilecek:

1. slow RMS AGC
2. peak-headroom aware gain cap
3. soft-knee compressor
4. final limiter

Bu karar, `docs/agc-multi-mode-target-level-decision.md` icindeki yeni modlu hedef tanimi ile birlikte okunmalidir.

Ortak hedef artik su sekilde yorumlanir:

- `konusma aktif frame'lerde hedef RMS ortalamasi`

Bu hedef hem `AM` hem `DIGITAL` mod icin ayni kavrami korur.

## Neden

### 1. `%80 FS RMS` korunmali

Bu hedef kaldirilmayacak; cunku farkli kullanim senaryolari icin ana gereksinim olarak tanimlandi.

### 2. `18 dB max gain` korunmali

Farkli sensitivitede mikrofonlara uyum icin genis gain araligi gerekli.

### 3. Hedef ile peak guvenligi ayni degil

`target RMS` ve `peak lineerligi` ayni anda her sinyalde kolayca saglanamaz.

Bu nedenle:

- RMS hedefi ana yonelim olarak korunacak
- ama peak headroom yeterli degilse AGC hedefe korlemesine saldiramayacak

## Uygulama Ilkesi

AGC hedefi artik su mantikla yorumlanacak:

`desired_gain = min(gain_for_target_rms, gain_allowed_by_peak_headroom, max_gain)`

Bu, `%80 RMS` hedefini "sert zorunlu" degil, "mumkunse ulasilacak hedef" haline getirir.

## Soft-Knee Compressor Neden Eklendi

Sadece limiter kullanmak:

- tepeye son anda sert mudahale eder
- peak bolgesinde sekil bozulmasini belirginlestirir

Soft-knee compressor:

- peak'e yaklasan bolgede daha yumusak bir gecis saglar
- limiter ve peak protector yukunu azaltir
- tepe bolgesindeki sekil bozulmasini daha az sert hale getirir

## Kapsam

Ilk uygulama asamasi:

- reference implementasyon uzerinde yapilacak
- fixed baseline dondurulmus kalacak
- fixed tarafa gecis, reference davranisi yeterince oturduktan sonra ayrica planlanacak
- sonraki iterasyonlarda `AM` ve `DIGITAL` mod profilleri ayni cekirdek uzerinde parametre farklariyla tanimlanacak

## Riskler

- compressor tuning'i pompalanma yaratabilir
- threshold/ratio/knee secimi konusma dogalligini etkileyebilir
- peak protector ve compressor birlikte fazla agresif kalirsa hedeflenen kazanc saglanmadan sinyal fazla yumusatilabilir

## Beklenen Kazanc

- yuksek RMS hedeflerinde daha az sekil bozulmasi
- peak protector aktivasyonunun daha kontrollu hale gelmesi
- farkli mikrofonlarda yuksek hedeflere daha dengeli ulasim

## Bu Kararin Baseline ile Iliskisi

- `docs/agc-reference-baseline.md` ve `docs/agc-fixed-baseline.md` dondurulmus durum olarak korunur
- bu karar yeni bir iterasyon mimarisi olarak degerlendirilir

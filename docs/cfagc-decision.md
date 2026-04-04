# CFAGC Decision

## Tarih

- 2026-04-04

## Ad

- `CFAGC = Crest-Factor-Aware Gain Controller`

## Amac

CFAGC, her ses sinyaline ayni RMS kuralini uygulamak yerine, sinyalin crest factor durumuna gore gain davranisini degistiren kontrol mantigidir.

Temel fikir:

- crest factor dusukse: `RMS target` daha baskin
- crest factor yuksekse: `peak-aware gain planning` daha baskin

## Neden

Tek tip `%80 RMS` zorlamasi:

- bazi sinyallerde limiter yukunu arttirabiliyor
- peak bolgelerde sekil bozulmasi olusturabiliyor

Bu nedenle hedef:

- aktif frame ortalamasini yuksek tutmak
- ancak peak baskin sinyallerde ayni agresiflikte davranmamak

## Secilen Metrik

- `smoothed crest factor (dB)`

Hesap:

- `crest_factor_db = 20 * log10(peak / rms)`

Bu deger birkac frame boyunca yumusatilarak kullanilacak.

## Ilk Parametreler

- `cf_low_db = 6.0`
- `cf_high_db = 10.5`
- `cf_rise_ms = 8`
- `cf_fall_ms = 40`
- `rms_activity_floor = 0.05`

## Crest Smoothing Kuralı

- `smoothed crest factor` artik her frame'de guncellenmez
- guncelleme yalniz:
  - `gate_open == true`
  - `smoothed_rms > rms_activity_floor`
  oldugunda yapilir
- guncelleme zamani asimetriktir:
  - crest factor yukselirken `cf_rise_ms`
  - crest factor duserken `cf_fall_ms`
- diger frame'lerde `smoothed crest factor` state'i tutulur

Bu secimle amac:

- sessizlik, kuyruk ve dusuk enerjili ara bolgelerin CF state'ini kirletmesini azaltmak
- peak-dominant yeni hece geldiginde CFAGC'nin daha anlamli gecmise bakmasini saglamak

## Hedefler

- `target_level = aktif frame RMS hedefi`
- ilk deneme: `target_peak = target_level + (1 - target_level) * 4/5`
- ikinci deneme: `target_peak = target_level + (1 - target_level) * 2/5`
- `limiter_threshold = target_level + (1 - target_level) * 9/10`

Ornek:

- ilk deneme:
  - `target_level = 0.80`
  - `target_peak = 0.96`
  - `limiter_threshold = 0.98`
- ikinci deneme:
  - `target_level = 0.80`
  - `target_peak = 0.88`
  - `limiter_threshold = 0.98`

## Gain Planning Yaklasimi

- `gain_rms = target_level / rms`
- `gain_peak = target_peak / peak`
- `w_cf = smooth blend weight`
- ilk lineer deneme sonrasi daha secici bir gecis icin:
  - `w_eff = w_cf^2`
  - `gain_desired`, lineer gain alaninda degil log-gain alaninda blend edilir
  - boylece peak-aware taraf, limiter yukunu azaltmak icin daha etkin cekis uygular

## Diger Kararlar

- Compressor bu asamada kodda kalacak ama `AM` modunda pasif olacak
- Limiter son emniyet katidir
- Limiter sayaç bilgisi ana gozlem verilerinden biridir

## Beklenen Fayda

- peak baskin sinyallerde limiter yukunu azaltmak
- crest factor dusuk sinyallerde ortalama seviyeyi daha verimli yukari tasimak
- tek tip RMS zorlamasinin sakincalarini azaltmak

## Ilk Risk

- yalniz gain planning ile beklenen iyilestirme yeterli olmayabilir
- CFAGC blend tek basina limiter yukunu yeterince dusurmezse, compressor tekrar ama daha kontrollu sekilde devreye alinabilir

## Ilk Gozlem

- `9 / 15 dB` araligi test seti icin fazla yukarida kaldi.
- `6.0 / 10.5 dB` araligi daha anlamli bir erken frenleme sagladi.
- Buna ragmen limiter yukundeki iyilesme sinirli, RMS kaybi ise belirgin olabilir.
- lineer blend sonrasi `convex (w^2)` blend, RMS ile limiter yuku arasinda daha dengeli bir ara nokta verdi.
- `convex (w^2)` blend'in log-gain alanina tasinmasi, limiter yukunu biraz daha asagi cekerken RMS'i buyuk olcude koruyan yeni bir ara aday verdi.
- `target_peak`in `4/5`ten `2/5`e indirilmesi ise beklenenden daha zayif etki gosterdi; limiter yukunun ana nedeni yalniz peak hedefi degil gibi gorunuyor.

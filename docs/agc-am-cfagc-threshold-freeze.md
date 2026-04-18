# AGC AM CFAGC Threshold Freeze

## Tarih

- 2026-04-04

## Amac

Bu freeze noktasi, `AM` preset uzerinde yapilan CFAGC ve limiter yuk denge arayisinda kabul edilen esik durumu sabitler.

Bu nokta bilerek su dengeyi korur:

- peak'ler onceki denemelere gore daha az zorlanir
- limiter yuku ciddi seviyede kontrol altina alinmistir
- RMS halen hedefin altindadir, yani sonraki iterasyon konusu olarak acik kalir

## Freeze Edilen Davranis

- `AM` preset uzerinde calisilir
- `DIGITAL` preset daha sonra ele alinacaktir
- `compressor` kodda vardir ama `AM` modunda pasiftir
- `CFAGC` etkindir
- Bu freeze sonrasi yeni iterasyonlarda `release_ms` gibi parametreler yeniden ayarlanabilir; asagidaki maddeler freeze aninin referans davranisini temsil eder
- `smoothed crest factor` asimetrik guncellenir:
  - `cf_rise_ms = 8`
  - `cf_fall_ms = 40`
- crest smoothing yalniz guvenilir aktif frame'lerde guncellenir:
  - `gate_open == true`
  - `smoothed_rms > 0.05`
- `target_peak = target + (1 - target) * 2/5`
- `limiter_threshold = target + (1 - target) * 9/10`
- `headroom-limited` frame'lerde peak-biased blend kullanilir
- peak-biased agirlik: `1 - (1 - w)^4`

## Bu Freeze'in Anlami

Bu nokta "nihai kalite bulundu" demek degildir.

Bu nokta, su an icin kabul edilen teknik esigi ifade eder:

- peak davranisi bariz bicimde kotulestirilmeden tutuluyor
- limiter sayaçlari onceki bircok denemeye gore daha kontrol altında
- ama RMS hedefe ulasma tarafi hala iyilestirme adayi

## Sonraki Iterasyonun Amaci

Bir sonraki arayis bu freeze'i bozmadan sunu hedefleyecek:

- peak davranisini cok kotulestirmeden
- limiter'i tekrar patlatmadan
- RMS'i daha yukari tasimak

## Ilgili Dokumanlar

- `docs/cfagc-decision.md`
- `docs/agc-test-wav-batch-results-v12.md`
- `docs/agc-test-wav-batch-results-v13.md`
- `docs/agc-test-wav-batch-results-v14.md`
- `docs/agc-test-wav-batch-results-v15.md`
- `docs/agc-signal-debugger.md`
- `docs/iteration-notes.md`

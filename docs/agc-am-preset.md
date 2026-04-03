# AGC AM Preset

## Tarih

- 2026-04-04

## Amac

Bu dokuman, ortak AGC cekirdegi icin ilk resmi `AM` preset'ini sabitler.

Bu preset:

- `konusma aktif frame'lerde hedef RMS ortalamasi` hedefini kullanir
- peak bolgelerde zorunlu olmadikca sert limiter mudahalesini azaltmaya calisir
- `AM` haberlesmede ortalama seviye/verim onceligini dikkate alir

## Ortak Hedef

`AM` preset'te target level su anlamdadir:

- `konusma aktif frame'lerde hedef RMS ortalamasi`

Bu yorum `DIGITAL` mod ile ayni kalir. Fark, bu hedefe nasil ulasildigindadir.

## Ilk AM Preset Degerleri

- `mode = AM`
- `frame_ms = 4`
- `attack_ms = 8`
- `release_ms = 300`
- `envelope_window_ms = 4`
- `rms_window_ms = 8`
- `gate_hold_ms = 80`
- `gate_threshold = kullanici ayarli`
- `max_gain = kullanici ayarli`
- `peak_headroom_cap_enabled = 0`
- `compressor_enabled = 1`
- `compressor_threshold_dbfs = -2`
- `compressor_ratio = 6`
- `compressor_knee_db = 8`
- `peak_protector_enabled = 0`
- `limiter_threshold = 0.995`

## Mantik

Bu preset'in temel mantigi:

- AGC ortalama seviyeyi yukari tasimaya calisir
- peak bolgelerde erken ve kaba frame-level fren yerine compressor once devreye girer
- final limiter yalniz son emniyet olarak kalir

## Beklenen Davranis

- `%80` gibi yuksek hedeflerde ortalama seviyeyi yukariya tasimaya daha isteklidir
- peak'ler `~%100`e kadar cikabilir
- peak protector yerine compressor ve son koruma agirligi artar
- sayisal olarak limiter hala gorulebilir, ancak amac onun zorunlu durumlar disinda baskin olmamasidir

## Bilinen Risk

- Compressor fazla agresif kalirsa konusma dogalligini azaltabilir
- Final limiter yine de devreye girerse peak bolgelerde lineerlik sorunu tamamen kaybolmayabilir
- `%80` hedefi her sinyalde birebir saglanamayabilir; crest factor yuksek sinyallerde hala trade-off vardir
- `compressor_threshold_dbfs` cok yukarida secilirse (`-1 dBFS` gibi), ortalama seviye bir miktar artsa da limiter yukunde ciddi artis olabilir

## Sonuc

Bu preset, ortak cekirdekte `AM` odakli ilk resmi profil olarak kullanilacaktir.

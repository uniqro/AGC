# AGC AM Reference Freeze

## Tarih

- 2026-04-04

## Amac

Bu dosya, baseline sonrasinda gelistirilen yeni reference AGC zincirinin `AM` odakli dondurulmus durumunu sabitler.

Bu freeze noktasi:

- eski `reference baseline`in yerine gecmez
- onun ustune gelisen yeni iterasyon zincirini kaydeder
- sonraki uygulama ve DSP uyarlama adimlari icin kararlı bir basvuru noktasi saglar

## Dondurulan Yapi

- `konusma aktif frame'lerde hedef RMS ortalamasi`
- `AM` preset
- slow RMS AGC
- gate
- soft-knee compressor
- peak protector devre disi
- soft final limiter
- `mode` tabanli config/preset yapisi

## Dondurulan Ilkeler

- `target level` hem `AM` hem `DIGITAL` icin ayni kavrami korur
- mode farki hedef anlaminda degil, uygulanisinda olusur
- peak bolgelerde zorunlu olmadikca kaba limiter davranisi tercih edilmez
- final protection son emniyet katidir
- `AM` preset verim/ortalama seviye tarafina daha isteklidir

## Dondurulan AM Preset Yonleri

- `attack_ms = 8`
- `release_ms = 300`
- `gate_hold_ms = 80`
- `compressor_enabled = 1`
- `compressor_threshold_dbfs = -2`
- `compressor_ratio = 6`
- `compressor_knee_db = 8`
- `peak_headroom_cap_enabled = 0`
- `peak_protector_enabled = 0`
- `limiter_threshold = 0.995`

## Ilgili Dokumanlar

- `docs/agc-am-preset.md`
- `docs/agc-multi-mode-target-level-decision.md`
- `docs/agc-high-rms-architecture-decision.md`
- `docs/agc-test-wav-batch-results-v2.md`
- `docs/agc-test-wav-batch-results-v3.md`
- `docs/agc-test-wav-batch-results-v4.md`
- `docs/agc-test-wav-batch-results-v5.md`

## Not

Bu freeze, "nihai urun tamamlandi" anlami tasimaz.

Bu nokta:

- fikir birligi saglanan mevcut algoritmik yonu dondurur
- sonraki uygulama adimlarinda yeni degisikliklerin nereden dallanacagini netlestirir

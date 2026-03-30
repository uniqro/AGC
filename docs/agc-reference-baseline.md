# AGC Reference Baseline

Bu dosya, DSP portundan once sabitlenen reference AGC davranisini tek yerde toplar.

## 1. Amaç

- Temiz referans davranisi dondurmek
- DSP portu sirasinda davranis sapmalarini bu baza gore karsilastirmak
- Tuning denemeleri arasindan korunacak cekirdek yapiyi netlestirmek

## 2. Sabitlenen Cekirdek

- RMS tabanli tek bant AGC
- Basit gate
- Ayrik peak protector
- Emniyet limiter
- Filtreleme AGC blogunun disinda
- Noise-aware gain benzeri ek karar katmanlari bu bazda yok

## 3. Sabitlenen Parametre Seti

- `target_rms_fs = 0.42`
- `attack_ms = 8`
- `release_ms = 300`
- `envelope_window_ms = 4`
- `rms_window_ms = 8`
- `gate_hold_ms = 80`
- `gate_threshold = 0.05`
- `max_gain = 4.0`
- `peak_protector_ratio = 0.97`
- `limiter_threshold = 0.98`

## 4. Beklenen Reference Davranis

- Konusma bloklarinda seviye dengelenir, ama tum bloklar kusursuz esitlenmez
- Sessizlikte veya gap bolgelerinde gain sisirmesi sinirli tutulur
- Buyuk step girislerinde limiter yerine once peak protector devreye girer
- `noise_floor_step` ve `noisy_speech_turns` senaryolari, sade cekirdegin sinirlarini gosteren regresyon senaryolari olarak korunur

## 5. Baseline Test Seti

- `reference_burst`
- `reference_step`
- `reference_step_hot`
- `reference_noisy`
- `reference_speech_turns`
- `reference_noisy_speech_turns`
- `reference_noise_floor_step`
- `reference_pause_recovery`

## 6. Porting Sirasinda Korunacaklar

- Frame yapisi: `4 ms`
- Mono isleme
- Init sirasinda sabit parametre yukleme
- Peak protector ile limiter ayrimi
- Detector state'in frame'ler arasi korunmasi

## 7. Not

Bu baseline, "nihai urun davranisi" degil; DSP portu sirasinda referans alacagimiz en temiz ve maliyet/fayda dengesi iyi bulunan davranistir.

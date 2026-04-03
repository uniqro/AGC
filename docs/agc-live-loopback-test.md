# AGC Live Loopback Test

Bu not, kulaklik mikrofonundan gelen sesi AGC'den gecirip tekrar kulakliga vermek icin eklenen Windows test aracini aciklar.

## Amac

- Gercek zamanli dinleme ile AGC davranisini duymak
- Burst, sesli-sessiz gecis ve genel seviye etkisini hizli test etmek
- Mevcut fixed-point cekirdegi algoritma degistirmeden canli akisa baglamak
- Cihaz secimi, seviye gostergesi ve eszamanli WAV kaydi ile test rahatligi saglamak

## Arac

- Kaynak: `tools/agc_live_loopback_winmm.c`
- API: Windows `waveIn/waveOut`
- Cekirdek: `fixed/agc_fixed_runtime_process(...)`

Ek ozellikler:

- input/output cihaz listeleme
- canli console seviye gostergesi
- input ve output'u ayni anda WAV olarak kaydetme

## Derleme

Ornek GCC derleme komutu:

```powershell
gcc -Ifixed `
    tools\agc_live_loopback_winmm.c `
    fixed\agc_fixed_config.c `
    fixed\agc_fixed_detector.c `
    fixed\agc_fixed_gate.c `
    fixed\agc_fixed_gain.c `
    fixed\agc_fixed_peak_protector.c `
    fixed\agc_fixed_limiter.c `
    fixed\agc_fixed_pipeline.c `
    fixed\agc_fixed_runtime.c `
    -lwinmm `
    -o tools\agc_live_loopback_winmm.exe
```

## Calistirma

Varsayilan:

```powershell
tools\agc_live_loopback_winmm.exe
```

Parametreli:

```powershell
tools\agc_live_loopback_winmm.exe 16000 42 12
```

Anlami:

- `16000`: sample rate
- `42`: target RMS yuzdesi
- `12`: max gain dB

Cihazlari listele:

```powershell
tools\agc_live_loopback_winmm.exe --list
```

Cihaz secimi ile:

```powershell
tools\agc_live_loopback_winmm.exe 16000 42 12 1 3
```

Anlami:

- `1`: input device id
- `3`: output device id

Kayit ile:

```powershell
tools\agc_live_loopback_winmm.exe 16000 42 12 1 3 session01
```

Bu durumda su dosyalar yazilir:

- `session01_input.wav`
- `session01_output.wav`

## Davranis

- Default capture cihazindan mono `int16` PCM veri alir
- Her blok mevcut fixed AGC pipeline'inda islenir
- Islenmis veri default output cihazina verilir
- Istendiyse belirli input/output cihaz id'leri kullanilir
- Console uzerinde input ve output peak seviyesi canli gorulur
- Istendiyse input ve output akisi eszamanli WAV olarak kaydedilir
- Durdurmak icin klavyeden bir tusa basilir

## Latency

- Algoritma frame tabanli oldugu icin temel blok gecikmesi vardir
- Baseline frame suresi `4 ms`
- Buna ses surucusu ve `waveIn/waveOut` tampon gecikmeleri eklenir
- Gercek duyulan toplam latency, algoritma gecikmesinden daha yuksek olur

## Dikkat Edilecekler

- Mutlaka kulaklik veya headset ile dene
- Aksi halde acoustic feedback olusabilir
- Ilk denemede ses seviyesini dusuk tut
- Windows varsayilan input/output cihazlarini kullandigi icin sistem secimi onemlidir

## Sinirlar

- Bu arac dusuk riskli yerel test icindir
- Asil DSP entegrasyon modelini temsil etmez
- `waveIn/waveOut` tabanli oldugu icin profesyonel ses API kadar dusuk latency beklenmemelidir

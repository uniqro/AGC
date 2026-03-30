# Reference AGC (C Skeleton)

Bu klasor, AGC reference implementasyonu icin C dilinde iskelet yapisini tutar.

Amac:
- Moduller arasi sorumluluklari netlestirmek
- Frame bazli pipeline akisini sabitlemek
- Sonraki asamada algoritma detaylarini kontrollu sekilde doldurmak

Bu asamada:
- Dosya yapisi olusturuldu
- Ortak veri yapilari tanimlandi
- Pipeline ve blok arayuzleri eklendi
- Metrik toplama ve test vektoru iskeleti eklendi
- Giris ve cikis WAV dosyasi uretimi eklendi
- RMS AGC ile peak koruma katmani ayri bloklar olarak ele aliniyor
- `fixed/agc_fixed_compare.c` ile ayni test vector'leri uzerinde reference ve fixed ciktilari yanyana karsilastirilabiliyor

Bu asamada henuz tamamlanmayanlar:
- Gercek AGC gain kanunu
- Gate karari ve limiter davranisinin tam tuning'i
- Clipping ve limiter esiklerinin son hali

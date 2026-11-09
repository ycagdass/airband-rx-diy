# Sorun Giderme — VHF Havacılık Bandı Alıcısı

Bu belgede yapım sürecinde karşılaşılan gerçek sorunlar ve çözümleri yer almaktadır.

---

## Sorun 1: LM386 Aşırı Isınıyor (93°C)

### Belirtiler
- Chip yükü olmadan (kulaklıksız) ~42–43°C
- Kulaklık takılıyken 70–80°C, bazen 93°C
- Zobel ağı eklendikten sonra kulaklıksız ~80°C

### Hatalı Tahmin
İlk başta jak pin 2–4 arasındaki köprü veya bağlantı hatası şüphelenildi.  
Bu **yanlış** bir teşhisti. İki stereo kanalı aynı mono kaynaktan beslemek sorun değildir.

### Gerçek Neden
LM386 besleme hattında (Pin 6 ve Pin 4 arasında) **çipe yakın hiç kondansatör yoktu.**  
Bu durum, uzun kablolar ve breadboard kapasitansının da etkisiyle **yüksek frekanslı osilasyon** başlattı.  
Chip, duyurulabilir ses frekansı üretmek yerine RF aralığında boşa salınım yaptı ve ısındı.

### Çözüm ✅
Pin 6 ile Pin 4 arasına, **çip bacaklarına doğrudan** (kısa kablo):
- 100 µF elektrolitik kondansatör (+ Pin 6)
- 100 nF seramik kondansatör (paralel)

**Sonuç:** Sıcaklık 29–30°C'ye düştü.

### Neden Zobel Ağı Sorunu Kötüleştirdi?
Osilasyon varken eklenen Zobel ağı (10 Ω + 47 nF), yüksek frekanslarda düşük empedans yolu oluşturdu.  
Bu, osilasyon akımının artmasına neden oldu.  
**Önce dekuplaj, sonra Zobel** eklenmesi gerekir.

---

## Sorun 2: Si5351 Bulunamadı

### Belirti
Serial Monitor'da: `Si5351 bulunamadi. Kablolari ve beslemeyi kontrol et.`

### Olası Nedenler ve Kontroller

| Kontrol | Nasıl Yapılır |
|---|---|
| I2C pinleri doğru mu? | SDA = GPIO21, SCL = GPIO22 olduğunu doğrula |
| Modül beslemesi | Si5351 → 3V3 ve GND bağlı mı? |
| Kablo bağlantısı | Jumper kablo tam oturdu mu, gevşek mi? |
| Kütüphane kurulu mu? | Arduino IDE → Library Manager → "Si5351" (Etherkit by Jason Milldrum) |

### Hızlı Test
```
I2C adresi: 0x60 (96 decimal)
I2C Scanner kodu çalıştır, 0x60 görünüyor mu?
```

---

## Sorun 3: Mikser Çalışmıyor / Ses Gelmiyor

### Test Yöntemi
Önce `mikser_test.ino` ile ton testi yap:
- CLK0 = 128.500 MHz (LO)
- CLK1 = 128.501 MHz (test sinyali, pin 1'e)
- Beklenen: Kulaklıkta 1 kHz ton (ince bip sesi)

### Ses Hiç Gelmiyorsa
1. LM386 çalışıyor mu? Pin 3'e parmak dokun — gürültü artıyor mu?
2. 10 µF kuplaj kondansatörü doğru yönde mi? (+ ucu NE602 pin 4 tarafında)
3. POT kablolu mu? Orta bacak LM386 Pin 3'e bağlı mı?
4. NE602 beslemesi: Pin 8 → +5 V bağlı mı?

### Ses Çok Kısıksa
- Bu normaldir — NE602 çıkışı zayıftır.
- POT'u sağa çevir.
- 101 Ω seri dirençleri geçici olarak çıkar, test et.
- LM386 Pin 1 ve Pin 8 arasına 10 µF kondansatör koy (kazanç 20× → 200×).

### Ses Var Ama Ton Frekansı Değişmiyorsa
CLK1 PLLB'ye atanmadı. `mikser_test.ino` kodunda şu satırın olduğundan emin ol:
```cpp
si5351.set_ms_source(SI5351_CLK1, SI5351_PLLB);
```
Bu olmadan CLK0 ve CLK1 aynı PLL'yi paylaşır, birbirinden bağımsız ayarlanamaz.

---

## Sorun 4: Havadan Sinyal Alınamıyor

### Frekans Kontrolü
`alici_lo.ino` ile Serial Monitor'dan LO frekansını gir.  
Kayseri Erkilet Havalimanı (LTAU) için dene:
- **121.500 MHz** — Acil kanal (her zaman açık)
- **118.000–136.975 MHz** — Havacılık bandının tamamı
- Yakın çevre havalimanı TWR/APP frekanslarını havacılık frekans veritabanlarından bul.

### Anten Kontrolü
- Uzunluk ~58 cm mi?
- Tel düz ve dik mi duruyor?
- NE602 Pin 1'e 100 pF üzerinden bağlı mı?
- Anten ve GND arasında kısa devre var mı? (Olmamalı)

### Alıcı Duyarlılığı
Bu basit bir direct-conversion alıcıdır. Beklentiler:
- Yakın istasyonlar (~10–50 km): Duyulabilir
- Uzak istasyonlar (>100 km): Zor veya imkânsız
- Gürültü katı: Yüksek — ön filtre veya RF amplifikatörü eklenmedi

---

## Sorun 5: NE602AN'a Yanlış Besleme

### Risk
NE602AN maksimum 9 V ile çalışabilir ama nominal 5 V'tur.  
8,9 V uygulanırsa chip zamanla bozulabilir.

### Çözüm
NE602AN Pin 8'i mutlaka **ESP32 VIN (5 V)** üzerinden besle.  
LM386'nın 8,9 V hattını NE602'ye **bağlama**.

---

## Sorun 6: Gürültü ve Parazit

### RF Paraziti
- ESP32 ve Si5351 RF üretir. Bunları alıcı devresinden mümkün olduğunca uzak tut.
- Kablolama kısa tutulmalı.

### Ortak GND Yoksa
Farklı modüllerin GND'leri bağlı değilse garip sesler, düşük ses seviyesi veya osilasyon görülür.  
Tüm GND noktalarını tek bir ortak noktada birleştir.

---

## Hızlı Teşhis Tablosu

| Belirti | İlk Kontrol |
|---|---|
| LM386 ısınıyor | Pin 6–4 arası 100 µF + 100 nF dekuplaj var mı? |
| Ses yok | LM386 Pin 3'e parmak dokun, gürültü var mı? |
| Si5351 bulunamıyor | I2C pinleri GPIO21/22 mi? 3V3 bağlı mı? |
| Mikser ton testi başarısız | CLK1 PLLB'ye atandı mı? CLK0 pin 6'ya bağlı mı? |
| Havadan sinyal yok | Anten bağlı mı? Doğru frekans aralığında mı? |
| Ses çok gürültülü | GND ortak mı? Kablolar kısa mı? |

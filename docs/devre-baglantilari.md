# Devre Bağlantıları — VHF Havacılık Bandı Alıcısı

Bu belge, `airband-rx-diy` projesinin fiziksel kablolama ve bağlantı bilgilerini içerir.  
**Önemli:** Belirtilen değerler, gerçek yapım sürecinde test edilmiş ve çalıştığı doğrulanmıştır.

---

## Genel Mimari

```
ANTEN (58 cm)
    │
    │  100 pF (kuplaj)
    ▼
NE602AN Pin 1 ──────── RF girişi
                        │
Si5351 CLK0 ──────────► Pin 6 (LO girişi)
   (1 kΩ + 110 Ω + 100 pF zayıflatıcı ile)
                        │
                      Pin 4 ──► 10 µF (kuplaj) ──► POT ──► LM386 Pin 3
                                                              │
                                                           Kulaklık
```

---

## 1. Güç Kaynağı

| Bileşen | Besleme | Kaynak |
|---|---|---|
| LM386N-1 | ~8,9 V DC | Adaptör (XL4015 modülü ile ayarlanmış) |
| NE602AN | 5 V | ESP32 VIN pini |
| Si5351 modülü | 3,3 V | ESP32 3V3 pini |
| ESP32 DevKit | USB veya adaptör | — |

**Önemli:** Tüm modüllerin GND'leri ortak noktada birleşmelidir.

---

## 2. NE602AN Mikser

**DIP-8 pin numaralandırması (tepe görünümü, çentik solda):**

```
        ┌───────────┐
 RF_A ──┤ 1       8 ├── (boş)
 RF_B ──┤ 2       7 ├── OSC_E (anten tarafından boş bırakıldı)
   GND ─┤ 3       6 ├── OSC_B  ◄── LO girişi (Si5351 CLK0)
 OUT_A ─┤ 4       5 ├── OUT_B
        └───────────┘
              │
             VCC (pin 8) ◄── +5 V (ESP32 VIN)
```

| Pin | Bağlantı |
|---|---|
| 1 (RF_A) | Anten kablosuna 100 pF seri kondansatör üzerinden |
| 2 (RF_B) | 100 nF kondansatör üzerinden GND'ye |
| 3 (GND) | Ortak GND |
| 4 (OUT_A) | 10 µF (+ uç pin 4'e) üzerinden potansiyometrenin sağ ucuna |
| 5 (OUT_B) | Kullanılmıyor |
| 6 (OSC_B) | LO sinyali (zayıflatıcıdan) |
| 7 (OSC_E) | Boş bırakıldı (harici osilatör modunda kullanılmıyor) |
| 8 (VCC) | +5 V |

---

## 3. Si5351 → NE602AN LO Zayıflatıcısı

Si5351 çıkışı 3,3 V CMOS kare dalga üretir; NE602AN'ın pin 6 girişi ~200 mV ister.  
Gerilim bölücü + kuplaj kondansatörü ile zayıflatılır.

```
Si5351 CLK0
     │
    [1 kΩ]
     │
     ├──── [110 Ω] ──── GND   ← 2 × 56 Ω seri (ölçülen değer 110 Ω)
     │
    [100 pF]
     │
  NE602 Pin 6
```

**Hesap:** 1 kΩ / (1 kΩ + 110 Ω) ≈ %10 oranı → 3,3 V × 0,10 ≈ 330 mV  
(100 pF'ın empedansı 128 MHz'de ~12 Ω, bölücüyü etkilemiyor)

**Not:** 100 pF yerine 1 nF kullanılabilir; elimizde sadece 100 pF (101 yazan) vardı ve çalıştı.

---

## 4. ESP32 → Si5351 I2C Bağlantısı

| ESP32 Pini | Si5351 Modülü |
|---|---|
| GPIO 21 (SDA) | SDA |
| GPIO 22 (SCL) | SCL |
| 3V3 | VCC |
| GND | GND |

---

## 5. LM386N-1 Ses Katı

**DIP-8 pin numaralandırması:**

```
        ┌───────────┐
 Gain ──┤ 1       8 ├── Gain   ← her ikisi de boş (varsayılan kazanç 20×)
  IN− ──┤ 2       7 ├── Bypass ← boş
  IN+ ──┤ 3       6 ├── VCC   ◄── ~8,9 V
   GND ─┤ 4       5 ├── OUT
        └───────────┘
```

| Pin | Bağlantı |
|---|---|
| 1 | Boş (kazanç 20×) |
| 2 (IN−) | GND |
| 3 (IN+) | Potansiyometre orta bacağı (wiper) |
| 4 (GND) | Ortak GND |
| 5 (OUT) | 220 µF (+) ucu → çıkış; ayrıca Zobel ağı |
| 6 (VCC) | ~8,9 V; **çip bacaklarına doğrudan** 100 µF + 100 nF dekuplaj |
| 7 | Boş |
| 8 | Boş (kazanç 20×) |

### Besleme Dekuplajı (kritik — ısınma sorunu bununla çözüldü)

```
+8,9 V ───┬──────── Pin 6
          │
        [100 µF elektrolitik, + pin 6]
          │       (paralel)
        [100 nF seramik]
          │
         GND (Pin 4)
```

Kondansatörler çipin bacaklarına doğrudan, kısa kablolarla lehimlendi.

### Zobel Ağı

```
Pin 5 ──[10 Ω]──[47 nF seramik]── GND (Pin 4)
```

### Çıkış

```
Pin 5 ──[220 µF (+)]──┬──[101 Ω]── Jak Pin 2 (sol kanal)
                       └──[101 Ω]── Jak Pin 4 (sağ kanal)
Jak Pin 1 ── GND
```

---

## 6. Potansiyometre (10 kΩ)

| Bacak | Bağlantı |
|---|---|
| Sol | GND |
| Orta (wiper) | LM386 Pin 3 |
| Sağ | NE602 çıkışından gelen 10 µF'ın − ucu |

---

## 7. Anten

- Uzunluk: **~58 cm** (127,5 MHz için λ/4 monopol)
- Malzeme: Bakır tel veya koaksiyel kablo iç iletkeni
- Bağlantı: Telin bir ucu, 100 pF seri kondansatör üzerinden NE602AN Pin 1'e

**Frekans hesabı:**
```
Uzunluk (cm) = 7500 / frekans_MHz
127,5 MHz için: 7500 / 127,5 ≈ 58,8 cm
```

---

## 8. Önemli Notlar

1. **GND ortak olmalı:** ESP32, NE602AN ve LM386 aynı GND noktasına bağlanmalı.
2. **Besleme dekuplajı zorunlu:** LM386 besleme hattında kondansatör olmadan osilasyona girer ve 90°C'yi aşar.
3. **LO seviyesi:** Si5351 doğrudan bağlanmamalı; zayıflatıcı devre şarttır.
4. **NE602AN 5 V ile çalışır:** Yanlışlıkla 8,9 V uygulanırsa chip kalıcı hasar görebilir.
5. **100 Ω yerine 110 Ω:** İki adet 56 Ω seri bağlanarak kullanıldı (ölçülen 110 Ω); kabul edilebilir.

---

## Doğrulanmış Testler

| Test | Sonuç |
|---|---|
| LM386 ısınma testi (besleme dekuplajı ile) | ✅ 29–30°C, normal |
| Si5351 FM radyo testi | ✅ Çalan müzik kesildi |
| NE602AN mikser ton testi (1 kHz fark) | ✅ Kulaklıkta 1 kHz ton duyuldu |
| Havadan sinyal alımı | ⏳ Henüz test edilmedi |

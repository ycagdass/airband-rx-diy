// Alıcı LO kontrolü: frekansı Serial Monitor'den klavyeyle ayarlarsın.
// CLK0 = yerel osilatör (mikserin pin 6'sına gider)
// Kütüphane: "Si5351" (Etherkit)
// ESP32 DevKit için I2C pinleri: SDA = GPIO21, SCL = GPIO22
//
// Serial Monitor ayarı: hız 115200, satır sonu "Newline" olmalı.
//
// Komutlar:
//   127.500  -> LO'yu 127.500 MHz yap (100–150 MHz arası)
//   +        -> 100 Hz yukarı
//   -        -> 100 Hz aşağı
//   ++       -> 1 kHz yukarı
//   --       -> 1 kHz aşağı

#include <Wire.h>
#include "si5351.h"

Si5351 si5351;
double loMHz = 127.500;  // başlangıç frekansı

void loGuncelle() {
  si5351.set_freq((uint64_t)(loMHz * 100000000.0 + 0.5), SI5351_CLK0);
  Serial.print("LO: ");
  Serial.print(loMHz, 4);
  Serial.println(" MHz");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(21, 22);

  if (!si5351.init(SI5351_CRYSTAL_LOAD_8PF, 25000000, 0)) {
    Serial.println("Si5351 bulunamadi. Kablolari kontrol et.");
    return;
  }
  Serial.println("Si5351 bulundu.");

  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 1);
  si5351.output_enable(SI5351_CLK1, 0);  // test sinyali kapali

  loGuncelle();
  Serial.println("Komutlar: 127.500 | + | - | ++ | --");
}

void loop() {
  if (!Serial.available()) return;

  String girdi = Serial.readStringUntil('\n');
  girdi.trim();
  if (girdi.length() == 0) return;

  if (girdi == "+") {
    loMHz += 0.0001;
  } else if (girdi == "-") {
    loMHz -= 0.0001;
  } else if (girdi == "++") {
    loMHz += 0.001;
  } else if (girdi == "--") {
    loMHz -= 0.001;
  } else {
    double f = girdi.toDouble();
    if (f < 100.0 || f > 150.0) {
      Serial.println("Gecersiz. 100 ile 150 MHz arasi bir sayi yaz.");
      return;
    }
    loMHz = f;
  }

  loGuncelle();
}

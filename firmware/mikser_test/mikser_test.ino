// Mikser testi: iki frekans arasındaki fark 1 kHz'lik ses tonu üretir.
// CLK0 = yerel osilatör (mikserin pin 6'sına gider)
// CLK1 = test sinyali (mikserin pin 1'ine gider)
// Kütüphane: "Si5351" (Etherkit)
// ESP32 DevKit için I2C pinleri: SDA = GPIO21, SCL = GPIO22

#include <Wire.h>
#include "si5351.h"

Si5351 si5351;

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(21, 22);

  if (!si5351.init(SI5351_CRYSTAL_LOAD_8PF, 25000000, 0)) {
    Serial.println("Si5351 bulunamadi. Kablolari kontrol et.");
    return;
  }
  Serial.println("Si5351 bulundu.");

  // CLK1'i ikinci PLL'ye ata, böylece iki çıkış birbirinden bağımsız ayarlanır.
  si5351.set_ms_source(SI5351_CLK1, SI5351_PLLB);

  // Frekansları MHz olarak yaz. Aradaki fark = duyacağın ton (1 kHz).
  double loMHz   = 128.500;
  double testMHz = 128.501;

  si5351.set_freq((uint64_t)(loMHz   * 100000000.0 + 0.5), SI5351_CLK0);
  si5351.set_freq((uint64_t)(testMHz * 100000000.0 + 0.5), SI5351_CLK1);

  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 1);
  si5351.output_enable(SI5351_CLK1, 1);

  Serial.print("CLK0 (LO):   ");
  Serial.print(loMHz, 3);
  Serial.println(" MHz");
  Serial.print("CLK1 (test): ");
  Serial.print(testMHz, 3);
  Serial.println(" MHz");
}

void loop() {
}

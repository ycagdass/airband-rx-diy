// Si5351 ilk test: CLK0 çıkışından 100 MHz üretir.
// Kütüphane: Arduino IDE > Library Manager > "Si5351" (Etherkit)
// ESP32 DevKit için I2C pinleri: SDA = GPIO21, SCL = GPIO22

#include <Wire.h>
#include "si5351.h"

Si5351 si5351;

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(21, 22);

  bool bulundu = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 25000000, 0);
  if (!bulundu) {
    Serial.println("Si5351 bulunamadi. Kablolari ve beslemeyi kontrol et.");
    return;
  }
  Serial.println("Si5351 bulundu.");

  // Frekansı MHz olarak buraya yaz, hesabı kod yapar.
  double frekansMHz = 100.0;
  uint64_t frekans = (uint64_t)(frekansMHz * 100000000.0 + 0.5);  // 0,01 Hz birimi
  si5351.set_freq(frekans, SI5351_CLK0);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 1);

  Serial.print("CLK0: ");
  Serial.print(frekansMHz, 3);
  Serial.println(" MHz");
}

void loop() {
}

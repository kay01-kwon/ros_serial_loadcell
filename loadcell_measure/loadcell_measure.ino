#include "HX711.h"

HX711 scale1, scale2;

const uint8_t dataPin1 = 2;
const uint8_t clockPin1 = 3;

const uint8_t dataPin2 = 4;
const uint8_t clockPin2 = 5;

float f1, f2;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  scale1.begin(dataPin1, clockPin1);
  scale2.begin(dataPin2, clockPin2);

  scale1.set_scale(1309.549561);
  scale2.set_scale(1462.802002);

  scale1.tare(30);
  scale2.tare(30);

}

void loop() {
  // put your main code here, to run repeatedly:

  f1 = scale1.get_units(20);
  f2 = scale2.get_units(20);

  // Serial.print('f1: ');
  Serial.print(f1,3);
  Serial.print('\t');
  // Serial.print('f2: ');
  Serial.println(f2,3);

  delay(200);

}

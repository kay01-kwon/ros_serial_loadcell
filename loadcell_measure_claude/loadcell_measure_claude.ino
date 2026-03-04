#include <HX711.h>

const int DOUT_PIN1 = 2;
const int SCK_PIN1 = 3;
const int DOUT_PIN2 = 4;
const int SCK_PIN2 = 5;

const float SCALE_FACTOR_1 = 989.0834;
const float SCALE_FACTOR_2 = 994.6169;

const long OFFSET_1 = 78463L;
const long OFFSET_2 = 17988L;

HX711 scale1, scale2;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  scale1.begin(DOUT_PIN1, SCK_PIN1);
  scale2.begin(DOUT_PIN2, SCK_PIN2);
  
  scale1.set_scale(SCALE_FACTOR_1);
  scale2.set_scale(SCALE_FACTOR_2);

  scale1.set_offset(OFFSET_1);
  scale2.set_offset(OFFSET_2);
  
  Serial.println("READY");
}

void loop() {

    if(scale1.is_ready() && scale2.is_ready())
    {
      float w1 = scale1.get_units(20);
      float w2 = scale2.get_units(20);

      Serial.print(w1, 3);
      Serial.print('\t');
      Serial.println(w2, 3);
      delay(15);
    }
}
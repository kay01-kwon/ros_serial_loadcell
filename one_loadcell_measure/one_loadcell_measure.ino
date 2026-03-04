#include <HX711.h>

const int DOUT_PIN1 = 2;
const int SCK_PIN1 = 3;

const float SCALE_FACTOR = 988.3641;
const long OFFSET = -202838L;

HX711 scale;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);

  scale.begin(DOUT_PIN1, SCK_PIN1);

  // Set scale and offset
  scale.set_scale(SCALE_FACTOR);

  scale.set_offset(OFFSET);

}

void loop() {
  // put your main code here, to run repeatedly:

  if(scale.is_ready())
  {
    float w = scale.get_units();

    Serial.print(w, 3);
    Serial.print('\n');
    delay(13);
  }

}

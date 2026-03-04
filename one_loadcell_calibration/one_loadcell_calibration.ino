#include <HX711.h>

const int DOUT_PIN1 = 2;
const int SCK_PIN1 = 3;

HX711 scale;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  while(!Serial);

  scale.begin(DOUT_PIN1, SCK_PIN1);

  Serial.println("=== Calibration Mode ===");
  Serial.println("Send 'C' to calibrate");
  Serial.println("Send 'O' to get offset");


}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0)
  {
    char cmd = Serial.read();

    if(cmd == 'C')
    {
      calibrate();
    }
    else if (cmd == 'O')
    {
      getOffset();
    }
  }
}

void calibrate()
{
  Serial.println("\n=== CALIBRATION ===");
  Serial.println("Step 1: Remove weight");
  Serial.println("Press Enter when ready");
  while(!Serial.available());
  Serial.read();

  Serial.println("Taring...");
  scale.tare(50);
  Serial.println("Tare done!");

  Serial.println("\nStep 2: Place 1000g on LoadCell");
  Serial.println("Press Enter when ready");
  while(!Serial.available());
  Serial.read();

  delay(3000);
  float reading = scale.get_units(50);
  float scale_factor = reading / 1000.0;

  Serial.print("\nReading: ");
  Serial.println(reading);
  Serial.println("SCALE_FACTOR = ");
  Serial.println(scale_factor,4);

  Serial.println("\n=== COPY THESE VALUES ===");
  Serial.print("const float SCALE_FACTOR = ");
  Serial.print(scale_factor, 4);
  Serial.println(";");
}

void getOffset()
{
  Serial.println("\n=== GET OFFSET ===");
  Serial.println("Remove Weight");
  Serial.println("Press Enter when ready");
  while(!Serial.available());
  Serial.read();

  delay(2000);

  scale.set_scale();

  Serial.println("Measuring offset...");
  scale.tare(50);

  long offset = scale.get_offset();

  Serial.println("\n=== COPY THESE VALUES ===");
  Serial.print("const long OFFSET = ");
  Serial.print(offset);
  Serial.println("L;");
}

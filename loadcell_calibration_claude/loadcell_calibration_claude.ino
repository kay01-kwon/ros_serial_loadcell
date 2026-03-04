#include <HX711.h>

const int DOUT_PIN1 = 2;
const int SCK_PIN1 = 3;
const int DOUT_PIN2 = 4;
const int SCK_PIN2 = 5;

HX711 scale1, scale2;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  scale1.begin(DOUT_PIN1, SCK_PIN1);
  scale2.begin(DOUT_PIN2, SCK_PIN2);
  
  Serial.println("=== Calibration Mode ===");
  Serial.println("Send 'C' to calibrate");
  Serial.println("Send 'O' to get offset");
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == 'C') {
      calibrate();
    }
    else if (cmd == 'O') {
      getOffset();
    }
  }
}

void calibrate() {
  Serial.println("\n=== CALIBRATION ===");
  Serial.println("Step 1: Remove ALL weights");
  Serial.println("Press Enter when ready");
  while (!Serial.available());
  Serial.read();
  
  Serial.println("Taring...");
  scale1.tare(50);
  scale2.tare(50);
  Serial.println("Tare done!");
  
  Serial.println("\nStep 2: Place 1000g on LoadCell 1");
  Serial.println("Press Enter when ready");
  while (!Serial.available());
  Serial.read();
  
  delay(3000);
  float reading1 = scale1.get_units(50);
  float scale_factor_1 = reading1 / 1000.0;
  
  Serial.print("\nReading: ");
  Serial.println(reading1);
  Serial.print("SCALE_FACTOR_1 = ");
  Serial.println(scale_factor_1, 4);
  
  Serial.println("\nStep 3: Move 1000g to LoadCell 2");
  Serial.println("Press Enter when ready");
  while (!Serial.available());
  Serial.read();
  
  delay(3000);
  float reading2 = scale2.get_units(50);
  float scale_factor_2 = reading2 / 1000.0;
  
  Serial.print("\nReading: ");
  Serial.println(reading2);
  Serial.print("SCALE_FACTOR_2 = ");
  Serial.println(scale_factor_2, 4);
  
  Serial.println("\n=== COPY THESE VALUES ===");
  Serial.print("const float SCALE_FACTOR_1 = ");
  Serial.print(scale_factor_1, 4);
  Serial.println(";");
  Serial.print("const float SCALE_FACTOR_2 = ");
  Serial.print(scale_factor_2, 4);
  Serial.println(";");
}

void getOffset() {
  Serial.println("\n=== GET OFFSET ===");
  Serial.println("Remove ALL weights (empty load cells)");
  Serial.println("Press Enter when ready");
  while (!Serial.available());
  Serial.read();
  
  delay(2000);
  
  // Scale factor가 없으면 set_scale() 필요
  // 지금은 raw offset만 구함
  scale1.set_scale();  // scale = 1
  scale2.set_scale();
  
  Serial.println("Measuring offset...");
  scale1.tare(50);
  scale2.tare(50);
  
  long offset1 = scale1.get_offset();
  long offset2 = scale2.get_offset();
  
  Serial.println("\n=== COPY THESE VALUES ===");
  Serial.print("const long OFFSET_1 = ");
  Serial.print(offset1);
  Serial.println("L;");
  Serial.print("const long OFFSET_2 = ");
  Serial.print(offset2);
  Serial.println("L;");
}
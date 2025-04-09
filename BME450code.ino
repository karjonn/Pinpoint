#define PHOTO_PIN A0  // Adjust if your photodiode is on another pin
#define THRESHOLD 25  // Example threshold for detection, needs tuning

int baseline = 0;
bool calibrated = false;

void setup() {
  Serial.begin(9600);
  delay(10000); // Wait for serial monitor
  Serial.println("Place normal tissue above the sensor to calibrate...");
  
  // Calibration step
  int sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(PHOTO_PIN);
    delay(10);
  }
  baseline = sum / 100;
  calibrated = true;

  Serial.print("################################");
  Serial.print("Calibration complete. Baseline: ");
  Serial.println("Baseline: "+ baseline);
}

void loop() {
  if (!calibrated) return;

  Serial.print("How many readings will you be taking ");
  int numReadings = Serial.parseInt();

  int reading = 0;
  Serial.println("----Starting Reading----");
  for (int i = 0; i < numReadings; i++){
    for (int j = 0; j < 100; j++) {
      reading += analogRead(PHOTO_PIN);
      delay(10);
    }
    Serial.println("----ROTATE POLARIZER----");
    delay(5000);
  }

  reading = reading / (100 * numReadings);
  int delta = reading - baseline;

  Serial.print("Reading: ");
  Serial.print(reading);
  Serial.print(" | Delta: ");
  Serial.println(delta);

  // threshold check
  if (abs(delta) > THRESHOLD) {
    Serial.println("🔴 Possible Lipohypertrophy Detected!");
  } else {
    Serial.println("🟢 Normal Tissue Detected.");
  }

  delay(2000);
  Serial.print("Do you want to continue(y/n)");
  String ans = Serial.readStringUntil('\n');
}









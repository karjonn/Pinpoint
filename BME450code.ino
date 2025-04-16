#define PHOTO_PIN A0  // Adjust if your photodiode is on another pin
#define THRESHOLD 0  // Example threshold for detection, needs tuning

#include <vector>
#include <algorithm>

std::vector<int> baseline = {};
bool calibrated = false;
int calibrationRange = 0;
int numOrientations = 0;
int numReadings = 500;

// Correct function definitions
int getMax(const std::vector<int>& nums) {
  return *std::max_element(nums.begin(), nums.end());
}

int getMin(const std::vector<int>& nums) {
  return *std::min_element(nums.begin(), nums.end());
}

String vectorToString(const std::vector<int>& vec) {
  String result = "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    result += String(vec[i]);
    if (i < vec.size() - 1) result += ", ";
  }
  result += "]";
  return result;
}

std::vector<int> takeReadings(int samplingCount) {
  std::vector<int> readings = {};
  delay(5000);
  if(!calibrated) samplingCount++;
  for (int i = 0; i < samplingCount; i++) {
    int temp = 0;
    Serial.println("Reading...");
    for (int j = 0; j < numReadings; j++) {
      temp += analogRead(PHOTO_PIN);
      delay(10);
    }
    temp /= numReadings;

    if (samplingCount > 1) {
      Serial.println("----ROTATE POLARIZER----");
    }

    delay(5000);
    readings.push_back(temp);
  }

  Serial.print("Readings = ");

  for (size_t i = 0; i < readings.size(); i++) {
    if(!(i == 0 && !calibrated)){
      Serial.print(readings[i]);
      Serial.print(", ");
    }
  }
  Serial.println();


  if(!calibrated){
    return std::vector<int>(readings.begin() + 1, readings.end());
  }
  
  return readings;
}

void calibrate() {
  Serial.println("Place normal tissue above the sensor to calibrate...");
  delay(5000);

  Serial.println("How many readings will you be taking?");
  while (Serial.available() == 0);
  numOrientations = Serial.parseInt(); // set global value here
  Serial.print("You entered: ");
  Serial.println(numOrientations);

  baseline = takeReadings(numOrientations);
  calibrationRange = getMax(baseline) - getMin(baseline);
  calibrated = true;

  Serial.println("################################");
  Serial.print("Calibration complete. Baseline: ");
  Serial.println(vectorToString(baseline));
}

void setup() {
  Serial.begin(9600);
  delay(5000); // Wait for serial monitor
  calibrate();
}

void loop() {
  if (!calibrated) return;

  std::vector<int> readings = takeReadings(numOrientations);
  int range = getMax(readings) - getMin(readings);
  int delta = range - calibrationRange;

  Serial.print("Range delta: ");
  Serial.println(delta);

  if (delta < (THRESHOLD * -1)) {
    Serial.println("🔴 Possible Lipohypertrophy Detected!");
  } else {
    Serial.println("🟢 Normal Tissue Detected.");
  }

  delay(2000);
  Serial.println("Do you want to continue or change the number of samples? (y/n/c to change samples)");
  while (Serial.available() == 0);
  String ans = Serial.readStringUntil('\n');
  ans.trim();

  if (ans == "n" || ans == "N") {
    Serial.println("Exiting...");
    while (true);  // Halt
  }
  if (ans == "c" || ans == "C") {
    calibrated = false;
    calibrate();
  }
}
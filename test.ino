#define PHOTO_PIN A0  // Photodiode pin
#define THRESHOLD 25  // Threshold

int baseline = 0;
bool calibrated = false;

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//Bluetooth funciton(do not touch)
int scanTime = 5; //In seconds
BLEScan* pBLEScan;
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};


void setup() {
  Serial.begin(115200);

  // Bluetooth setup
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  delay(2000); // Wait for serial monitor

  // Begin calibration
  Serial.println("Place normal tissue above the sensor to calibrate...");
  
  // Calibration step
  int sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(PHOTO_PIN);
    delay(10);
  }
  baseline = sum / 100;
  calibrated = true;

  Serial.print("Calibration complete. Baseline: ");
  Serial.println(baseline);
}

void loop() {
  if (!calibrated) return;

  int reading = 0;
  for (int i = 0; i < 100; i++) {
    reading += analogRead(PHOTO_PIN);
    delay(10);
  }
  reading = reading / 100;
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

  //BLUETOOTH Stuff (goes at end of loop)
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(10000);

}
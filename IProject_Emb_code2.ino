#include <FallDetect_inferencing.h>
#include "LSM6DS3.h"
#include "Wire.h"
#include <ArduinoBLE.h>

LSM6DS3 xIMU(I2C_MODE, 0x6A);

// BLE setup
BLEService fallService("180D");
BLEStringCharacteristic fallCharacteristic("2A37", BLERead | BLENotify, 50);

// Inference buffer
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
int featureIndex = 0;

// Timing
unsigned long lastSample = 0;

void setup() {
  delay(1000);

  xIMU.begin();

  BLE.begin();
  BLE.setLocalName("FallDetector");
  BLE.setAdvertisedService(fallService);
  fallService.addCharacteristic(fallCharacteristic);
  BLE.addService(fallService);
  fallCharacteristic.writeValue("ready");
  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    while (central.connected()) {

      // ----------------- SAMPLE AT 100Hz -----------------
      unsigned long now = millis();
      if (now - lastSample < 10) continue;
      lastSample = now;

      // ----------------- SENSOR READ -----------------
      features[featureIndex++] = xIMU.readFloatAccelX();
      features[featureIndex++] = xIMU.readFloatAccelY();
      features[featureIndex++] = xIMU.readFloatAccelZ();
      features[featureIndex++] = xIMU.readFloatGyroX();
      features[featureIndex++] = xIMU.readFloatGyroY();
      features[featureIndex++] = xIMU.readFloatGyroZ();

      // ----------------- RUN INFERENCE WHEN BUFFER FULL -----------------
      if (featureIndex >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        featureIndex = 0;

        signal_t signal;
        numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

        ei_impulse_result_t result;
        run_classifier(&signal, &result, false);

        // Find highest confidence label
        float maxVal = 0;
        const char* maxLabel = "";
        for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
          if (result.classification[i].value > maxVal) {
            maxVal = result.classification[i].value;
            maxLabel = result.classification[i].label;
          }
        }

        // fall and almost both trigger "fall", everything else is "safe"
        bool isFall = (strcmp(maxLabel, "fall") == 0 || strcmp(maxLabel, "almost") == 0)
                      && maxVal > 0.80;
        fallCharacteristic.writeValue(isFall ? "fall" : "safe");
      }
    }
  }
}
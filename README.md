---

## How It Works

### 1. Data Collection — `CollectDATAwireless.ino`
Labelled IMU data was collected using a button-triggered recording firmware. 
Each press starts a new CSV session saved to an SD card, capturing 6-axis motion 
data at 100Hz. Files are labelled by action class (e.g. `front0342.txt`, 
`back7819.txt`) for use in the Edge Impulse training pipeline.

**Action classes collected:**
- `fall` — forward and backward falls
- `almost` — near-fall stumbles
- `stand` — standing still
- `sit` — sitting down
- `walk` — normal walking

---

### 2. ML Model — Edge Impulse (FallDetect)
The labelled dataset was uploaded to Edge Impulse for feature extraction, 
model design, and training.

| Detail | Value |
|---|---|
| Sensors | accX, accY, accZ, gyroX, gyroY, gyroZ @ 100Hz |
| Total data collected | 13 minutes 27 seconds |
| Training samples | 320 samples |
| Labels | fall, almost, sit, stand, walk |
| Model type | Neural Network Classifier |
| DSP block | Spectral Features + Raw Data |
| Validation accuracy | **98.0%** |
| Test set accuracy | **96.9%** |
| Inference latency | **50ms** (Nordic nRF52840, Cortex-M4F 64MHz) |
| Peak RAM usage | 7.9K |
| Flash usage | 33.5K |

🔗 [View the public Edge Impulse project](https://studio.edgeimpulse.com/public/955211/live)

---

### 3. Inference & BLE — `FallDetect_inference.ino`
The trained model was exported as an Arduino library via Edge Impulse and deployed 
on the XIAO microcontroller. The firmware:

- Samples IMU data at 100Hz into a rolling inference buffer
- Runs the Edge Impulse classifier when the buffer is full
- Triggers a fall alert if the top prediction is `fall` or `almost` with 
  confidence above **80%**
- Broadcasts the result (`"fall"` or `"safe"`) over BLE to a connected device

---

## Hardware

- Seeed XIAO nRF52840 Sense (Cortex-M4F, BLE)
- LSM6DS3 IMU (accelerometer + gyroscope, I2C)
- SD card module (for data collection phase)
- Push button (for recording trigger)

---

## Software & Tools

- Arduino IDE + Edge Impulse Arduino Library
- Edge Impulse Studio (data collection, DSP, model training, deployment)
- Python (signal processing and ML pipeline development)
- Android application (BLE companion app for real-time alerts)

---

## Key Outcomes

- Achieved **98% validation accuracy** and **96.9% test accuracy** on a 5-class 
  motion classification task
- Deployed a quantised int8 model running at **50ms latency** on a Cortex-M4F 
  microcontroller with only **7.9K RAM**
- Built a complete end-to-end pipeline from raw sensor data collection through 
  to real-time BLE-enabled wearable deployment
- Developed a companion Android app to receive and display fall alerts wirelessly

---

## Author

**Krishna Praveen Kasilingam**  
MSc Embedded and Control Systems Engineering — University of Leicester  
📧 krishnapraveenka@gmail.com  
🌐 [Portfolio](https://krishnapraveen-k.github.io/portfolio)  
💼 [LinkedIn](https://linkedin.com/in/krishna-praveen-k)

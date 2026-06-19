#include "LSM6DS3.h"
#include "Wire.h"
#include <SPI.h>
#include "SdFat.h"

// -----------------------------------------------
// CHANGE THIS BEFORE EACH ACTION TYPE
char actionPrefix[] = "back"; //front fall, back fall, stand, sit
// -----------------------------------------------

LSM6DS3 xIMU(I2C_MODE, 0x6A);

SdFat SD;
#define SD_CS_PIN D2
#define BUTTON_PIN D1

char filename[20];
File myFile;
bool recording = false;
unsigned long startTime = 0;

void setup() 
{
  //Buffer Time
  delay(1000); 

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(A0));

  xIMU.begin(); // Initializing IMU

  SD.begin(SD_CS_PIN); // Initializing SD card
}

void loop() 
{

  // ----------------- BUTTON -----------------
  // When the button is pressed this process will start
  // and it will stop recording when the button is the second time
  // They work in a pair of two actions
  if (digitalRead(BUTTON_PIN) == LOW) 
  {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) 
    {
      if (!recording) 
      {
        sprintf(filename, "%s%04d.txt", actionPrefix, (int)random(10000));
        myFile = SD.open(filename, FILE_WRITE); // opening a file to write data
        if (myFile) 
        {
          myFile.print("time,accX,accY,accZ,gyroX,gyroY,gyroZ\n");
          startTime = 0;
          recording = true;
        }

      } 
      else 
      {
        myFile.flush();
        myFile.close(); // closing the file
        recording = false;
      }

      while (digitalRead(BUTTON_PIN) == LOW);
    }
  }

  if (!recording) return;

  // ----------------- TIMING (100Hz) -----------------
  static unsigned long lastTime = 0;
  unsigned long now = millis();
  if (now - lastTime < 10) return;
  lastTime = now;

  if (startTime == 0) startTime = lastTime;
  float time_sec = (lastTime - startTime) / 1000.0;// so that the timing is recorded from 0.0sec and not in millis

  // ----------------- SENSOR READ -----------------
  float x  = xIMU.readFloatAccelX();
  float y  = xIMU.readFloatAccelY();
  float z  = xIMU.readFloatAccelZ();
  float gx = xIMU.readFloatGyroX();
  float gy = xIMU.readFloatGyroY();
  float gz = xIMU.readFloatGyroZ();

  // ----------------- SD WRITE -----------------
  myFile.print(time_sec, 3); myFile.print(",");
  myFile.print(x);           myFile.print(",");
  myFile.print(y);           myFile.print(",");
  myFile.print(z);           myFile.print(",");
  myFile.print(gx);          myFile.print(",");
  myFile.print(gy);          myFile.print(",");
  myFile.println(gz);
  // -------------Flush every 100 samples--------
  static int flushCount = 0;
  if (++flushCount >= 100) 
  {
    myFile.flush();
    flushCount = 0;
  }
}

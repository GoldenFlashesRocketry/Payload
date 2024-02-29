////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib-Arduino
//
//  Copyright (c) 2014-2015, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of 
//  this software and associated documentation files (the "Software"), to deal in 
//  the Software without restriction, including without limitation the rights to use, 
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
//  Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all 
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//Modified by Tristan Magel 2023
//Modified by Jeremy Cwik 2024

//V1.0 11/10/2023
//Changed display functions to myFile.print()
//Also breaks up vectors into x,y,z components
//Test on 11/14/2023
//Work to Do:
//How to integrate pitch, yaw, and roll and linear acceleration so frame of reference stays static

#include <Wire.h>
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h" 
#include "CalLib.h"
#include <EEPROM.h>
#include <SD.h>
#include <string.h>
#include "GY521.h"


RTIMU *imu;                                           // the IMU object
RTFusionRTQF fusion;                                  // the fusion object
RTIMUSettings settings;                               // the settings object

const int buttonPin = 2;  // the number of the pushbutton pin
int buttonState = 0;  // variable for reading the pushbutton status
const int ledpin = 3; // number for LED pin
int i = 0;


//  DISPLAY_INTERVAL sets the rate at which results are displayed

#define DISPLAY_INTERVAL  40                         // interval between pose displays

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port

#define  SERIAL_PORT_SPEED  115200

const int chipSelect = 10;
unsigned long lastDisplay;
unsigned long lastRate;
int sampleCount;

File myFile;

void setup()
{
    int errcode;
  
    Serial.begin(SERIAL_PORT_SPEED);
    pinMode(ledpin, OUTPUT);
    pinMode(buttonPin, INPUT);
    

    Wire.begin();
    imu = RTIMU::createIMU(&settings);                       // create the imu object
  
    myFile.print("ArduinoIMU starting using device "); Serial.println(imu->IMUName());
    if ((errcode = imu->IMUInit()) < 0) {
        myFile.print("Failed to init IMU: "); Serial.println(errcode);
    }
    if (imu->getCalibrationValid())
        Serial.println("Using compass calibration");
    else
        Serial.println("No valid compass calibration data");
        if (SD.begin(chipSelect))
    {
    Serial.println("SD card is present & ready");
    } 
    else
    {
    Serial.println("SD card missing or failure");
    while(1);  //wait here forever
    }

    lastDisplay = lastRate = millis();
    sampleCount = 0;


    // Slerp power controls the fusion and can be between 0 and 1
    // 0 means that only gyros are used, 1 means that only accels/compass are used
    // In-between gives the fusion mix.
    
    fusion.setSlerpPower(0.25); // originally 0.02
    
    // use of sensors in the fusion algorithm can be controlled here
    // change any of these to false to disable that sensor
    
    fusion.setGyroEnable(true);
    fusion.setAccelEnable(true);
    fusion.setCompassEnable(false); // mag
    buttonState = digitalRead(buttonPin);

    while (buttonState == false) {
      buttonState = digitalRead(buttonPin);
      delay(1000);
    }
    
    while (i <= 20) {
      digitalWrite(ledpin, LOW);
      delay(200);
      digitalWrite(ledpin, HIGH);
      delay(200);
      i = i + 1;
    }
    digitalWrite(ledpin, HIGH);

}

void loop()
{  
    float now = millis();
    float delta;
    float time;
    int loopCount = 1;
    buttonState = digitalRead(buttonPin);
 
    while (imu->IMURead()) {                                // get the latest data if ready yet
        // this flushes remaining data in case we are falling behind
        if (++loopCount >= 10)
            continue;
        fusion.newIMUData(imu->getGyro(), imu->getAccel(), imu->getCompass(), time = imu->getTimestamp());
        sampleCount++;
        if ((delta = now - lastRate) >= 1000) {
            Serial.print("Sample rate: "); Serial.print(sampleCount);
            if (imu->IMUGyroBiasValid())
                Serial.println(", gyro bias valid");
            else
                Serial.println(", calculating gyro bias");
        
            sampleCount = 0;
            lastRate = now;
        }
        if ((now - lastDisplay) >= DISPLAY_INTERVAL) {
            lastDisplay = now;
           RTVector3& gyro = (RTVector3&)imu->getGyro(); //gets gyro data
           RTVector3& accel = (RTVector3&)imu->getAccel(); //gets accel data
           RTVector3& mag = (RTVector3&)imu->getCompass(); //gets compass/magnetometer data
           RTVector3& pose = (RTVector3&)fusion.getFusionPose(); //gets pose (pitch,yaw,roll) data


          myFile = SD.open("datalog.csv", FILE_WRITE);



           
           //Outputs gyro data in rad/s (x,y,z)
           myFile.print(time);
           myFile.println();
           myFile.println();
           myFile.print(gyro.x());
           myFile.print(", ");
           myFile.print(gyro.y());
           myFile.print(", ");
           myFile.println(gyro.z());

           //Outputs accel data in m/s^2 (x,y,z)
           myFile.println();
           myFile.print(accel.x());
           myFile.print(", ");
           myFile.print(accel.y());
           myFile.print(", ");
           myFile.println(accel.z());
/*
           //Outputs mag/compass data in micro Tesla (uT):(x,y,z)
           myFile.print("Mag: ");
           myFile.println();
           myFile.print(mag.x());
           myFile.print(", ");
           myFile.print(mag.y());
           myFile.print(", ");
           myFile.println(mag.z());
*/
           //Outputs pose data in degrees (pitch, yaw, roll),(+/-180)
           myFile.println();
           myFile.print(pose.x());
           myFile.print(", ");
           myFile.print(pose.y());
           myFile.print(", ");
           myFile.println(pose.z());
           myFile.println();

           if (buttonState == true) {
            delay(1000);
            if (buttonState == true) {
              digitalWrite(ledpin, LOW);
              while(1){
                delay(1);
              }
            }
           }

          

           //myFile.flush(); // may not be needed
           myFile.close();
        }
    }

}
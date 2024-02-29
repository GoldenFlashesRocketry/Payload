/***************************************************************************
 This is a library for the BMP280 humidity, temperature & pressure sensor

 Designed specifically to work with the Adafruit BMP280 Breakout
 ----> http://www.adafruit.com/products/2651

 These sensors use I2C or SPI to communicate, 2 or 4 pins are required
 to interface.

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing products
 from Adafruit!

 Written by Limor Fried & Kevin Townsend for Adafruit Industries.
 BSD license, all text above must be included in any redistribution

 //Combined BMP280 and Magnetometer code
 //Unifies sensor data into single output

 //2/1/2024
 //Code running and detect change in magentic field
****************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <LIS3MDL.h>
#include <SD.h>

#define BMP_SCK (2)
#define BMP_MISO (3)
#define BMP_MOSI (4)
#define BMP_CS   (5)

Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK);
LIS3MDL mag;
File myFile;                 

char t[30];
//char report[80];

unsigned long sec;
int i = 0;


void setup() {
  // Checks multiplexer ports and detection 
  
    
    delay(1000);
    Wire.begin();
    Serial.begin(115200);

    Serial.println("\ndone"); 
 Serial.begin(115200);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 while ( !Serial ) delay(100);   // wait for native usb
 Serial.println(F("BMP280 test"));
 unsigned status;
 
 status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
 status = bmp.begin();
 if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
 }

 /* Default settings from datasheet. */
 bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                 Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                 Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                 Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                 Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

 Wire.begin();
 if (!mag.init())
 {
    Serial.println("Failed to detect and initialize magnetometer!");
    while (1);
 }

 mag.enableDefault();
 delay(5000);

}

void loop() {
    myFile = SD.open("datalog.csv", FILE_WRITE);
    Serial.print("Temp: ");
    Serial.print(bmp.readTemperature());
    Serial.println(" ");

    Serial.print("Pressure: ");
    Serial.print(bmp.readPressure());
    Serial.println(" ");

    Serial.print("Alt: ");
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" ");

    mag.read();

    sec = millis()/1000;

    //Snprintf(report, sizeof(report), "M: %6d %6d %6d", mag.m.x, mag.m.y, mag.m.z);
    //Serial.println(report);
    Serial.print("MagX: ");
    Serial.print(mag.m.x);
    Serial.println(" ");
    Serial.print("MagY: ");
    Serial.print(mag.m.y);
    Serial.println(" ");
    Serial.print("MagZ: ");
    Serial.print(mag.m.z);
    Serial.println(" ");

    myFile.print("Temp:, ");
    myFile.print(bmp.readTemperature());
    myFile.println(" ");

    myFile.print("Pressure:, ");
    myFile.print(bmp.readPressure());
    myFile.println(" ");

    myFile.print("Alt:, ");
    myFile.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    myFile.println(" ");

    mag.read();

    myFile.print("MagX:, ");
    myFile.print(mag.m.x);
    myFile.println(" ");
    myFile.print("MagY:, ");
    myFile.print(mag.m.y);
    myFile.println(" ");
    myFile.print("MagZ:, ");
    myFile.print(mag.m.z);
    myFile.println(" ");
    delay(25);
    myFile.print("Iterations:, ");
    Serial.print("Iterations: ");
    i = i + 1;
    Serial.print(i);
    Serial.println();
    myFile.print(i);
    myFile.println();
    Serial.print(sec);
    Serial.println();
    myFile.print("Time Elapsed:, ");
    myFile.print(sec);
    myFile.println();
    

    myFile.close();
}

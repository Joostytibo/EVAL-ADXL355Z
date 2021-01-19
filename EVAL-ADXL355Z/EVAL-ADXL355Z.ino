/*
  Name: EVAL-ADXL355Z
  Date: November 3, 2020
  Author: Joost Lassche
  
  Description: EVAL_ADXL355Z Accelerometer

  Revision: V1.4.3

  Rev update: V1.0    November  3, 2020       *--*  Written the main code
  Rev update: V1.1    November 10, 2020       *--*  Added Bit Shifting to the code
  Rev update: V1.2    November 19, 2020       *--*  Added enable function
  Rev update: V1.3    November 24, 2020       *--*  Added calculations of roll, pitch and tilt
  Rev update: V1.3.1  December  9, 2020       *--*  Changes to pitch and tilt calculations
  Rev update: V1.3.2  December 17, 2020       *--*  Changes to pitch and tilt readings on serial monitor
  Rev update: V1.4    January   7, 2021       *--*  Added Reset function 
  Rev update: V1.4.1  January   8, 2021       *--*  Changes to Reset function
  Rev update: V1.4.2  January  12, 2021       *--*  Changes to calculations of roll, pitch and tilt
  Rev update: V1.4.3  January  13, 2021       *--*  Changes to function Processing354
*/
 
// include libraries
#include <Wire.h>                       // Wire library - used for I2C communication
#include <math.h>                       // Math library - used for math calculations

// ------------------------- Declare Constants ------------------------- //
#define STATUS        0x04              // define STATUS as 0x04
#define XDATA3        0x08              // define XDATA3 as 0x08
#define XDATA2        0x09              // define XDATA2 as 0x09
#define XDATA1        0x0A              // define XDATA1 as 0x0A
#define YDATA3        0x0B              // define YDATA3 as 0x0B
#define YDATA2        0x0C              // define YDATA2 as 0x0C
#define YDATA1        0x0D              // define YDATA1 as 0x0D
#define ZDATA3        0x0E              // define ZDATA3 as 0x0E
#define ZDATA2        0x0F              // define ZDATA2 as 0x0F
#define ZDATA1        0x10              // define ZDATA1 as 0x10

#define RANGE         0x2C              // define RANGE as 0x2C
#define POWER_CTL     0x2D              // define POWER_CTL as 0x2D, power_register address
#define RANGE_2G      2.048             // define RANGE_2G as decimal 2.048
#define RANGE_4G      4.096             // define RANGE_3G as decimal 4.096
#define RANGE_8G      8.192             // define RANGE_8G as decimal 8.192

#define Pi            3.1415926535      // define Pi as math PI --> 3.1415926535

// ------------------------- Declare Global Variables ------------------------- // 
int ADXL355 = 0x53;                     // ADXL355 I2C address - 0x53
float x,y,z;                            // x,y,z initiated as float
float range;                            // range initiated as float
float rollF,pitchF=0;                   // rollF,pitchF initiated as float
uint8_t enable(uint8_t);                // enable initiated as an unidentified integer

// Setup
void setup() 
{   
  Serial.begin(9600);                   // Initiate serial communication
  Wire.begin();                         // Initiate the Wire library
  reset();                              // call function reset()
  enable(0x00);                         // call function enable() and write 0X00
  Serial.println("Started");            // Print "Started" on the serial monitor
}

// Main loop
void loop() 
{
  /*      There are 2 functions which you can use. 
   *      The data in function 'Processing354' can be used if you want to have a 3D-visualization of the accelerometer in Processing 3.5.4 --> https://processing.org/download/
   *      Only the function 'Processing354' can be used to make the program work in Processing 3.5.4
   *      The data in function 'AccelerationData' can be used if you want to have the acceleration data printed on the serial monitor
   *      Comment out the one you are not using.
   */     
  
  //Processing354();
  AccelerationData();
  
}

// function readOrientation
float readOrientation(unsigned long orientation) {
  Wire.beginTransmission(ADXL355);                  // begin data transmission from ADXL355
  Wire.write(orientation);                          // write hexadecimal constants XDATA3, YDATA3 or ZDATA3 containing x, y or z
  Wire.endTransmission();                           // end data transmission
  Wire.requestFrom((int)ADXL355,3);                 // request 3 registers from ADXL355
  uint32_t v1 = Wire.available()?Wire.read():0;     // Ternary operator - if wire is available then read value of v1 else write 0
  uint32_t v2 = Wire.available()?Wire.read():0;     // Ternary operator - if wire is available then read value of v2 else write 0
  uint32_t v3 = Wire.available()?Wire.read():0;     // Ternary operator - if wire is available then read value of v3 else write 0 
  uint32_t data = (v1<<16)|(v2<<8)|v3;              // Bit shift v1 16 bits to the left, v2 8 bits to the left
  data = data>>4;                                   // Bit shift data 4 bits to the right
  float value = (float)data;                        // value = data
  value = data>0X7FFFF?value-0XFFFFE:value;         // Ternary operator - if data is greater than 0x7FFFF then value-0XFFFFE else write value
  return value/0X7FFFF*RANGE_2G;                    // returns value divided by 0X7FFFF times RANGE_2G, which is decimal 2.048 
}

// function enable
uint8_t enable(uint8_t c = 0X00) { 
  Wire.beginTransmission(ADXL355);                  // begin data transmission from ADXL355
  if (Wire.endTransmission()==0) {                  // if end data transmission from ADXL355 is equal to 0, then execute the next code
    Wire.beginTransmission(ADXL355);                // begin data transmission from ADXL355
    Wire.write(0X00);                               // write address 0X00
    Wire.endTransmission();                         // end data transmission
    Wire.requestFrom((int)ADXL355,1);               // request 1 register from ADXL355
    if (Wire.available()) {                         // if data transmission is available, then execute the next code
      if (Wire.read()==0XAD) {                      // if the readings from the data is equal to 0XAD, then execute the next code
        Serial.println("Device found");               // print "Device found" on serial monitor
      }
      else {
        return 1;                                   // return 1
      }
    }
  }
  else {
    return 0;                                       // return 0
  }
  Wire.beginTransmission(ADXL355);                  // begin data transmission from ADXL355
  Wire.write(POWER_CTL);                            // writes address 0X2D from the accelerometer to the Arduino
  Wire.write(c);                                    // write 0X00
  Wire.endTransmission();                           // end data transmission
  return 1;                                         // return 1
}

// function Processing354
void Processing354() {
  
  // Low-pass filter
  rollF = 0.94 * rollF + 0.06 * roll();             // rollF is a low-pass filter for function roll()
  pitchF = 0.94 * pitchF + 0.06 * pitch();          // pitchF is a low-pass filter for function pitch()
  
  Serial.print(rollF);                              // print rollF to the serial monitor
  Serial.print("/");                                // print "/" to the serial monitor
  Serial.println(pitchF);                           // print pitchF to the serial monitor
}

// function AccelerationData
void AccelerationData() {
  Serial.println();                                           // print a (new) line on the serial monitor
  Serial.print("Accelerometer ADXL355 (x,y,z): ");            // print "Accelerometer ADXL355 (x,y,z): " tot the serial monitor
  Serial.print(readOrientation(XDATA3)); Serial.print(",");   // print function readOrientation with data from XDATA3 on the serial monitor
  Serial.print(readOrientation(YDATA3)); Serial.print(",");   // print function readOrientation with data from YDATA3 on the serial monitor
  Serial.print(readOrientation(ZDATA3));                      // print function readOrientation with data from ZDATA3 on the serial monitor
  
  Serial.println();                     // print a (new) line on the serial monitor

  Serial.print("Roll: ");               // print "Roll: " to the serial monitor
  Serial.println(roll());               // print function roll() to the serial monitor
  Serial.print("Pitch: ");              // print "Pitch: " to the serial monitor
  Serial.println(pitch());              // print function to pitch() the serial monitor
  //Serial.print("tilt: ");              // print "yaw: " to the serial monitor
  //Serial.println(tilt());              // print function yaw() to the serial monitor
  delay(200);                           // a delay of 500 milliseconds
}

// function reset
void reset() {
  Wire.beginTransmission(ADXL355);                  // begin data transmission from ADXL355
  Wire.write(0x2f);                                 // writes address 0X2f from the accelerometer to the Arduino
  Wire.write(0x52);                                 // writes code 0x52 to reset the device
  Wire.endTransmission();                           // end data transmission
}

// function readData
void readData() {
  while(dataReady()==0);                            // while the function dataReady is equal to 0, then execute next code

  Wire.beginTransmission(ADXL355);                  // begin data transmission from ADXL355
  Wire.write(XDATA3);                               // writes address 0X08 from the accelerometer to the Arduino
  Wire.endTransmission();                           // end data transmission
  Wire.requestFrom(ADXL355,9);                      // request 9 registers from ADXL355
  
  x = readAxis();                                   // x uses function readAxis()
  y = readAxis();                                   // y uses function readAxis()
  z = readAxis();                                   // z uses function readAxis()
}

// function readAxis
float readAxis() {
  
  float value;                                      // value is a float
  uint32_t d3,d2,d1,data;                           // d3,d2,d1,data are uint32_t - unidentified integers, 32 bits/4 bytes
  
  d3 = Wire.available()?Wire.read():0;              // Ternary operator - if wire is available then read value of d3 else write 0
  d2 = Wire.available()?Wire.read():0;              // Ternary operator - if wire is available then read value of d2 else write 0
  d1 = Wire.available()?Wire.read():0;              // Ternary operator - if wire is available then read value of d1 else write 0
  data = (d3<<16)|(d2<<8)|d1;                       // Bit shift d3 16 bits to the left, d2 8 bits to the left
  data = data>>4;                                   // Bit shift data 4 bits to the right
  value = (float)data;                              // value = data
  value = data>0X7FFFF?value-0XFFFFE:value;         // Ternary operator - if data is greater than 0x7FFFF then value-0XFFFFE else write value
  return value/0X7FFFF*RANGE_2G;                    // returns value divided by 0X7FFFF times RANGE_2G, which is decimal 2.048
}

// function dataReady
uint8_t dataReady() {
  Wire.beginTransmission(ADXL355);                  // begin data transmission from ADXL355
  Wire.write(STATUS);                               // writes address 0X04 from the accelerometer to the Arduino
  Wire.endTransmission();                           // end data transmission
  Wire.requestFrom(ADXL355,1);                      // request 1 register from ADXL355
  return Wire.available()?Wire.read()&0X01:0X00;    // returns value from STATUS and write 0X01 if wire is available, else write 0X00
}

// calculation of roll
float roll() {
  readData();                                       // call function readData()
  return atan2(y,z)*180/Pi;                         // returns value in arctan with values y and z times 180 divided by Pi
}

// calculation of pitch
float pitch() {
  readData();                                                 // call function readData()                 
  return atan(-1 * x / sqrt(pow(y, 2) + pow(z, 2)))*180/Pi;   // returns value in arctan with values x and y times 180 divided by Pi
}

// calculation of tilt
float tilt() {
  readData();                                       // call function readData()
  //return atan2(x,z)*180/Pi;                       // returns value in arctan with values x and z times 180 divided by Pi
  return atan(sqrt(pow(x, 2) + pow(y, 2))/z)*180/Pi;
}

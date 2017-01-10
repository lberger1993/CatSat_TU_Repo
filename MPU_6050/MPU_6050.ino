#include<Wire.h>

// general adderss declarations
const int MPU_6050_addr = 0x68;
const byte PWR_MNG_1 = 0x68;;
const byte WHO_AM_I = 0x75;
const byte TMP_SENSOR_IN = 0x41;
const byte TMP_SENSOR_OUT = 0x42;

/* 
 *  WHO_AM_IContains the 6-bit I2C address of the MPU-60X0. 
 *  DATA: The Power-On-Reset value of Bit6:Bit1 is 110 100.
 * 
  */
byte WHO_AM_I_data;
byte temp_data;

void setup() {
  // put your setup code here, to run once:
  
  Wire.begin();
  Wire.beginTransmission(MPU_6050_addr);
  
  // Wire library declare
  Wire.write(PWR_MNG_1);
  Wire.write(0);

  
  Wire.endTransmission();
  Serial.begin(9600);
}

void loop() {
 
  // put your main code here, to run repeatedly:
  
  Wire.beginTransmission(MPU_6050_addr);
  //Wire.write(WHO_AM_I);
  // merge the two bytes 
  Wire.write(TMP_SENSOR_OUT);  
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 1);
 
  temp_data = Wire.read();
  Serial.print("Who am I?");
  Serial.println(temp_data );
  
  delay(1000);

}

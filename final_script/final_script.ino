// @ToDO: weigh each piece, place on template
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>

/* MPU CONSTANTS */
const int MPU_ADDR=0x68;
const byte PWR_MGMT_1=0x6b;
const byte WHO_AM_I=0x75;
const byte MPU_ACX_1=0x3B;
const byte MPU_ACY_1=0x3D;
const byte MPU_ACZ_1=0x3F;
const byte MPU_TEMP_1=0x41;
const byte MPU_GYX_1=0x44;

/* MPU VARIABLE CONSTANTS */
const float ACC_SCALE_FACT=4096.;
const float GYR_SCALE_FACT=131.;
const float REF_PRESSURE=1013.25;

/* MPU RESULTS */
byte WHO_AM_I_data;
float acc[3];
float gyro[3];

/* tempLM35 SENSOR */
const int TEMPLM35_SENSOR=A0;

/* tempLM35 VARIABLES */
float tempc;  
float tempf;  
float vout; 

/* Bme SENSOR */
Adafruit_BMP280 bme; 

/* XBee SENSOR */
SoftwareSerial XBee(2, 3); 

/* counter & timer */
int counter = 0; 

void setup() {
  initXBee();
  initB280();
  Wire.begin();
  initMPU6050();
  Serial.begin(9600);
}

void loop() {
  // COUNT, outside TMP,inside TMP0, inside TMP1, Barometer, Accelerator, Gyro
  counter = counter + 1;
  
  Serial.print(counter);
  Serial.print(";");
  
  getTmpLM35();
  readTemp();
  getBaro();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(WHO_AM_I);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,1);
  getAcc();
  
  getGyro();
  Serial.print(counter);
  Serial.println();
//  Serial.print(";");
  delay(1000); 
}

void initXBee() {
   // Configures XBEE
   XBee.begin(9600);
}

void initTMPLM35() {
   // Configures temp35 sensor 
  pinMode(TEMPLM35_SENSOR,INPUT); 
}

void initMPU6050() {
  // Configures MPU 6050  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);  // Set accelerometer precision
  Wire.write(0x10);  // .. to +/- 8 g
  Wire.write(0x1B);  // Set gyroscope precision
  Wire.write(0x07);  // .. to +/- 250 deg/s
  Wire.endTransmission();
}

void initB280() {
  // Configures Barometer
  Wire.begin();
  Serial.begin(9600);
  if (!bme.begin()) {  
    Serial.println(("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  delay(2000);
}

void getAcc() {
  /*** Get Accelerator ***/
  int tmp;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_ACX_1);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR,6);  // request a total of 6 registers
  tmp=Wire.read()<<8|Wire.read();
  acc[0] = tmp / ACC_SCALE_FACT;
  tmp=Wire.read()<<8|Wire.read();
  acc[1] = tmp / ACC_SCALE_FACT;
  tmp=Wire.read()<<8|Wire.read();
  acc[2] = tmp / ACC_SCALE_FACT;
  
  Serial.print(acc[0]);
  Serial.print(";");
//  if (acc[0] == 60){
//     Serial.println("Should be released");
//  }
  Serial.print(acc[1]);
  Serial.print(";");
  Serial.print(acc[2]);
  Serial.print(";");
  
  return;
}

float readTemp() {
  /*** Read Temps  ***/
  int Temp;
  float tempC;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_TEMP_1);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,2);
  Temp= Wire.read() <<8 | Wire.read();
  tempC = Temp/340. + 36.53;
  Serial.print(tempC); 
  Serial.print(";");
  return tempC; 
}

void getGyro() {
  /*** Get Gyro readings ***/
  int tmp;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_GYX_1);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR,6);  // request a total of 6 registers
  tmp=Wire.read()<<8|Wire.read();
  gyro[0] = tmp / GYR_SCALE_FACT;
  tmp=Wire.read()<<8|Wire.read();
  gyro[1] = tmp / GYR_SCALE_FACT;
  tmp=Wire.read()<<8|Wire.read();
  gyro[2] = tmp / GYR_SCALE_FACT;
  Serial.print(gyro[0]);
  Serial.print(";");
  Serial.print(gyro[1]);
  Serial.print(";");
  Serial.print(gyro[2]);
  Serial.print(";");
  return;
}

void getBaro() {
    /*** Get Baro readings ***/
  Serial.print(bme.readTemperature());
  Serial.print(";");
  Serial.print(bme.readPressure());
  Serial.print(";");
  Serial.print(bme.readAltitude(REF_PRESSURE));
  Serial.print(";");// this should be adjusted to your local forcase
}

void getTmpLM35() {
  /*** Get and print temperature ***/
  vout=analogRead(TEMPLM35_SENSOR);
  vout=(vout*500)/1023;
  tempc=vout; // Storing value in Degree Celsius
  Serial.print(tempc);
  Serial.println(";");
}


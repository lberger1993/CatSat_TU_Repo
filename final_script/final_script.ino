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

byte WHO_AM_I_data;
float acc[3];
float gyro[3];

/* tempLM35 sensor */
const int TEMPLM35_SENSOR=A0;

/* tempLM35 VARIABLES */
float tempc;  
float tempf;  
float vout; 

Adafruit_BMP280 bme; // I2C

//@TODO : May need to change this ???
// XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
SoftwareSerial XBee(2, 3); // XBEE

void setup() {

  initXBee();
  initB280();
  Wire.begin();
  initMPU6050();
  Serial.begin(9600);
}

int my_putc( char c, FILE *t) {
  
  Serial.write( c );

}

void loop() {
  
  // @TODO: test actual temp 
  getTmpLM35();
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(WHO_AM_I);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,1);
  
  WHO_AM_I_data = Wire.read();
  Serial.print("Who am I? - ");
  Serial.println(WHO_AM_I_data);
  getAcc();
  getGyro();
  getBaro();
  // @TODO: test this method * writen before 
  getSerial(); 
  delay(1000);
  
}

void initMPU6050() {
  
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

void initXBee() {
   
   XBee.begin(9600);
   
}

void initB280() {
   
  Wire.begin();
  Serial.begin(9600);
  if (!bme.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  delay(2000);
  
}

void initTMPLM35() {

  pinMode(TEMPLM35_SENSOR,INPUT);
  
}

void getAcc() {
  
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
  Serial.print("AcX - ");
  Serial.print(acc[0]);
  Serial.print(" | AcY - ");
  Serial.print(acc[1]);
  Serial.print(" | AcZ - ");
  Serial.println(acc[2]);
  return;
  
}
float readTemp() {
  
  int Temp;
  float tempC;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_TEMP_1);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,2);
  Temp= Wire.read() <<8 | Wire.read();
//  Serial.print(Temp);Serial.print(" ");
  tempC = Temp/340. + 36.53;
  return tempC;
  
}

void getGyro() {
  
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
   Serial.print("GyX - ");
  Serial.print(gyro[0]);
  Serial.print(" | GyY - ");
  Serial.print(gyro[1]);
  Serial.print(" | GyZ - ");
  Serial.println(gyro[2]);
  return;

}

void getBaro() {
/*** Get and print temperature ***/
    Serial.print("Temperature:");
    Serial.println( bme.readTemperature());
       
    Serial.print(F("Pressure:"));
    Serial.print(bme.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Altitude:"));
    Serial.print(bme.readAltitude(REF_PRESSURE)); // this should be adjusted to your local forcase
    Serial.println(" m");
    

}


void getTmpLM35() {
  
  vout=analogRead(TEMPLM35_SENSOR);
  vout=(vout*500)/1023;
  tempc=vout; // Storing value in Degree Celsius

  Serial.print("in DegreeC=");
  Serial.print("\t");
  Serial.print(tempc);
  Serial.println();  

}

void getSerial() {
  if (Serial.available())
  { // If data comes in from serial monitor, send it out to XBee
    XBee.write(Serial.read());
  }
  if (XBee.available())
  { // If data comes in from XBee, send it out to serial monitor
    Serial.write(XBee.read());
  }
}

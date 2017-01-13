#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

/* MPU CONSTANTS */
const int MPU_ADDR=0x68;
const byte PWR_MGMT_1=0x68;
const byte WHO_AM_I=0x75;
const byte MPU_ACX_1=0x3B;
const byte MPU_ACY_1=0x3D;
const byte MPU_ACZ_1=0x3F;
const byte MPU_TEMP_1=0x41;
const byte MPU_GYX_1=0x44;

const float ACC_SCALE_FACT=4096.;
const float GYR_SCALE_FACT=131.;
const float REF_PRESSURE=1013.25;

byte WHO_AM_I_data;
float acc[3];
float gyro[3];
float baro[3];

Adafruit_BMP280 bme; // I2C

void setup() {
  // 280 Sensor 
  Wire.begin();
  Serial.begin(9600);
  if (!bme.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  
  delay(2000);
  Wire.begin();
  initMPU6050();
  Serial.begin(9600);
  

}
int my_putc( char c, FILE *t) {
  Serial.write( c );
}
void loop() {
  getMetrology();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(WHO_AM_I);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,1);
  WHO_AM_I_data = Wire.read();
  Serial.print("Who am I? - ");
  Serial.println(WHO_AM_I_data);
  getAcc();
  Serial.print("AcX - ");
  Serial.print(acc[0]);
  Serial.print(" | AcY - ");
  Serial.print(acc[1]);
  Serial.print(" | AcZ - ");
  Serial.println(acc[2]);
  getGyro();
  Serial.print("GyX - ");
  Serial.print(gyro[0]);
  Serial.print(" | GyY - ");
  Serial.print(gyro[1]);
  Serial.print(" | GyZ - ");
  Serial.println(gyro[2]);
  getBaro();
  printf("T = %.2f, p = %.0f Pa, h = %.2f\n", baro[0], baro[1], baro[2]);
/*** Get and print temperature ***/
  Serial.print("Temp? - ");
  Serial.println(readTemp());
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
  return;

}

void getBaro() {
    
    baro[0] = bme.readTemperature();
    baro[1] = bme.readPressure();
    baro[2] = bme.readAltitude(REF_PRESSURE);

}



void getMetrology(){
    
    //@TODO: make more precise readings and average  
    Serial.print(F("Temperature = "));
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    Serial.print(F("Pressure = "));
    Serial.print(bme.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");
    
    Serial.println();
    delay(2000);

}



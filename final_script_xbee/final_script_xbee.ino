// @ TO DO : find when to release the servos
// @ TO DO : get calibration
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>
#include <Servo.h>

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
float baro[3];

/* tempLM35 SENSOR */
const int TEMPLM35_SENSOR=A7;

/* tempLM35 VARIABLES */
float tempc;  
float tempf;  
float vout; 

/* Bme SENSOR */
Adafruit_BMP280 bme; 
float altitude;

/* XBee SENSOR */
SoftwareSerial XBee(2, 3); 

/* counter & timer */
int counter = 0; 
unsigned long start_time = 0;
unsigned long current_time;
unsigned long elapsed_time;
boolean servo_starting_position = false; 

/* Servos */
Servo myservo;

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
  XBee.print(counter);
  XBee.print(";");
  XBee.print(millis());
  XBee.print(";");
  getTmpLM35();
  readTemp();
  getBaro();
  Serial.println("Altitude");
  Serial.println(baro[2]);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(WHO_AM_I);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,1);
  getAcc();
  getGyro();
  XBee.print(millis());
  XBee.println();
  start_timer();
  current_time = millis();
  elapsed_time = current_time - start_time;
  Serial.println("Elapsed Time");
  Serial.println(elapsed_time);
  Serial.println("*****");
  Serial.println("Counter");
  Serial.println(counter);
  Serial.println("Altitude");
  Serial.println(baro[2]);
  if (servo_starting_position == true && counter == 100) {
      Serial.println("Send signal");
      initServo();
      delay(1000);
      myservo.write(180);
      delay(1000);
      myservo.detach();
    }

}

void initXBee() {
   // Configures XBEE
   XBee.begin(9600);
}
void initServo() {
  // Configures Servos
  myservo.attach(9, 1000, 2000);
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
  XBee.begin(9600);
  if (!bme.begin()) {  
    XBee.println(("Could not find a valid BMP280 sensor, check wiring!"));
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
  
  XBee.print(acc[0]);
  XBee.print(";"); 
  
  int isReleased = 0;
  XBee.print(acc[1]);
  XBee.print(";");
  XBee.print(acc[2]);
  XBee.print(";");
  
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
  XBee.print(tempC); 
  XBee.print(";");
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
  XBee.print(gyro[0]);
  XBee.print(";");
  XBee.print(gyro[1]);
  XBee.print(";");
  XBee.print(gyro[2]);
  XBee.print(";");
  return;
}

float getBaro() {
    /*** Get Baro readings ***/
  XBee.print(bme.readTemperature());
  baro[0] = bme.readTemperature();
  XBee.print(";");
  XBee.print(bme.readPressure());
  XBee.print(";");
  XBee.print(bme.readAltitude(REF_PRESSURE));
  baro[2] = bme.readAltitude(REF_PRESSURE);
  XBee.print(";");// this should be adjusted to your local forcase
  
}

void getTmpLM35() {
  /*** Get and print temperature ***/
  vout=analogRead(TEMPLM35_SENSOR);
  // Change to how is calibrated
  vout=(vout*500)/1023;
  tempc=vout; // Storing value in Degree Celsius
  XBee.print(tempc);
  XBee.print(";");
}

void start_timer(){
   if (XBee.available() && Serial.write(XBee.read())){
    Serial.println(XBee.read());
    start_time = millis();
//    counter = 0;
    Serial.println("Should open");
    delay(1000);
    initServo();
    myservo.write(0);
    delay(1000);
    myservo.detach();
    servo_starting_position = true;
   }
}


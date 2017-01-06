#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
 
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void setup() {
  
  Serial.begin(9600);
  if (!bme.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  delay(2000);

}

void loop() {
  
  getMetrology();

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



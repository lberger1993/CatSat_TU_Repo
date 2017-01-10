const int sensor=A1; // Assigning analog pin A1 to variable 'sensor'

float tempc;  
float tempf;  
float vout; 

void setup()
{

pinMode(sensor,INPUT); // Configuring pin A1 as input
Serial.begin(9600);

}
void loop() 
{

vout=analogRead(sensor);
vout=(vout*500)/1023;
tempc=vout; // Storing value in Degree Celsius

Serial.print("in DegreeC=");
Serial.print("\t");
Serial.print(tempc);
Serial.println();
delay(1000); //Delay of 1 second for ease of viewing 

}

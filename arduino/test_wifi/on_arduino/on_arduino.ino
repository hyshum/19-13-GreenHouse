#include <dht.h>
dht DHT;
#define DHT11_PIN 4 //Nodemcu: D1, Arduino: 4
#define LIGHT_PIN 3  //Nodemcu: D2, Arduino: 0
#define OUTSIDE_TEMP_PIN 5  //Nodemcu: D5, Arduino: 1
//#define Soil_moisture_PIN A0  //Nodemcu: D7, Arduino: 1
int in1 = 6;  //Nodemcu: D6, Arduino: 7


int out_reading;
float voltage;
float tempOutF;
int chk;
double temp;
double humidity;
int on;
int photoCellReading;

unsigned long StartTime;
unsigned long FinishTime;
unsigned long RunningTime;
unsigned long TotalRunningTime = 0;
int heaterindex = 0;
int Warning_temperture_low = 0;

int sensorValue = 0;  
int percent = 0;
String percentString ="0";
int stringLength = 0;
double threshold_temperature = 85;

String toSend = "";

void setup() {
  

  // Start serial
  Serial.begin(115200);

}

void loop() {

  toSend = "";
  out_reading = analogRead( OUTSIDE_TEMP_PIN );
  voltage = out_reading * 3.3;
  voltage /= 1024.0;

  tempOutF = ( voltage - 0.5 ) * 100 * 9.0/5.0 + 25;
  chk = DHT.read11( DHT11_PIN );
  temp = DHT.temperature * 1.8 + 32;
  humidity = DHT.humidity;
  photoCellReading = analogRead( LIGHT_PIN );

  toSend += temp;
  toSend += ";";
  toSend += tempOutF;
  toSend += ";";
  toSend += photoCellReading;
  toSend += ";";

  digitalWrite( in1, HIGH );
  if ( temp > threshold_temperature )
  {
    //if temperature is above threshold temperature, turn relay off and turn LED off
    digitalWrite(in1, HIGH );

    Warning_temperture_low = 0;
     
    if (heaterindex == 1) {
     RunningTime = FinishTime - StartTime;
     TotalRunningTime += RunningTime;
     heaterindex = 0;
    }
    StartTime = millis();
    
  }
  else if ( temp <= threshold_temperature )
  {
    //if temperature is below threshold temperature, turn on relay and turn LED on
    

    if (temp + 20 < threshold_temperature)
        Warning_temperture_low = 1;
    else  
        Warning_temperture_low = 0;
    
    heaterindex = 1;
    FinishTime = millis();
    digitalWrite(in1,LOW);
    
  }
  Serial.println( toSend );
  delay( 5000 );
  digitalWrite( in1, LOW );
}

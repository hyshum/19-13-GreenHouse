#include <dht.h>
dht DHT;
#define DHT11_PIN 4 //Nodemcu: D1, Arduino: 4
#define LIGHT_PIN 3  //Nodemcu: D2, Arduino: 0
#define OUTSIDE_TEMP_PIN 5  //Nodemcu: D5, Arduino: 1
#define RW_PIN 10
#define LED_PIN 7
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
bool read_val = 0;
String toSend = "";

void setup() {

  pinMode( RW_PIN, INPUT );
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, LOW );
  // Start serial
  Serial.begin(115200);

}
String newTemp = "";
void loop() {

  read_val = digitalRead( RW_PIN );

  if( read_val == 1 )
  {
      //digitalWrite( LED_PIN, HIGH ); 
      delay( 100 );
      if( Serial.available() > 0 )
      {
       newTemp = Serial.readString();
       Serial.println( newTemp );
       if ( newTemp == "hello" )
       {
        Serial.println("yay");
        digitalWrite( LED_PIN, HIGH ); 
        delay( 5000 );
       }
      }
  }
  
  else
  {
    digitalWrite( LED_PIN, LOW );

    Serial.print( "arduino" );
    delay( 1000 );
    /*
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
    digitalWrite( in1, LOW );*/
  }
}

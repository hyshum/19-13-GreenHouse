#include <dht.h>
dht DHTIN;
dht DHTOUT;
#define DHT11_PIN 4 //Nodemcu: D1, Arduino: 4
#define LIGHT_PIN 3  //Nodemcu: D2, Arduino: 0
#define DHT11_OUT 8  //Nodemcu: D5, Arduino: 1
#define READ_PIN 10
#define WRITE_PIN 11
#define LED_PIN 7
#define RELAY_PIN 6
#define SOIL_PIN1 5
//#define Soil_moisture_PIN A0  //Nodemcu: D7, Arduino: 1


double tempOutF;
int chk;
int chkOut;
double temp;
double humidity;
int on;
int photoCellReading;
int soilMoisture;

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
unsigned long begin_time = 0;
unsigned long end_time = 0;

void setup() {

  pinMode( READ_PIN, INPUT );
  pinMode( WRITE_PIN, OUTPUT );
  digitalWrite( WRITE_PIN, LOW );
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, LOW );
  // Start serial
  Serial.begin(115200);

}
String newTemp = "";
void parseString( String input )
{
  String tmp = "";
  int len = input.length();
  for( int i = 0; i <  len; i++ )
  {
     if( isDigit( input.charAt( i ) ) || input.charAt( i ) == '.' )
     {
      tmp += input.charAt( i );
     }
  }
  threshold_temperature = atof( tmp.c_str() );
}
void loop() {

  read_val = digitalRead( READ_PIN );
  end_time = millis();
  if( read_val == 1 )
  {
      digitalWrite( LED_PIN, HIGH );
       digitalWrite( WRITE_PIN, LOW );
      //digitalWrite( LED_PIN, HIGH ); 
      delay( 100 );
      while( true )
      {
        if( Serial.available() > 0 )
        {
         newTemp = Serial.readString();
         if( newTemp[0] == 'T' && newTemp[1] == 'M' && newTemp[2] == 'P' )
         {
          digitalWrite( LED_PIN, HIGH ); 
          parseString( newTemp );
          break;
         }
        }
      }
  }
  
  else 
  {
    digitalWrite( LED_PIN, LOW );
    delay( 5000 );
    toSend = "";
  
    chkOut = DHTOUT.read11( DHT11_OUT );
    tempOutF = DHTOUT.temperature * 1.8 + 32;
    chk = DHTIN.read11( DHT11_PIN );
    temp = DHTIN.temperature * 1.8 + 32;
    humidity = DHTIN.humidity;
    photoCellReading = analogRead( LIGHT_PIN );
    soilMoisture = analogRead( SOIL_PIN1 );

    if ( temp > threshold_temperature )
    {
      //if temperature is above threshold temperature, turn relay off and turn LED off
      digitalWrite(RELAY_PIN, HIGH );
  
      Warning_temperture_low = 0;
       
      if (heaterindex == 1) 
      {
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
      digitalWrite(RELAY_PIN,LOW);   
    }

    bool check = digitalRead( READ_PIN );
    if( end_time - begin_time > 10000 && !check )
    {
      begin_time = end_time;
      digitalWrite( LED_PIN, LOW );
      digitalWrite( WRITE_PIN, HIGH );
      delay( 5000 );
      toSend += "DTA";
      toSend += temp;
      toSend += ";";
      toSend += tempOutF;
      toSend += ";";
      toSend += photoCellReading;
      toSend += ";";
      toSend += humidity;
      toSend += ";";
      toSend += soilMoisture;
      toSend += ";";
  
      Serial.print( toSend );
      //Serial.print( toSend );
      digitalWrite( WRITE_PIN, LOW );
      delay( 5000 );
    }
  }
}

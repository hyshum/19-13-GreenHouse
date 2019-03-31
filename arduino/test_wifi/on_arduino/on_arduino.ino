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

unsigned long startTimeout;
unsigned long endTimeout;
unsigned long timeout = 30000;


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

  //read pin indicates nodeMCU -> arduino com
  pinMode( READ_PIN, INPUT );

  //write pin indicates arduino -> nodeMCU com
  pinMode( WRITE_PIN, OUTPUT );
  digitalWrite( WRITE_PIN, LOW );

  //LED indicates that nodeMCU is writing to arduino
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, LOW );
  // Start serial
  Serial.begin(115200);

}


String newTemp = "";
void parseString( String input )
{
  //parse temperature string information from nodeMCU and set to threshold_temp
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

  //turn on LED to indicate reading from nodeMCU
  digitalWrite( LED_PIN, HIGH );
  digitalWrite( WRITE_PIN, LOW );
  //digitalWrite( LED_PIN, HIGH ); 
  delay( 100 );
  digitalWrite( LED_PIN, LOW );
  toSend = "";

  //read outside temperature sensor
  chkOut = DHTOUT.read11( DHT11_OUT );
  tempOutF = DHTOUT.temperature * 1.8 + 32;

  //read inside temperature sensor
  chk = DHTIN.read11( DHT11_PIN );
  temp = DHTIN.temperature * 1.8 + 32;

  //read inside humidity
  humidity = DHTIN.humidity;

  //read photocell reading
  photoCellReading = analogRead( LIGHT_PIN );

  //read soil moisture reading
  soilMoisture = analogRead( SOIL_PIN1 );
  //check READ_PIN again to ensure no bus fight

    //
    
    digitalWrite( LED_PIN, LOW );
    //turn WRITE_PIN on to indicate writing to nodeMCU
    digitalWrite( WRITE_PIN, HIGH );

    //delay so nodeMCU has time to read correct pin value
    
    //prepend "DTA" to indicate successful transmission and not garbage
    //use semicolon as token to parse string
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

    //send over serial connection
    Serial.print( toSend );

    //turn off write pin
    digitalWrite( WRITE_PIN, LOW );

    //delay
    delay( 5000 );
    
}

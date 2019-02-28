

// Libraries
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WifiClientSecure.h>
#include <MQTT.h>
#include <time.h>
#include "secrets.h"
// WiFi network

uint8_t DST = 0;
const int MQTT_PORT = 8883;
const char MQTT_SUB_TOPIC[] = "$aws/things/greenhouse/shadow/update";
const char MQTT_PUB_TOPIC[] = "$aws/things/greenhouse/shadow/update";

int threshold_temperature = 80;

//int soil_moisture = 10;


#include <dht.h>
dht DHT;
#define DHT11_PIN D1 //Nodemcu: D1, Arduino: 4
#define LIGHT_PIN D3  //Nodemcu: D2, Arduino: 0
#define OUTSIDE_TEMP_PIN A0  //Nodemcu: D5, Arduino: 1
#define WRITE_PIN D0
#define READ_PIN D5
#define LED_PIN D1
#define WIFI_ON D2
//#define Soil_moisture_PIN A0  //Nodemcu: D7, Arduino: 1
int in1 = D6;  //Nodemcu: D6, Arduino: 7

double tempOutF;
double temp;
double humidity;
int soilMoisture;
int on;
int photoCellReading;
bool changeTemp = false;
int soilCorrected;
unsigned long StartTime;
unsigned long FinishTime;
unsigned long RunningTime;
unsigned long TotalRunningTime = 0;
int heaterindex = 0;
char htmlResponse[3000];
int Warning_temperture_low = 0;



int sensorValue = 0;  
int percent = 0;
String percentString ="0";
int stringLength = 0;

WiFiClientSecure net;

BearSSL::X509List cert( cacert );
BearSSL::X509List client_crt( client_cert );
BearSSL::PrivateKey key( privkey );

MQTTClient client;
unsigned long lastMillis = 0;
time_t now;
time_t nowish = 1510592825;

void NTPConnect(void)
{
    configTime( TIME_ZONE * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov" );
    now = time( nullptr );
    while( now < nowish )
    {
      delay( 500 );
      //Serial.print(".");
      now = time( nullptr );
    }
    //Serial.println( "done!" );
    struct tm timeinfo;
    gmtime_r( &now, &timeinfo );
}

void messageReceived(String &topic, String &payload)
{
  //Serial.println("Recieved [" + topic + "]: " + payload);
}



void lwMQTTErr(lwmqtt_err_t reason)
{
  
  if (reason == lwmqtt_err_t::LWMQTT_SUCCESS)
    Serial.print("Success");
  else if (reason == lwmqtt_err_t::LWMQTT_BUFFER_TOO_SHORT)
    Serial.print("Buffer too short");
  else if (reason == lwmqtt_err_t::LWMQTT_VARNUM_OVERFLOW)
    Serial.print("Varnum overflow");
  else if (reason == lwmqtt_err_t::LWMQTT_NETWORK_FAILED_CONNECT)
    Serial.print("Network failed connect");
  else if (reason == lwmqtt_err_t::LWMQTT_NETWORK_TIMEOUT)
    Serial.print("Network timeout");
  else if (reason == lwmqtt_err_t::LWMQTT_NETWORK_FAILED_READ)
    Serial.print("Network failed read");
  else if (reason == lwmqtt_err_t::LWMQTT_NETWORK_FAILED_WRITE)
    Serial.print("Network failed write");
  else if (reason == lwmqtt_err_t::LWMQTT_REMAINING_LENGTH_OVERFLOW)
    Serial.print("Remaining length overflow");
  else if (reason == lwmqtt_err_t::LWMQTT_REMAINING_LENGTH_MISMATCH)
    Serial.print("Remaining length mismatch");
  else if (reason == lwmqtt_err_t::LWMQTT_MISSING_OR_WRONG_PACKET)
    Serial.print("Missing or wrong packet");
  else if (reason == lwmqtt_err_t::LWMQTT_CONNECTION_DENIED)
    Serial.print("Connection denied");
  else if (reason == lwmqtt_err_t::LWMQTT_FAILED_SUBSCRIPTION)
    Serial.print("Failed subscription");
  else if (reason == lwmqtt_err_t::LWMQTT_SUBACK_ARRAY_OVERFLOW)
    Serial.print("Suback array overflow");
  else if (reason == lwmqtt_err_t::LWMQTT_PONG_TIMEOUT)
    Serial.print("Pong timeout");
}

void lwMQTTErrConnection(lwmqtt_return_code_t reason)
{
  
  if (reason == lwmqtt_return_code_t::LWMQTT_CONNECTION_ACCEPTED)
    Serial.print("Connection Accepted");
  else if (reason == lwmqtt_return_code_t::LWMQTT_UNACCEPTABLE_PROTOCOL)
    Serial.print("Unacceptable Protocol");
  else if (reason == lwmqtt_return_code_t::LWMQTT_IDENTIFIER_REJECTED)
    Serial.print("Identifier Rejected");
  else if (reason == lwmqtt_return_code_t::LWMQTT_SERVER_UNAVAILABLE)
    Serial.print("Server Unavailable");
  else if (reason == lwmqtt_return_code_t::LWMQTT_BAD_USERNAME_OR_PASSWORD)
    Serial.print("Bad UserName/Password");
  else if (reason == lwmqtt_return_code_t::LWMQTT_NOT_AUTHORIZED)
    Serial.print("Not Authorized");
  else if (reason == lwmqtt_return_code_t::LWMQTT_UNKNOWN_RETURN_CODE)
    Serial.print("Unknown Return Code");
    
}

void connectToMqtt(bool nonBlocking = false)
{
  Serial.print("MQTT connecting ");
  while (!client.connected())
  {
    if (client.connect(THINGNAME))
    {
      Serial.println("connected!");
      if (!client.subscribe(MQTT_SUB_TOPIC))
        lwMQTTErr(client.lastError());
    }
    else
    {
      Serial.print("failed, reason -> ");
      lwMQTTErrConnection(client.returnCode());
      if (!nonBlocking)
      {
        Serial.println(" < try again in 5 seconds");
        delay(5000);
      }
      else
      {
        Serial.println(" <");
      }
    }
    if (nonBlocking)
      break;
  }
}

void connectToWiFi(String init_str)
{
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
}

void checkWiFiThenMQTT(void)
{
  connectToWiFi("Checking WiFi");
  connectToMqtt();
}

unsigned long previousMillis = 0;
const long interval = 5000;

void checkWiFiThenMQTTNonBlocking(void)
{
  connectToWiFi(emptyString);
  if (millis() - previousMillis >= interval && !client.connected()) {
    previousMillis = millis();
    connectToMqtt(true);
  }
}

void checkWiFiThenReboot(void)
{
  connectToWiFi("Checking WiFi");
  ESP.restart();
}


void sendData(void)
{
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  JsonObject state = root.createNestedObject("state");
  JsonObject state_reported = state.createNestedObject("reported");
  state_reported["tmpi"] = temp;
  state_reported["tmpo"] = tempOutF;
  state_reported["mdy"] = humidity;
  state_reported["lt"] = photoCellReading;
  state_reported["sm1"] = soilCorrected;  
  char shadow[measureJson(root) + 1];
  serializeJson(root, shadow, sizeof(shadow));
  if (!client.publish(MQTT_PUB_TOPIC, shadow, false, 0))
    lwMQTTErr(client.lastError());
}


String incomingString;


void setup() {

  
  pinMode(WRITE_PIN, OUTPUT);
  digitalWrite(WRITE_PIN, LOW);
  pinMode(READ_PIN, INPUT);
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, LOW );
  // Start serial
  Serial.begin(115200);
  Serial.println( "begin" );
  // Connecting to a WiFi network
  WiFi.hostname( THINGNAME );
  WiFi.mode( WIFI_STA );
  WiFi.begin( ssid, password );
  connectToWiFi(String( "connecting..." ) );
  NTPConnect();
  Serial.println( "wifi" );
  net.setTrustAnchors( &cert );
  net.setClientRSACert( &client_crt, &key );
  client.begin( MQTT_HOST, MQTT_PORT, net );
  client.onMessage( messageReceived );

  connectToMqtt();
  
  delay( 500 );
  }

String sensorArray[6];

void parseString( String input )
{
 int len =  input.length();
 String current = "";
 int pos = 0;
 for( int i = 0; i < len; i++ )
 {
  if( isDigit( input.charAt(i) ) || input.charAt(i) == '.' )
  {
    current += input.charAt( i );
  }
  else if( input.charAt(i) == ';' )
  {
    sensorArray[pos] = current;
    current = "";
    pos = pos + 1; 
  }
 }
 temp = atof( sensorArray[0].c_str() );
 tempOutF = atof( sensorArray[1].c_str() );
 photoCellReading = atof( sensorArray[2].c_str() );
 humidity = atof( sensorArray[3].c_str() ); 
 soilMoisture = atof( sensorArray[4].c_str() );
 soilCorrected = -.51*soilMoisture + 235; 
 if( soilCorrected > 100 )
 {
   soilCorrected = 100;
 }
 else if( soilCorrected < 0 )
 {
  soilCorrected = 0;
 }
}

bool read_val = false;
unsigned long start_time = 0;
unsigned long end_time = 0;
void loop() {

    
    read_val = digitalRead( READ_PIN );
    end_time = millis();
    digitalWrite( WIFI_ON, HIGH );

    if( !client.connected() )
    {
      checkWiFiThenMQTT();
    }
    else
    {
      client.loop();
    
      if( end_time - start_time > 1000000 )
      {
          start_time = end_time;
          changeTemp = 1;
      }
      
      if( changeTemp == 1 && !read_val )
      {
        digitalWrite( WRITE_PIN, HIGH );
        delay( 5000 );
        String tmpString = "TMP";
        tmpString += threshold_temperature;
        Serial.print( tmpString );
        digitalWrite( LED_PIN, LOW );
        changeTemp = false;
        digitalWrite( WRITE_PIN, LOW );
        delay( 5000 );
      }
      else if( read_val == 1 )
      {
        //digitalWrite( WIFI_ON, HIGH );
        delay( 100 );
        digitalWrite( LED_PIN, HIGH );
  
        while( true )
        {
          if (Serial.available() > 0) 
          {
          
            // read the oldest byte in the serial buffer:
            incomingString = Serial.readString();
            //check for prepending
            if( incomingString[0] == 'D' && incomingString[1] == 'T' && incomingString[2] == 'A' )
            {
              parseString( incomingString );
              sendData();
              break;
            }
          delay( 5000 );
          }
         
        }
      }
      else
      { 
        digitalWrite( LED_PIN, LOW );
      }
    }
}

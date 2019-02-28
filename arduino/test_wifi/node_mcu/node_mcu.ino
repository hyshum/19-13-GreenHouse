

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

float tempOutF;
double temp;
double humidity;
int on;
int photoCellReading;
bool changeTemp = false;

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
/*void read_sensors() {
   out_reading = analogRead( OUTSIDE_TEMP_PIN );
   voltage = out_reading * 3.3;
    voltage /= 1024.0;

   tempOutF = (voltage - 0.5) * 100 * 9.0/5.0 + 29;
   chk = DHT.read11(DHT11_PIN);
   temp = DHT.temperature * 1.8 + 32;
   humidity = DHT.humidity;
   photoCellReading = analogRead( LIGHT_PIN );
  //Serial.print("light reading: " );
  //Serial.println( photoCellReading );
  //Serial.print("Temp = " );
  //Serial.println( temp );
  //Serial.print( "Temp outside = " );
  //Serial.println( tempOutF );
  //Serial.print("Humidity = ");
  //Serial.println(DHT.humidity);
  
  if ( temp > threshold_temperature )
  {
    //if temperature is above threshold temperature, turn relay off and turn LED off
    digitalWrite(in1, HIGH );

    Warning_temperture_low = 0;
     
    if (heaterindex == 1) 
    {
      RunningTime = FinishTime - StartTime;
      TotalRunningTime += RunningTime;
      heaterindex = 0;
    }
    StartTime = millis();
    
  }
  else if ( temp <= threshold_temperature)
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
 // Serial.println(TotalRunningTime);
  
}
*/

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
    //Serial.print("Setting time using SNTP");
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
    //Serial.print( "Current time: " );
    //Serial.print( asctime(&timeinfo ) );
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
  state_reported["value"] = random(100);
  state_reported["tmpi"] = temp;
  state_reported["tmpo"] = tempOutF;
  state_reported["mdy"] = humidity;
  state_reported["sm1"] = random(100);  
  //serializeJson(root, Serial);
  char shadow[measureJson(root) + 1];
  serializeJson(root, shadow, sizeof(shadow));
  if (!client.publish(MQTT_PUB_TOPIC, shadow, false, 0))
    lwMQTTErr(client.lastError());
}


String incomingString;


int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 220, 0, 100);
  if(percentValue > 100)
    percentValue = 100;
  return percentValue;
}
void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html lang=\"en\">";
  html += "<head> <meta charset=\"utf-8\"> <meta http-equiv=\"refresh\" content=\"10\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> </head>";
  html += "<body>";
  html += "<h1>Sensor Data</h1>";
  if (Warning_temperture_low) {
    html += "<p>Warning: temperature is too low</p>";
  }
  if (humidity>90) {
    html += "<p>Warning: humidity is too high</p>";
  }
  if (humidity<30) {
    html += "<p>Warning: humidity is too low</p>";
  }
  /*if( percent <10)
  {
    html += "<p>Warning: soil moisture is too low</p>";
  }*/
  html += "<p>Temperature: </p>";
  html += temp;
  html += "<br><br>";
  html += "<p>Humidity: </p>";
  html += humidity;
  html += "<br><br>";
  html += "<p>Temperature outside</p>";
  html += tempOutF;
  //html += "<p>Soil Mositure: </p>";
  //html += percentString;
  html += "<br><br>";
  html += "<p>Light: </p>";
  html += String(photoCellReading);
  html += "<br><br>";
  html += "<h1>Input the threshold temperature</h1>";
  html += "<input type='text' name='date_hh' id='date_hh' size=2 autofocus> Degree";
  html += "<div> <br><button id=\"save_button\">Submit</button></div>";
  html += "<br>";
  html += "<p>Heater running time(s): </p>";
  html += TotalRunningTime / 1000;
  html += "<br><br>";
  //html += "<div> <br><button onclick="document.getElementById('date_hh').value = ''" id=\"save_button\">Submit</button></div>";
  html += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>"; 
  html += "<script>";
  html += "var hh;";
  html += "var mm;";
  html += "var ss;";
  html += "$('#save_button').click(function(e){";
  html += "e.preventDefault();";
  html += "hh = $('#date_hh').val();";
  html += "mm = $('#date_mm').val();";
  html += "ss = $('#date_ss').val();";        
  html += "$.get('/save?hh=' + hh + '&mm=' + mm + '&ss=' + ss, function(data){";
  html += "console.log(data);";
  html += "});";
  html += "})";     
  html += "</script>";
  html += "</body>";
  html += "</html>";


  //server.send ( 200, "text/html", html );  

}

/*
void handleSave() {
  if (server.arg("hh")!= ""){
    threshold_temperature = server.arg("hh").toInt();
    changeTemp = true;
  }
}
*/

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

String sensorArray[5];

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

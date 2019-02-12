

// Libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// WiFi network
const char* ssid     = "HotspotSM8";
const char* password = "ygia3939";

ESP8266WebServer server ( 80 );
int threshold_temperature = 80;

//int soil_moisture = 10;


#include <dht.h>
dht DHT;
#define DHT11_PIN D1 //Nodemcu: D1, Arduino: 4
#define LIGHT_PIN D3  //Nodemcu: D2, Arduino: 0
#define OUTSIDE_TEMP_PIN A0  //Nodemcu: D5, Arduino: 1
#define RW_PIN D0
#define LED_PIN D1
//#define Soil_moisture_PIN A0  //Nodemcu: D7, Arduino: 1
int in1 = D6;  //Nodemcu: D6, Arduino: 7


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
char htmlResponse[3000];
int Warning_temperture_low = 0;


String apiKey = "A7OVYSO7FWW8XYSC"; //  Enter your Write API key from ThingSpeak
const char *thingspeakserver = "api.thingspeak.com";
WiFiClient client;

int sensorValue = 0;  
int percent = 0;
String percentString ="0";
int stringLength = 0;

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


   server.send ( 200, "text/html", html );  

}


void handleSave() {
  if (server.arg("hh")!= ""){
    threshold_temperature = server.arg("hh").toInt();
  }
}


void setup() {

  pinMode(RW_PIN, OUTPUT);
  digitalWrite(RW_PIN, LOW);
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, LOW );
  // Start serial
  Serial.begin(115200);
  //delay(10);

  // Connecting to a WiFi network
  /*Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on ( "/", handleRoot );
  server.on ("/save", handleSave);

  server.begin();
  Serial.println ( "HTTP server started" );*/


}
int incomingByte;      // a variable to read incoming serial data into
String incomingString;
bool changeTemp = 0;
void loop() {


    if( changeTemp == 1 )
    {
      digitalWrite( RW_PIN, HIGH );
      delay( 5000 );
      Serial.print( "hello" );
      digitalWrite( LED_PIN, LOW );
      changeTemp = 0;
      delay( 5000 );
    }
    else
    {
      digitalWrite( RW_PIN, LOW );

      delay( 5000 );
      while( true )
      {
        if (Serial.available() > 0) {
        
          // read the oldest byte in the serial buffer:
          incomingString = Serial.readString();
          if( incomingString == "arduino" )
          {
            Serial.println( "woohoo!" );
            digitalWrite( LED_PIN, HIGH );
            changeTemp = 1; 
            break;
          }
  
        }
      }
    }
      
      //delay( 5000 );
      /*
      changeTemp = 1;
    }
    //changeTemp = ~changeTemp;
   /*
  server.handleClient();
  Serial.print("The current threshold temperature is ");
  Serial.println(threshold_temperature);
  Serial.println("");
  read_print_temperature();
  
      if (client.connect(thingspeakserver, 80)) //   "184.106.153.149" or api.thingspeak.com
    {

        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(temp);   //Temperature
        postStr += "&field2=";
        postStr += String(humidity);   //Humidity
        postStr += "&field3=";
        postStr += String(tempOutF);   //Humidity
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
    }
    client.stop();

    //sensorValue = analogRead(Soil_moisture_PIN);
    //percent = convertToPercent(sensorValue);
    //percentString = String(percent);
    //Serial.println( "sensor val" + sensorValue );
    //Serial.println( "percent: " + percentString );

    delay(5000);
}

void read_print_temperature() {
   out_reading = analogRead( OUTSIDE_TEMP_PIN );
   voltage = out_reading * 3.3;
  voltage /= 1024.0;

   tempOutF = (voltage - 0.5) * 100 * 9.0/5.0 + 29;
   chk = DHT.read11(DHT11_PIN);
   temp = DHT.temperature * 1.8 + 32;
   humidity = DHT.humidity;
   on = 0;
   photoCellReading = analogRead( LIGHT_PIN );
  //Serial.print("light reading: " );
  //Serial.println( photoCellReading );
  Serial.print("Temp = " );
  Serial.println( temp );
  Serial.print( "Temp outside = " );
  Serial.println( tempOutF );
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  
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
  Serial.println(TotalRunningTime);
  */
}



int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 220, 0, 100);
  if(percentValue > 100)
    percentValue = 100;
  return percentValue;
}



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


String apiKey = "A7OVYSO7FWW8XYSC"; //  Enter your Write API key from ThingSpeak
const char *thingspeakserver = "api.thingspeak.com";
WiFiClient client_1;

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


  server.send ( 200, "text/html", html );  

}


void handleSave() {
  if (server.arg("hh")!= ""){
    threshold_temperature = server.arg("hh").toInt();
    changeTemp = true;
  }
}


void setup() {

  pinMode(WRITE_PIN, OUTPUT);
  digitalWrite(WRITE_PIN, LOW);
  pinMode(READ_PIN, INPUT);
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, LOW );
  // Start serial
  Serial.begin(115200);

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.on ( "/", handleRoot );
  server.on ("/save", handleSave);
  server.begin();

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
    /*
    if( end_time - start_time > 1000000 )
    {
        start_time = end_time;
        changeTemp = 1;
    }
    */
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
            digitalWrite( LED_PIN, HIGH );
            break;
          }
        delay( 5000 );
        }
       
      }
    }
    else
    { 
      digitalWrite( LED_PIN, LOW );
      server.handleClient();
  //Serial.print("The current threshold temperature is ");
  //Serial.println(threshold_temperature);
  //Serial.println("");
  
      if (client_1.connect(thingspeakserver, 80)) //   "184.106.153.149" or api.thingspeak.com
      {
  
          String postStr = apiKey;
          postStr += "&field1=";
          postStr += String(temp);   //Temperature
          postStr += "&field2=";
          postStr += String(humidity);   //Humidity
          postStr += "&field3=";
          postStr += String(tempOutF);   //Humidity
          postStr += "\r\n\r\n";
  
          client_1.print("POST /update HTTP/1.1\n");
          client_1.print("Host: api.thingspeak.com\n");
          client_1.print("Connection: close\n");
          client_1.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
          client_1.print("Content-Type: application/x-www-form-urlencoded\n");
          client_1.print("Content-Length: ");
          client_1.print(postStr.length());
          client_1.print("\n\n");
          client_1.print(postStr);
      }
      client_1.stop();

    //sensorValue = analogRead(Soil_moisture_PIN);
    //percent = convertToPercent(sensorValue);
    //percentString = String(percent);
    //Serial.println( "sensor val" + sensorValue );
    //Serial.println( "percent: " + percentString );

      delay(5000);
    }
}
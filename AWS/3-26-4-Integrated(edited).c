  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266WiFi.h>
  #include <ArduinoJson.h>
  #include <ESP8266AWSIoTMQTTWS.h>
  #include <dht.h>

  // WiFi network
  const char* ssid     = "Lyndon";
  const char* password = "987654321";

  char *region = (char *) "us-east-1";
  char *endpoint = (char *) "ap3wui1s0hiyp";
  char *mqttHost = (char *) "ap3wui1s0hiyp.iot.us-east-1.amazonaws.com";
  int mqttPort = 443;
  char *iamKeyId = (char *) "AKIAIRZPHHK6PN47GWGQ";
  char *iamSecretKey = (char *) "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";
  const char* aws_topic  = "$aws/things/Greenhouse_project/shadow/update";
  ESP8266DateTimeProvider dtp;
  AwsIotSigv4 sigv4(&dtp, region, endpoint, mqttHost, mqttPort,
  iamKeyId, iamSecretKey);
  AWSConnectionParams cp(sigv4);
  AWSWebSocketClientAdapter adapter(cp);
  AWSMqttClient client(adapter, cp);



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
  String incomingString;


  int convertToPercent(int value)
  {
    int percentValue = 0;
    percentValue = map(value, 1023, 220, 0, 100);
    if(percentValue > 100)
      percentValue = 100;
    return percentValue;
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
      int res = client.connect();
      Serial.printf("mqtt connect=%d\n", res);

      if (res == 0) {
        client.subscribe(aws_topic, 1,
          [](const char* topic, const char* msg)
          { Serial.printf("Got msg '%s' on topic %s\n", msg, topic); }
        );
      }  

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

      if( changeTemp == 1 && !read_val )
      {
        digitalWrite( WRITE_PIN, HIGH );
        delay( 5000 );
        String tmpString = "TMP";
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

        delay(5000);
      }

        if (client.isConnected()) {
      DynamicJsonDocument jsonBuffer;
      JsonObject root = jsonBuffer.to<JsonObject>();
      JsonObject state = root.createNestedObject("state");
      JsonObject state_reported = state.createNestedObject("reported");
      state_reported["time"] = random(100);
      state_reported["Temperature_inside"] = random(100);
      state_reported["Temperature_outside"] = random(100);
      state_reported["Humidity"] = random(100);
      state_reported["Soil Moisture"] = random(100);
      serializeJson(root, Serial);
      Serial.println();
      char shadow[measureJson(root) + 1];
      serializeJson(root, shadow, sizeof(shadow));

      client.publish(aws_topic, shadow, 0, false);
      client.yield();

    } else {
      Serial.println("Not connected...");
      delay(2000);
    }
    delay(10000);
  }
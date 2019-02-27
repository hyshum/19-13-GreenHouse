
//https://www.youtube.com/watch?v=AiCa6E_DBL8

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
//https://github.com/bblanchon/ArduinoJson (use v6.xx)
#include <ESP8266AWSIoTMQTTWS.h>
//https://github.com/debsahu/esp8266-arduino-aws-iot-ws
#include <dht.h>
dht DHT_inside_D1_GPIO5;
dht DHT_outside_D4_GPIO2;
#define DHT_inside_D1_GPIO5_Pin 5
#define DHT_outside_D4_GPIO2_Pin 2

#define sensorPin = 0
int Soil_inside_D5_GPIO14_enable = 14;      
int Soil_outside_D6_GPIO12_enable = 12;  

int Soil_inside = 0;  // variable to store the value coming from sensor A
int Soil_outside = 0;  // variable to store the value coming from sensor B

//https://github.com/Links2004/arduinoWebSockets

//https://projects.eclipse.org/projects/technology.paho/downloads
//(download Arduino version)

const char *ssid = "Lyndon";
const char *password = "987654321";

// See `src/aws_iot_config.h` for formatting
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
int count = 0;
void setup() {

    Serial.begin(115200);
    while(!Serial) {
        yield();
    }

pinMode(Soil_inside_D5_GPIO14_enable, OUTPUT);
  pinMode(Soil_outside_D6_GPIO12_enable, OUTPUT);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
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

void loop() {
  int chk_DHT_inside_D1_GPIO5 = DHT_inside_D1_GPIO5.read11(DHT_inside_D1_GPIO5_Pin);
  int tem_DHT_inside_D1_GPIO5 = DHT_inside_D1_GPIO5.temperature;
  int hum_DHT_inside_D1_GPIO5 = DHT_inside_D1_GPIO5.humidity;
  int chk_DHT_outside_D4_GPIO2 = DHT_outside_D4_GPIO2.read11(DHT_outside_D4_GPIO2_Pin);
  int tem_DHT_outside_D4_GPIO2 = DHT_outside_D4_GPIO2.temperature;
  int hum_DHT_outside_D4_GPIO2 = DHT_outside_D4_GPIO2.humidity;
  digitalWrite(Soil_inside_D5_GPIO14_enable, HIGH); 
  Soil_inside = analogRead(sensorPin);
  digitalWrite(Soil_inside_D5_GPIO14_enable, LOW);
  delay(100);
  digitalWrite(Soil_outside_D6_GPIO12_enable, HIGH); 
  Soil_outside = analogRead(sensorPin);
  digitalWrite(Soil_outside_D6_GPIO12_enable, LOW);
  if (client.isConnected()) {
    DynamicJsonDocument jsonBuffer;
    JsonObject root = jsonBuffer.to<JsonObject>();
    JsonObject state = root.createNestedObject("state");
    JsonObject state_reported = state.createNestedObject("reported");
    state_reported["time"] = 0;
    state_reported["Temperature_inside"] = tem_DHT_inside_D1_GPIO5;
    state_reported["Temperature_outside"] = tem_DHT_outside_D4_GPIO2;
    state_reported["Humidity_inside"] = hum_DHT_inside_D1_GPIO5;
    state_reported["Humidity_outside"] = hum_DHT_outside_D4_GPIO2;
    state_reported["Soil Moisture_inside"] = Soil_inside;
    state_reported["Soil Moisture_outside"] = Soil_outside;
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
count = count + 1;
  delay(20000);
}




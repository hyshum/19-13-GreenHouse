//https://www.youtube.com/watch?v=AiCa6E_DBL8




//Time stamp --copy---
//https://github.com/jumejume1/NodeMCU_ESP8266/blob/master/READ_TIME_FROM_INTERNET/READ_TIME_FROM_INTERNET.ino
//https://github.com/esp8266/Arduino/blob/master/tools/sdk/libc/xtensa-lx106-elf/include/time.h
#include <time.h>
int timezone = -4 * 3600;
int dst = 0;
//Done copy

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
//https://github.com/bblanchon/ArduinoJson (use v6.xx)
#include <ESP8266AWSIoTMQTTWS.h>
//https://github.com/debsahu/esp8266-arduino-aws-iot-ws

//https://github.com/Links2004/arduinoWebSockets

//https://projects.eclipse.org/projects/technology.paho/downloads
//(download Arduino version)

const char *ssid = "Volcano";
const char *password = "Cannonball";

//Time stamp --copy---
//https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/
//https://github.com/arduino-libraries/NTPClient

#include <NTPClient.h>
#include <WiFiUdp.h>
const long utcOffsetInSeconds = -14400;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
//Done copy

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
int Hourminsec = 0;
int Yearmonthdate = 0;
void setup() {

    Serial.begin(115200);
    while(!Serial) {
        yield();
    }

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

    //Time stamp --copy---
    configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
    while(!time(nullptr)){
      Serial.print("*");
       delay(1000);
     }
    //Done copy
}

void loop() {
  if (client.isConnected()) {
    //Time stamp --copy---
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    Hourminsec = p_tm->tm_sec;
    Hourminsec += p_tm->tm_min*100;
    Hourminsec += p_tm->tm_hour*10000;
    Yearmonthdate = p_tm->tm_mday;
    Yearmonthdate += (p_tm->tm_mon + 1)*100;
    Yearmonthdate += (p_tm->tm_year + 1900)*10000;
    //Done copy

    DynamicJsonDocument jsonBuffer;
    JsonObject root = jsonBuffer.to<JsonObject>();
    JsonObject state = root.createNestedObject("state");
    JsonObject state_reported = state.createNestedObject("reported");
    state_reported["Date"] = Yearmonthdate;
    state_reported["Time"] = Hourminsec;
    state_reported["Temperature_inside"] = random(100);
    state_reported["Temperature_outside"] = random(100);
    state_reported["Humidity"] = random(100);
    state_reported["Soil_Moisture"] = random(100);
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
  delay(20000);
}
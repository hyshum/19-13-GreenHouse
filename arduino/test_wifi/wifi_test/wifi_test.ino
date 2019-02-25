/*
 NodeMCU DHT11

 1.  Vin VCC
 2. GND GND
 3. D3  Data Out
*/

//https://roboindia.com/tutorials/nodeMCU-dht11-thingspeak-data-upload
#include <dht.h> // Including library for dht

#include <ESP8266WiFi.h>

String apiKey = "A7OVYSO7FWW8XYSC"; //  Enter your Write API key from ThingSpeak

const char *ssid = "Hotspot"; // replace with your wifi ssid and wpa2 key
const char *pass = "987654321";
const char *server = "api.thingspeak.com";

#define DHTPIN 0 //pin where the dht11 is connected

dht DHT;

WiFiClient client;

void setup()
{
    Serial.begin(9600);
    delay(10);
    //dht.begin();

    Serial.println("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
}

void loop()
{
    int chk = DHT.read11( DHTPIN );
    double h = DHT.humidity;
    double t = DHT.temperature * 1.8 + 32;

    if (isnan(h) || isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    if (client.connect(server, 80)) //   "184.106.153.149" or api.thingspeak.com
    {

        String postStr  = apiKey;
        postStr += "&field1=";
        postStr += String(t);   //Temperature
        postStr += "&field2=";
        postStr += String(h);   //Humidity

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

        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" degrees Celcius, Humidity: ");
        Serial.print(h);
        Serial.println("%. Send to Thingspeak.");
    }
    client.stop();

    Serial.println("Waiting...");

    // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
    delay(10000);
}

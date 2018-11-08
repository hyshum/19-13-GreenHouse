#include <dht.h>

dht DHT;

#define DHT11_PIN 7

void setup(){
  Serial.begin(9600);
}

void loop()
{
  int chk = DHT.read11(DHT11_PIN);
  double temp = DHT.temperature * 1.8 + 32;
//temp = temp * (9/5);
 // temp = temp + 32;
  Serial.print("Temperature = ");
  Serial.println(temp);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(2000);
}

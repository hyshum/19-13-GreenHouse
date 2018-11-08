#include <dht.h>

dht DHT;
#define DHT11_PIN 4
#define LIGHT_PIN 0

int in1 = 7;
void setup() {
  pinMode(in1, OUTPUT);
  digitalWrite(in1, LOW);
  Serial.begin(9600);
}
void loop() {
  int chk = DHT.read11(DHT11_PIN);
  double temp = DHT.temperature * 1.8 + 32;
  double humidity = DHT.humidity;
  int on = 0;
  int photoCellReading = analogRead( LIGHT_PIN );
  Serial.print("light reading: " );
  Serial.println( photoCellReading );
  Serial.print("Temp = " );
  Serial.println( temp );
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  
  if ( photoCellReading > 200 )
  {
    Serial.println("high");
    digitalWrite( in1, LOW );
  }
  else if ( photoCellReading <= 200)
  {
    Serial.println("low");
    digitalWrite(in1,HIGH);
  }


 delay(3000);
}

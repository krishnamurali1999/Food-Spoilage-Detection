#include<DHT.h>  // An arduino library for reading DHT-11 sensor

#define MQ2_PIN A0
#define MQ3_PIN A1 
#define MQ4_PIN A2
#define MQ135_PIN A3
#define LDR A4

#define DHT11_PIN 8
#define DHTTYPE DHT11


float MQ2_read=0, MQ3_read=0, MQ4_read=0, MQ135_read=0;
float temperature=0, humidity=0;
float LDRReading = 0;

DHT dht(DHT11_PIN, DHTTYPE); // initializing DHT object

void setup() {
  
  Serial.begin(9600);
  dht.begin();
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ3_PIN, INPUT);
  pinMode(MQ4_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  MQ2_read = analogRead(MQ2_PIN);
  MQ3_read = analogRead(MQ3_PIN);
  MQ4_read = analogRead(MQ4_PIN);
  MQ135_read = analogRead(MQ135_PIN);
  LDRReading = analogRead(LDR);
  
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  Serial.print(LDRReading);
  Serial.print(" ");
  Serial.print(MQ2_read);
  Serial.print(" ");
  Serial.print(MQ3_read);
  Serial.print(" ");
  Serial.print(MQ4_read);
  Serial.print(" ");
  Serial.print(MQ135_read);
  Serial.print(" ");
  Serial.print(temperature);
  Serial.print(" ");
  Serial.println(humidity);
  delay(30000);
}

#include<SoftwareSerial.h>
#include <ESP8266WiFi.h>                 // library used for nodemcu module
#include <ThingSpeak.h>                  // library used to upload data to thingspeak.com

SoftwareSerial mySerial(D1, D2);         // rx, tx
                                         // rx - which receives serial data
                                         // tx - transmit serial data 

char* ssid = "Kmurali";                  // WiFi - SSID
char* password = "spectre99";            // WiFi - password
const char* host = "maker.ifttt.com";    // created an applet to send emails

WiFiClient  client;                      // Creates a client that can connect to to a specified internet IP address and port
const int httpPort = 80;
unsigned long myChannelNumber = 1400396; // channel number for thinkspeak
const char* myWriteAPIKey = "08E4IMSBJX21ZBPP"; 


String message, LDR, MQ2, MQ3, MQ4, MQ135, temperature, humidity;
int count_spoiled=0,count_air_quality=0, count=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);                // enables the serial monitor 
  mySerial.begin(9600);                // enables connection to arduino uno
  
  WiFi.mode(WIFI_STA);                 // station mode
  ThingSpeak.begin(client);    
  WiFi.begin(ssid,password);
  
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
   message = mySerial.readStringUntil('\n');
   LDR = getValue(message,' ',0);
   MQ2 = getValue(message,' ',1);
   MQ3 = getValue(message,' ',2);
   MQ4 = getValue(message,' ',3);
   MQ135 = getValue(message,' ',4);
   temperature = getValue(message,' ',5);
   humidity = getValue(message,' ',6);
   Serial.println(message);

  thingspeak(temperature, humidity, MQ2, MQ3, MQ4, MQ135, LDR); // sends the sensor data to the thingspeak.com

  sending_email(); // sends an email via an applet created in ifttt.com 
  
  delay(40000);

}



String getValue(String data, char sep, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==sep || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}



void thingspeak(String temperature, String humidity, String MQ2, String MQ3, String MQ4, String MQ135, String LDR) {
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, MQ2);
  ThingSpeak.setField(4, MQ3);
  ThingSpeak.setField(5, MQ4);
  ThingSpeak.setField(6, MQ135);
  ThingSpeak.setField(7, LDR);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}



void sending_email() {
  if(!client.connect(host, httpPort)) {
      Serial.println("email connection failed");
      return;
    }

  if(count == 15) {
      count_spoiled = 0;
      count_air_quality = 0;
      count=0;
    }
    
  if(count_spoiled!=3 && MQ2.toInt() >= 470 && MQ3.toInt() >= 345 && MQ4.toInt() >= 280) {
    String url1 = "/trigger/food_spoiled/with/key/oOAEkILKPPinkD1S3IJ5I7FCYNdDNmWFzlgsNFoqBD";
    Serial.print("Requesting url: ");
    Serial.println(url1);
    client.print(String("GET ") + url1 + " HTTP/1.1\r\n" + 
                                    "Host: " + host + "\r\n" +   
                                    "Connection: close\r\n\r\n");    
                                      
    count_spoiled += 1;
    } else {
      Serial.println("email not sent because food is not spoiled yet."); 
    }   
  delay(5000);   
  if(count_air_quality!=3 && LDR.toInt() > 300 && MQ135.toInt() > 270) {
    String url = "/trigger/air_quality_and_light_around_the_food_is_not_good-Move_your_food_to_a_darker_place/with/key/oOAEkILKPPinkD1S3IJ5I7FCYNdDNmWFzlgsNFoqBD";
    Serial.print("Requesting url: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                                    "Host: " + host + "\r\n" +   
                                    "Connection: close\r\n\r\n"); 
  count_air_quality += 1;
  } else {
    Serial.println("email not sent because air quality is good.");
  }
  count+=1;
}

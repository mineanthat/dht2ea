#include "DHT.h"
#include <ESP8266WiFi.h>
#include <MicroGear.h>

const char* ssid     = "<Enter your WiFi Name>";
const char* password = "<Enter your WiFi Password>";

#define DHTPIN1 D1     
#define DHTPIN2 D2
#define DHTTYPE DHT11   // DHT 11
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

#define APPID   "<Enter your Application ID>"
#define KEY     "<Enter your Key>"
#define SECRET  "<Enter Your Secret>"
#define ALIAS   "dht2ea"

WiFiClient client;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

void setup() {

  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);
  
  Serial.begin(115200);
  Serial.println("Test DHT11 2 ea");

  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  }

  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  microgear.init(KEY,SECRET,ALIAS);
  microgear.connect(APPID);

  dht1.begin();
  dht2.begin();
}

void loop() {

  if (microgear.connected()) {
    Serial.println("connected");
    microgear.loop();
    
    float h1 = dht1.readHumidity();
    float t1 = dht1.readTemperature();
    
    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();

    Serial.print("Humidity1: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature1: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.println();
    Serial.print("Humidity2: ");
    Serial.print(h2);
    Serial.print(" %\t");
    Serial.print("Temperature2: ");
    Serial.print(t2);
    Serial.print(" *C ");
    Serial.println();

    String data = "{\"humid1\":";
            data += h1 ;
            data += ", \"temp1\":";
            data += t1 ;
			data += ", \"humid2\":";
			data += h2 ;
			data += ", \"temp2\":";
			data += t2 ;
            data += "}";
    
    if ( isnan(h1) || isnan(t1) || isnan(h2) || isnan(t2)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    } else {
      microgear.publish("/dht1/humid", h1);
      microgear.publish("/dht1/temp", t1);
      microgear.publish("/dht2/humid", h2);
      microgear.publish("/dht2/temp", t2);
      Serial.println("Publish success.....");
      microgear.writeFeed("TestFeedSolve",data);
      Serial.println("Write Feed Success....");
	  delay(15000);
    }    
  } else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(100);
}

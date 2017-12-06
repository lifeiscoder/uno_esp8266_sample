#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include "DHT.h"

#define GAS A0
#define FIRE 2
#define RAINWATER A1
#define DUST A2

#define DHTPIN 4
#define DHTTYPE DHT22

#define SSID ""
#define PASSWORD ""

#define HOST "192.168.0.16"
#define PORT "7575"

SoftwareSerial wifi(6, 7);
DHT dht(DHTPIN, DHTTYPE);
bool isAvailable = false;

void connectWifi();
void requestGet(const char *path);

void setup() {
  Serial.begin(9600);
  wifi.begin(9600);
  dht.begin();
  connectWifi();
}

void loop() {
  String url;
  if (!isAvailable) return;
  
  // This section is about gas sensor
  int gas = analogRead(GAS);
  Serial.println(gas);
  url = "/gas/";
  url += gas;
  requestGet(url.c_str());
  delay(250);

  // This section is about detect fire
  int fire = digitalRead(FIRE);
  if (fire == LOW) {
    Serial.println("detect fire");
    url = "/fire/1";
  } else {
    Serial.println("safe for fire");
    url = "/fire/0";
  }
  requestGet(url.c_str());
  delay(250);

  // This section is about the rain
  int rainwater = analogRead(RAINWATER);
  Serial.println(rainwater);
  url = "/rainwater/";
  url += rainwater;
  requestGet(url.c_str());
  delay(250);

  // This section is about DHT22 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  delay(250);

  if (!isnan(humidity) && !isnan(temperature)) {
    Serial.println(humidity);
    Serial.println(temperature);
    url = "/humidity/";
    url += humidity;  
    requestGet(url.c_str());
    
    delay(250);
    url = "/temperature/";
    url += temperature;
    requestGet(url.c_str());
  }

  delay(250);

  // This section is about dust sensor
  //float dust = analogRead(DUST);
  //Serial.println(dust);
}

// connect wifi! 
void connectWifi() {
  char cmd[50];
  sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", SSID, PASSWORD);
  wifi.println(cmd);

  while (wifi.readString().indexOf("OK") == -1);
  Serial.println("connected WIFI");
  isAvailable = true;
}

// request host/path
void requestGet(const char *path) {
  char cmd[50];
  String data;
  
  sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", HOST, PORT);
  wifi.println(cmd);

  while(!wifi.available());
  if (wifi.readString().indexOf("OK") >= 0) {
    Serial.println("connected web");
  } else {
    Serial.println("failed connect to web");
    return;
  }

  data = "GET ";
  data += path;
  data += " HTTP/1.1\r\nHost: ";
  data += HOST;
  data += "\r\n\r\n";
  sprintf(cmd, "AT+CIPSEND=%d", data.length());
  
  wifi.println(cmd);
  wifi.println(data);

  while(wifi.available()) 
    wifi.read();
  //wifi.println("AT+CIPCLOSE");
}


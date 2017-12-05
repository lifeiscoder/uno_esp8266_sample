#include <SoftwareSerial.h>
#include "DHT.h"

#define GAS A0
#define FIRE 2
#define RAINWATER A1
#define DUST A2

#define DHTPIN 4
#define DHTTYPE DHT22

#define SSID "wifi_ssid"
#define PASSWORD "wifi_password"

#define HOST "host_ip"

SoftwareSerial wifi(6, 7);
DHT dht(DHTPIN, DHTTYPE);
bool isAvailable = false;

void connectWifi();
void requestGet(const char *data);

void setup() {
  Serial.begin(9600);
  wifi.begin(9600);
  dht.begin();
  connectWifi();
  requestGet("/company/readServices");
}

void loop() {
  if (!isAvailable) return;
  
  // This section is about gas sensor
  int gas = analogRead(GAS);
  Serial.println(gas);

  // This section is about detect fire
  int fire = digitalRead(FIRE);
  if (digitalRead(fire) == HIGH) {
    Serial.println("detect fire");
  } else {
    Serial.println("safe for fire");
  }

  // This section is about the rain
  int rainwater = analogRead(RAINWATER);
  Serial.println(rainwater);

  // This section is about DHT22 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (!isnan(humidity) && !isnan(temperature)) {
    Serial.println(humidity);
    Serial.println(temperature);
  }

  // This section is about dust sensor
  float dust = analogRead(DUST);
  Serial.println(dust);
  
  delay(1000);
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
  char data[1024];
  
  sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",80", HOST);
  wifi.println(cmd);

  while(!wifi.available());
  if (wifi.readString().indexOf("OK") >= 0) {
    Serial.println("connected web");
  } else {
    Serial.println("failed connect to web");
    return;
  }

  sprintf(data, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, HOST);
  
  sprintf(cmd, "AT+CIPSEND=%d", strlen(data));
  wifi.println(cmd);
  wifi.println(data);

  while(wifi.available())
    Serial.println(wifi.readString()); // this is not working...couldn't receive data but success to request~
  
  wifi.println("AT+CIPCLOSE");
}


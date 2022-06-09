#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ctime>
#include <Adafruit_Sensor.h>

#include "wifi_conf.h"

const char *ssid = WifiConf::SSID;
const char *password = WifiConf::PASSWORD;

void connectToWifi();

void setup() {
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) {
    connectToWifi();

    if (WiFi.status() != WL_CONNECTED){
      delay(1000);
    }
  }

}

void loop() {

  // auto reconnect wifi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    Serial.println("Reconnecting...");
    
    connectToWifi();
    
    if (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
  }

}

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}

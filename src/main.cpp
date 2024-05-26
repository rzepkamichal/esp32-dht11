#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <PubSubClient.h>


#include "wifi_conf.h"

#define DHTPIN 16
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = WifiConf::SSID;
const char* password = WifiConf::PASSWORD;
const char* MQTT_BROKER_HOST = "192.168.0.8";
const int MQTT_BROKER_PORT = 1883;
const char* MQTT_TOPIC_TEMP = "esp32/t";
const char* MQTT_TOPIC_HMDT = "esp32/h";

WiFiClient wiFiClient;
PubSubClient mqttClient(wiFiClient);

void attemptWiFiConnection();
void attemptMqttConnection();
void readSensorsAndPublish();

void setup() {
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) {
    attemptWiFiConnection();

    if (WiFi.status() != WL_CONNECTED){
      delay(1000);
    }
  }

  mqttClient.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  dht.begin();
}

void loop() {

  // auto reconnect wifi
  if (WiFi.status() != WL_CONNECTED) {
    attemptWiFiConnection();
    
    if (WiFi.status() != WL_CONNECTED) {
      delay(2000);
      return;
    }
  }

  if (!mqttClient.connected()) {
    attemptMqttConnection();

    if (!mqttClient.connected()) {
      delay(2000);
      return;
    }
  }

  readSensorsAndPublish();
  delay(5000);
}

void attemptWiFiConnection() {
  Serial.println("WiFi Disconnected");
  Serial.println("Reconnecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}

void attemptMqttConnection() {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    }
}

void readSensorsAndPublish() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println();

  char tempString[8];
  dtostrf(t, 1, 2, tempString);
  mqttClient.publish(MQTT_TOPIC_TEMP, tempString);

  char hmdtString[8];
  dtostrf(h, 1, 2, hmdtString);
  mqttClient.publish(MQTT_TOPIC_HMDT, hmdtString);
}

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "config.h"

const char* server = "api.weatherapi.com";
const int httpPort = 443;
void sendRequest();

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient wifiClient;
    DynamicJsonDocument doc(8192);

    http.begin(wifiClient,"http://api.weatherapi.com/v1/current.json?key=" + apiToken +"&q=Kiev&aqi=no");

    int httpResponseCode = http.GET();

    if (httpResponseCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println(response);

      DeserializationError error = deserializeJson(doc, response);
      if(error){
        Serial.print("Deserialization error: ");
        Serial.println(error.c_str());
        return;
      }

      const char *name = doc["location"]["name"];
      const char* country = doc["location"]["country"];
      const char* localtime = doc["location"]["localtime"];
      float temp_c = doc["current"]["temp_c"];
      float feelslike_c = doc["current"]["feelslike_c"];
      Serial.print("Name: ");
      Serial.println(name);
      Serial.print("Country: ");
      Serial.println(country);
      Serial.print("Local Time: ");
      Serial.println(localtime);
      Serial.print("Temperature (C): ");
      Serial.println(temp_c);
      Serial.print("Feels Like (C): ");
      Serial.println(feelslike_c);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();

    delay(500000000);
  }

}
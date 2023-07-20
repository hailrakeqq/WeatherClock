#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* server = "api.weatherapi.com";
const int httpPort = 443;

struct WeatherData {
    const char* name;
    const char* country;
    const char* localtime;
    float temp_c;
    float feelslike_c;
};


void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the I2C address of the display
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  Serial.println("Connected to WiFi");
}

void updateDisplay(const WeatherData& weather){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.print(weather.name);
    display.print(", ");
    display.println(weather.country);

    display.println(weather.localtime);

    display.print("Temperature: ");
    display.print(weather.temp_c);
    display.println("C");

    display.print("Feels Like: ");
    display.print(weather.feelslike_c);
    display.println("C");

    display.display();
}

WeatherData GetWeatherData() {
  WeatherData weather;
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
        return weather;
      }

      weather.name = doc["location"]["name"];
      weather.country = doc["location"]["country"];
      weather.localtime = doc["location"]["localtime"];
      weather.temp_c = doc["current"]["temp_c"];
      weather.feelslike_c = doc["current"]["feelslike_c"];

      http.end();

      return weather;
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      http.end();
    }
  }
  return weather;
}

void loop() {
  WeatherData weather = GetWeatherData();
  updateDisplay(weather);
  delay(60000);
}

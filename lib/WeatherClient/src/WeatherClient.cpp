#include "WeatherClient.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "WiFiManager.h"
#include "../../../include/Config.h"

WeatherClientClass WeatherClient;

WeatherClientClass::WeatherClientClass() {
}

void WeatherClientClass::begin() {
    // nothing for now
}

const WeatherData& WeatherClientClass::get() const {
    return data;
}

bool WeatherClientClass::update(unsigned long minIntervalMs) {
    unsigned long now = millis();
    Serial.println("WeatherClient: update requested");
    if(data.valid && (now - data.lastUpdate) < minIntervalMs) {
        Serial.println("WeatherClient: cache fresh, skipping fetch");
        return false;
    }
    if(!WiFiManager.isConnected()) {
        Serial.println("WeatherClient: WiFi not connected, skipping fetch");
        return false;
    }
    if(fetch()) {
        data.lastUpdate = now;
        data.valid = true;
        Serial.println("WeatherClient: fetch succeeded");
        return true;
    }
    Serial.println("WeatherClient: fetch failed");
    return false;
}

bool WeatherClientClass::fetch() {
#if !defined(OPENWEATHER_API_KEY) || !defined(OPENWEATHER_LAT) || !defined(OPENWEATHER_LON)
    Serial.println("WeatherClient: API key or coordinates not defined, skipping fetch");
    return false;
#endif
    String url = String("https://api.openweathermap.org/data/2.5/weather?lat=") + String(OPENWEATHER_LAT, 6) + "&lon=" + String(OPENWEATHER_LON, 6) + "&units=metric&appid=" + String(OPENWEATHER_API_KEY);
    Serial.print("WeatherClient: fetching URL: ");
    Serial.println(url);

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    Serial.print("WeatherClient: HTTP code: "); Serial.println(httpCode);
    if(httpCode != HTTP_CODE_OK) {
        Serial.println("WeatherClient: HTTP request failed");
        http.end();
        return false;
    }
    String payload = http.getString();
    http.end();

    Serial.print("WeatherClient: payload size: "); Serial.println(payload.length());

    // Parse JSON
    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, payload);
    if(err) {
        Serial.print("WeatherClient: JSON parse error: "); Serial.println(err.c_str());
        return false;
    }

    if(doc.containsKey("name")) {
        data.city = String((const char*)doc["name"]);
        Serial.print("WeatherClient: city= "); Serial.println(data.city);
    }
    if(doc.containsKey("main") && doc["main"].containsKey("temp")) {
        data.tempC = doc["main"]["temp"].as<float>();
        Serial.print("WeatherClient: tempC= "); Serial.println(data.tempC);
    }
    if(doc.containsKey("weather") && doc["weather"].is<JsonArray>() && doc["weather"][0].containsKey("main")) {
        data.condition = String((const char*)doc["weather"][0]["main"]);
        Serial.print("WeatherClient: condition= "); Serial.println(data.condition);
    }
    return true;
}

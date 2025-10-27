#include "EdgeServiceClient.h"
#include "HeartRateLog.h"
#include "config.h"

EdgeServiceClient::EdgeServiceClient()
  : ssid(WIFI_SSID), password(WIFI_PASSWORD), backendUrl(EDGE_BACKEND_URL), smartBandId(SMART_BAND_ID) {}

void EdgeServiceClient::connectWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.print(".");
        delay(500);
        attempts++;
    }
    Serial.println();
    if(WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected. IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Failed to connect to WiFi");
    }
}

void EdgeServiceClient::setSmartBandId(int id) {
    smartBandId = id;
}

void EdgeServiceClient::sendHeartRateData(int heartRate) {
    if (!isConnected()) {
        Serial.println("WiFi not connected. Reconnecting...");
        connectWifi();
    }

    HeartRateLog* heartRateLog = HeartRateLog::getInstance();
    heartRateLog->setHeartRate(heartRate);
    heartRateLog->setSmartBandId(smartBandId);
    
    String jsonData = "{\"smartBandId\":" + String(heartRateLog->getSmartBandId()) + 
                  ",\"pulse\":\"" + String(heartRateLog->getHeartRate()) + "\"}";

    Serial.println("Sending data to backend:");
    Serial.println("URL: " + String(backendUrl));
    Serial.println("Data: " + jsonData);

    client_http.begin(backendUrl);
    client_http.addHeader("Content-Type", "application/json");

    int httpCode = client_http.POST(jsonData);

    if (httpCode > 0) {
        String payload = client_http.getString();
        Serial.println("Status code: " + String(httpCode));
        Serial.println("Response: " + payload);
    } else {
        Serial.println("Error sending data to backend");
        Serial.println("Error: " + client_http.errorToString(httpCode));
    }

    client_http.end();
}

bool EdgeServiceClient::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}
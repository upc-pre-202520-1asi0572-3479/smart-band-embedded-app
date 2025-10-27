#include "Device.h"
#include <Wire.h>

Device::Device() 
  : edgeServiceClient(),
    pulseSimulator() {
    heartRateLog = HeartRateLog::getInstance();
}

void Device::setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    pinMode(LED_PIN, OUTPUT);
    Serial.println("Hello, ESP32!");
    edgeServiceClient.connectWifi();
    edgeServiceClient.setSmartBandId(SMART_BAND_ID);
    pulseSimulator.begin(PULSE_PIN);
}

void Device::loop() {
    float voltage = pulseSimulator.readVoltage();
    int heartRate = calculateHeartRate(voltage);

    Serial.print("\nHeart Rate: ");
    Serial.println(heartRate);
    
    sendDataToBackend(heartRate);
    controlLed(heartRate);
    
    delay(500);
}

void Device::sendDataToBackend(int heartRate) {
    edgeServiceClient.sendHeartRateData(heartRate);
}

int Device::calculateHeartRate(float voltage) {
    return (voltage / 3.3) * 675;
}

void Device::controlLed(int heartRate) {
    if (heartRate < MIN_HEART_RATE || heartRate > MAX_HEART_RATE) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
}
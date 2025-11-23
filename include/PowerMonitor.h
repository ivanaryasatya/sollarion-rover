/* contoh penggunaan
#include <Arduino.h>
#include "PowerMonitor.h"

// Pin analog sensor
#define CURRENT_PIN 34
#define VOLTAGE_PIN 35

// Parameter sensor
// Contoh: ACS712 5A → 0.185 V/A
float currentSensitivity = 0.185;

// Contoh voltage divider: R1=30k, R2=7.5k → rasio = (R1+R2)/R2 = 5
float voltageDividerRatio = 5.0;

// Buat object
PowerMonitor power(CURRENT_PIN, VOLTAGE_PIN, currentSensitivity, voltageDividerRatio);

void setup() {
    Serial.begin(115200);

    power.begin();
    Serial.println("Power Monitor Started!");
}

void loop() {
    float current = power.readCurrent();
    float voltage = power.readVoltage();
    float powerW  = power.readPower();

    Serial.print("Current: ");
    Serial.print(current);
    Serial.println(" A");

    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println(" V");

    Serial.print("Power: ");
    Serial.print(powerW);
    Serial.println(" W");

    Serial.println("--------------------------");

    delay(1000);
}

*/


#pragma once
#include <Arduino.h>

class PowerMonitor {
public:
    PowerMonitor(uint8_t currentPin, uint8_t voltagePin, float currentSensitivity, float voltageDividerRatio)
        : _currentPin(currentPin), _voltagePin(voltagePin),
          _currentSensitivity(currentSensitivity), _voltageDividerRatio(voltageDividerRatio) {}

    void begin() {
        pinMode(_currentPin, INPUT);
        pinMode(_voltagePin, INPUT);
    }

    float readCurrent() {
        int raw = analogRead(_currentPin);
        float voltage = (raw / 4095.0f) * 3.3f;
        float current = voltage / _currentSensitivity;
        return current;
    }

    float readVoltage() {
        int raw = analogRead(_voltagePin);
        float adcVoltage = (raw / 4095.0f) * 3.3f;
        float realVoltage = adcVoltage * _voltageDividerRatio;
        return realVoltage;
    }

    // Hitung daya (Watt)
    float readPower() {
        return readVoltage() * readCurrent();
    }

private:
    uint8_t _currentPin;
    uint8_t _voltagePin;
    float _currentSensitivity;   // contoh: ACS712 5A -> 0.185 V/A
    float _voltageDividerRatio;  // contoh: R1=30k, R2=7.5k -> 5x
};

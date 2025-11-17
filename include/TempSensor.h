/*
EXAMPLE:
#include "DHTSensor.h"

DHTSensor dht(2, DHT22);   // pin 2, tipe DHT22

void setup() {
    Serial.begin(9600);
    dht.begin();
}

void loop() {
    dht.update();  // penting! harus dipanggil berkala

    Serial.print("Temp: ");
    Serial.print(dht.getTemperature());
    
    Serial.print("  Hum: ");
    Serial.print(dht.getHumidity());
    
    Serial.print("  HeatIndex: ");
    Serial.println(dht.getHeatIndex());

    delay(500);
}

*/




#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <Arduino.h>
#include <DHT.h>

class DHTSensor {
private:
    DHT dht;
    float temperature;
    float humidity;

    unsigned long lastReadTime = 0;
    unsigned long readInterval = 1500; // minimal 1 detik agar hasil stabil

public:

    DHTSensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

    void begin() {
        dht.begin();
        temperature = 0;
        humidity = 0;
    }

    // Membaca sensor setiap interval
    void update() {
        unsigned long now = millis();

        if (now - lastReadTime < readInterval) return;
        lastReadTime = now;

        float h = dht.readHumidity();
        float t = dht.readTemperature();      // Celcius
        // float f = dht.readTemperature(true); // Fahrenheit

        // Cek error baca
        if (isnan(h) || isnan(t)) {
            // Jika error, jangan update nilai agar data sebelumnya tetap aman
            return;
        }

        humidity = h;
        temperature = t;
    }

    // ============================
    // Getter
    // ============================

    float getTemperature() {
        return temperature;
    }

    float getHumidity() {
        return humidity;
    }

    float getHeatIndex() {
        return dht.computeHeatIndex(temperature, humidity, false); // false = Celcius
    }

    // Mendapatkan semua data sekaligus (opsional)
    void getAll(float &temp, float &hum, float &heatIndex) {
        temp = temperature;
        hum = humidity;
        heatIndex = getHeatIndex();
    }

    // Mengatur interval pembacaan (ms)
    void setInterval(unsigned long ms) {
        readInterval = ms;
    }
};

#endif

/*
#include "Compass.h"

Compass compass(0.0); // sesuaikan deklinasi lokal

void setup() {
  Serial.begin(115200);
  if (!compass.begin()) {
      while(1); // berhenti jika sensor gagal
  }
  compass.calibrate();
}

void loop() {
  compass.printInfo();
  delay(500);
}
*/








#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>  // Contoh pakai HMC5883L

class Compass {
private:
    Adafruit_HMC5883_Unified mag; // Objek sensor kompas
    float declination; // Deklinasi magnetik lokal

public:
    // Constructor
    Compass(float decl = 0.0) { 
        declination = decl; 
    }

    // Inisialisasi sensor
    bool begin() {
        if (!mag.begin()) {
            Serial.println("Sensor kompas tidak terdeteksi!");
            return false;
        }
        Serial.println("Sensor kompas siap.");
        return true;
    }

    // Baca arah dalam derajat 0-360 (0=utara)
    float readHeading() {
        sensors_event_t event;
        mag.getEvent(&event);

        // Hitung arah
        float heading = atan2(event.magnetic.y, event.magnetic.x);
        heading += declination; // koreksi deklinasi
        if (heading < 0) heading += 2 * PI;
        if (heading > 2 * PI) heading -= 2 * PI;

        float headingDegrees = heading * 180.0 / PI;
        return headingDegrees;
    }

    // Fungsi untuk cek arah mata angin
    String getCardinal(float headingDegrees) {
        if (headingDegrees >= 337.5 || headingDegrees < 22.5) return "Utara";
        else if (headingDegrees < 67.5) return "Timur Laut";
        else if (headingDegrees < 112.5) return "Timur";
        else if (headingDegrees < 157.5) return "Tenggara";
        else if (headingDegrees < 202.5) return "Selatan";
        else if (headingDegrees < 247.5) return "Barat Daya";
        else if (headingDegrees < 292.5) return "Barat";
        else return "Barat Laut";
    }

    // Fungsi bantu kalibrasi (tampilan serial)
    void calibrate(int delayTime = 500) {
        Serial.println("Putar sensor kompas untuk kalibrasi...");
        for (int i = 0; i < 36; i++) {
            float deg = readHeading();
            Serial.print("Arah sekarang: ");
            Serial.println(deg);
            delay(delayTime);
        }
        Serial.println("Kalibrasi selesai.");
    }

    // Fungsi print info kompas ke Serial
    void printInfo() {
        float deg = readHeading();
        Serial.print("Heading: ");
        Serial.print(deg);
        Serial.print("° (");
        Serial.print(getCardinal(deg));
        Serial.println(")");
    }
};
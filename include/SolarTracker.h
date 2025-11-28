#pragma once

#include <Arduino.h>
#include <Servo.h>

class LightFollower {
private:
    uint8_t ldrPins[4];   // Pin LDR: 0=FrontLeft, 1=FrontRight, 2=BackLeft, 3=BackRight
    Servo servoX;         // Servo horizontal
    Servo servoY;         // Servo vertikal
    int servoXPin, servoYPin;
    int servoXPos, servoYPos; // posisi servo saat scan
    int servoStep;           // step scan servo

public:
    // Constructor: masukkan pin LDR & servo
    LightFollower(uint8_t pins[4], int sXPin, int sYPin, int step = 10) {
        for (int i = 0; i < 4; i++) ldrPins[i] = pins[i];
        servoXPin = sXPin;
        servoYPin = sYPin;
        servoStep = step;
        servoXPos = 90; // tengah
        servoYPos = 90; // tengah
    }

    void begin() {
        for (int i = 0; i < 4; i++) pinMode(ldrPins[i], INPUT);
        servoX.attach(servoXPin);
        servoY.attach(servoYPin);
        servoX.write(servoXPos);
        servoY.write(servoYPos);
    }

    // Scan cahaya dengan servo dan baca LDR
    void scanAndRead() {
        // contoh sederhana: servo bergerak horizontal dari 0-180
        servoX.write(servoXPos);
        servoY.write(servoYPos);
        delay(50); // tunggu servo
        servoXPos += servoStep;
        if (servoXPos > 180 || servoXPos < 0) servoStep = -servoStep;
    }

    // Baca nilai LDR
    void readLDR(int &fl, int &fr, int &bl, int &br) {
        fl = analogRead(ldrPins[0]);
        fr = analogRead(ldrPins[1]);
        bl = analogRead(ldrPins[2]);
        br = analogRead(ldrPins[3]);
    }

    // Hitung arah gerak berdasarkan LDR
    // Output: derajat mata angin 0-360 (0=utara, 90=timur, dll.)
    int getLightDirection() {
        int fl, fr, bl, br;
        readLDR(fl, fr, bl, br);

        // Berat tiap sensor (FL=NW, FR=NE, BL=SW, BR=SE)
        float x = (fr + br) - (fl + bl); // sumbu timur-barat
        float y = (fl + fr) - (bl + br); // sumbu utara-selatan

        float angle = atan2(x, y) * 180.0 / PI; // sudut dari utara
        if (angle < 0) angle += 360;
        return int(angle);
    }

    // Fungsi bantu set posisi servo manual
    void setServoPos(int x, int y) {
        servoXPos = constrain(x, 0, 180);
        servoYPos = constrain(y, 0, 180);
        servoX.write(servoXPos);
        servoY.write(servoYPos);
    }
};

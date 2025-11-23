#pragma once

#include <Arduino.h>

class MotorDriver {
private:
    // LEFT motors
    int in1L, in2L, enL;

    // RIGHT motors
    int in1R, in2R, enR;

    int maxSpeed = 255;

public:

    MotorDriver(int in1_left, int in2_left, int en_left,
                int in1_right, int in2_right, int en_right)
    {
        in1L = in1_left;
        in2L = in2_left;
        enL  = en_left;

        in1R = in1_right;
        in2R = in2_right;
        enR  = en_right;
    }

    void begin() {
        pinMode(in1L, OUTPUT);
        pinMode(in2L, OUTPUT);
        pinMode(enL, OUTPUT);

        pinMode(in1R, OUTPUT);
        pinMode(in2R, OUTPUT);
        pinMode(enR, OUTPUT);
    }

    // ----------------------
    //  Basic Motor Control
    // ----------------------

    void stopAll() {
        analogWrite(enL, 0);
        analogWrite(enR, 0);
        digitalWrite(in1L, LOW);
        digitalWrite(in2L, LOW);
        digitalWrite(in1R, LOW);
        digitalWrite(in2R, LOW);
    }

    void forward(int speedL, int speedR) {
        speedL = constrain(speedL, 0, maxSpeed);
        speedR = constrain(speedR, 0, maxSpeed);

        digitalWrite(in1L, HIGH);
        digitalWrite(in2L, LOW);

        digitalWrite(in1R, HIGH);
        digitalWrite(in2R, LOW);

        analogWrite(enL, speedL);
        analogWrite(enR, speedR);
    }

    void backward(int speedL, int speedR) {
        speedL = constrain(speedL, 0, maxSpeed);
        speedR = constrain(speedR, 0, maxSpeed);

        digitalWrite(in1L, LOW);
        digitalWrite(in2L, HIGH);

        digitalWrite(in1R, LOW);
        digitalWrite(in2R, HIGH);

        analogWrite(enL, speedL);
        analogWrite(enR, speedR);
    }

    // steer > 0 = belok kanan
    // steer < 0 = belok kiri
    // steer: 0 - 255
    void driveForwardWithSteer(int baseSpeed, int steer) {
        steer = constrain(steer, -255, 255);
        baseSpeed = constrain(baseSpeed, 0, 255);

        int speedL = baseSpeed;
        int speedR = baseSpeed;

        if (steer > 0) {
            // belok kanan → kurangi roda kanan
            speedR = max(baseSpeed - steer, 0);
        } else if (steer < 0) {
            // belok kiri → kurangi roda kiri
            speedL = max(baseSpeed + steer, 0); // steer negatif
        }

        forward(speedL, speedR);
    }

    void driveBackwardWithSteer(int baseSpeed, int steer) {
        steer = constrain(steer, -255, 255);
        baseSpeed = constrain(baseSpeed, 0, 255);

        int speedL = baseSpeed;
        int speedR = baseSpeed;

        if (steer > 0) {
            // belok kanan sambil mundur
            speedR = max(baseSpeed - steer, 0);
        } else if (steer < 0) {
            // belok kiri sambil mundur
            speedL = max(baseSpeed + steer, 0);
        }

        backward(speedL, speedR);
    }
};


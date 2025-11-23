#pragma once
#include <Arduino.h>

class DualJoystick { //joystick ganda
private:
  int x1Pin, y1Pin;
  int x2Pin, y2Pin;

  int x1, y1;
  int x2, y2;

  int mapAxis(int v) {
    return map(v, 0, 1023, -512, 512);
  }

public:
  DualJoystick(int _x1, int _y1, int _x2, int _y2)
    : x1Pin(_x1), y1Pin(_y1), x2Pin(_x2), y2Pin(_y2) {}

  void begin() {
    pinMode(x1Pin, INPUT);
    pinMode(y1Pin, INPUT);
    pinMode(x2Pin, INPUT);
    pinMode(y2Pin, INPUT);
  }

  void read() {
    x1 = mapAxis(analogRead(x1Pin));
    y1 = mapAxis(analogRead(y1Pin));
    x2 = mapAxis(analogRead(x2Pin));
    y2 = mapAxis(analogRead(x2Pin));
  }

  int getX1() { return x1; }
  int getY1() { return y1; }
  int getX2() { return x2; }
  int getY2() { return y2; }
};
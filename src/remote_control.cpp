#include <SPI.h>
#include <Arduino.h>
#include "RadioDataPacker.h"
#include "RadioCom.h"
#include "radioComCode.h"
#include "DualJoystick.h"

RadioDataPacker radioDataPacker;

// pins ==============================================
const byte RF24_csn_pin = 8;
const byte RF24_ce_pin  = 7;
const byte joystick1_x_pin = A0;
const byte joystick1_y_pin = A1;
const byte joystick2_x_pin = A0;
const byte joystick2_y_pin = A1;
const byte joystick1_button_pin = 2;
const byte joystick2_button_pin = 3;
// pins end ==========================================

String radioDataTransmit = "";
String radioDataReceive = "";

DualJoystick joy(joystick1_x_pin, joystick1_y_pin, joystick2_x_pin, joystick2_y_pin);
NRF24Radio radio(RF24_ce_pin, RF24_csn_pin, "ROVct");  // CE, CSN, address
Unpacked unpacked;

void setup() {
  Serial.begin(9600);
  radio.begin(true);
}

void loop() {
  

  radioDataPacker.clear();
  radioDataPacker.add(&joystick1_x, joy.getX1());
  radioDataPacker.add(&joystick1_y, joy.getY1());
  radioDataPacker.add(&joystick2_x, joy.getX2());
  radioDataPacker.add(&joystick2_y, joy.getY2());

  radioDataTransmit = radioDataPacker.get().c_str();
  radio.send(&radioDataTransmit, sizeof(radioDataTransmit));

  radioDataPacker.clear();
  delay(100);
}




//==================


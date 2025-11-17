#include <SPI.h>
#include <RF24.h>


#include "DHTSensor.h"
s
RF24 radio(7, 8); // CE, CSN
const byte pipe[6] = "00001";

struct ControlData {
  int16_t x;
  int16_t y;
};

ControlData data;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
  radio.stopListening();
}

void loop() {
  data.x = analogRead(A0); // kiri/kanan
  data.y = analogRead(A1); // maju/mundur

  radio.write(&data, sizeof(data));

  delay(20);
}

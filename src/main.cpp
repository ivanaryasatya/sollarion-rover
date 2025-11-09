#include "RadioCom.h"

RadioCom radio(9, 10, "00001", true); // CE, CSN, alamat, transmitter

void setup() {
  Serial.begin(9600);
  radio.begin();
}

void loop() {
  const char msg[] = "Halo Ivan!";
  radio.send(msg, sizeof(msg));
  delay(1000);
} //testasas

v#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001"; // Alamat komunikasi

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address); // Atur alamat tujuan
  radio.stopListening(); // Mode kirim
  Serial.println("Transmitter siap!");
}

void loop() {
  const char text[] = "Halo dari Transmitter!";
  radio.write(&text, sizeof(text));
  Serial.println("Pesan dikirim: Halo dari Transmitter!");
  delay(1000);
}

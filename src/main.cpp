#include <SPI.h>
#include <RF24.h>
// #include "NRF24Radio.h"

class NRF24Radio {
private:
    RF24 radio;
    uint8_t address[6];

public:
    // Constructor
    NRF24Radio(uint8_t cePin, uint8_t csnPin, const char* addr = "00001")
        : radio(cePin, csnPin)
    {
        memcpy(address, addr, 6);
    }

    // Inisialisasi NRF24
    bool begin(bool isTransmitter = true) {

        if (!radio.begin()) return false;

        radio.setPALevel(RF24_PA_LOW);
        radio.setDataRate(RF24_250KBPS);
        radio.setChannel(76);

        radio.openWritingPipe(address);
        radio.openReadingPipe(1, address);

        if (isTransmitter)
            radio.stopListening();     // Mode TX
        else
            radio.startListening();    // Mode RX

        return true;
    }

    // Kirim data apa pun
    bool send(const void* data, uint8_t len) {
        radio.stopListening();
        return radio.write(data, len);
    }

    // Terima data apa pun
    bool receive(void* data, uint8_t len) {
        radio.startListening();
        if (radio.available()) {
            radio.read(data, len);
            return true;
        }
        return false;
    }

    // Ganti address kapan saja
    void setAddress(const char* addr) {
        memcpy(address, addr, 6);
        radio.openWritingPipe(address);
        radio.openReadingPipe(1, address);
    }
};



NRF24Radio radio(7, 8, "NODE1");  // CE, CSN, address

void setup() {
  Serial.begin(9600);

  if (radio.begin(true)) {
    Serial.println("Transmitter siap!");
  } else {
    Serial.println("Gagal inisialisasi NRF24.");
  }
}

void loop() {
  const char msg[] = "Halo Ivan!";

  if (radio.send(msg, sizeof(msg))) {
    Serial.println("Terkirim!");
  } else {
    Serial.println("Gagal kirim!");
  }

  delay(1000);
}

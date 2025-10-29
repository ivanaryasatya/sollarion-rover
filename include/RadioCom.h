#ifndef RADIOCOM_H
#define RADIOCOM_H

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

class RadioCom {
private:
    RF24 radio;
    byte address[6];
    bool isTransmitter;

public:
    // Constructor: CE, CSN, dan alamat komunikasi
    RadioCom(uint8_t cePin, uint8_t csnPin, const char* addr, bool transmitter)
        : radio(cePin, csnPin), isTransmitter(transmitter) {
        memcpy(address, addr, 6);
    }

    // Inisialisasi modul radio
    void begin() {
        radio.begin();
        radio.setPALevel(RF24_PA_LOW); // bisa LOW, MEDIUM, HIGH, MAX
        radio.setDataRate(RF24_250KBPS);
        radio.setRetries(3, 5); // delay, count
        radio.enableAckPayload(); // aktifkan fitur ACK payload
        radio.setAutoAck(true);   // aktifkan auto-ack
        radio.setChannel(100);    // bebas pilih (0–125)

        if (isTransmitter) {
            radio.openWritingPipe(address);
            radio.stopListening();
        } else {
            radio.openReadingPipe(0, address);
            radio.startListening();
        }
    }

    // Kirim data (dengan ACK)
    bool send(const void* data, uint8_t len) {
        if (!isTransmitter) return false;
        bool ok = radio.write(data, len);
        if (ok) {
            Serial.println(F("[Radio] Data terkirim dan ACK diterima."));
        } else {
            Serial.println(F("[Radio] Gagal kirim data."));
        }
        return ok;
    }

    // Cek dan baca data masuk
    bool receive(void* buffer, uint8_t len) {
        if (isTransmitter) return false;
        if (radio.available()) {
            radio.read(buffer, len);
            return true;
        }
        return false;
    }

    // Kirim payload balasan dalam ACK (jika sebagai receiver)
    void sendAckPayload(const void* payload, uint8_t len) {
        if (!isTransmitter) {
            radio.writeAckPayload(1, payload, len); // pipe 1 default
        }
    }
};

#endif

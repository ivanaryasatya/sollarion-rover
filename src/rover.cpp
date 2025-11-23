#include <Arduino.h>

// custom library
#include "FIrebaseCom.h"
#include "MotorDriver.h"
#include "RadioDataPacker.h"
#include "RadioCom.h"
#include "TempSensor.h"

// pins ==============================================
const byte tempSensorPin = A0;
const byte RF24_ce_pin   = 7;
const byte RF24_csn_pin  = 8;

const byte driver_right_in1_pin = 22;
const byte driver_right_in2_pin = 23;
const byte driver_right_ena_pin = 2;
const byte driver_left_in1_pin  = 24;
const byte driver_left_in2_pin  = 25;
const byte driver_left_ena_pin  = 3;

// pins end ===========================================
char radioDataTransmit[32] = "";
char radioDataReceive[32] = "";

MotorDriver motor(driver_left_in1_pin, driver_left_in2_pin, driver_left_ena_pin,
                  driver_right_in1_pin, driver_right_in2_pin, driver_right_ena_pin);

NRF24Radio radio(RF24_ce_pin, RF24_csn_pin, "ROVct"); // CE, CSN, 5 char address

// Unpacked unpacked;
DHTSensor tempSensor(tempSensorPin, DHT11);

void setup() {
  Serial.begin(9600);
  motor.begin();
  tempSensor.begin();
  tempSensor.setInterval(2000);
  radio.begin(true); // mulai radio
}

void loop() {
  tempSensor.update();
  motor.stopAll();

  radio.receive(radioDataReceive, sizeof(radioDataReceive));
  RadioDataPacker packer;

  for (byte radioDataList = 0; ; radioDataList++) {
    Unpacked out = packer.unpack(radioDataReceive, radioDataList);

    if (out.name == "") break;

    Serial.print(out.name);
    Serial.print(" = ");
    Serial.println(out.value);

    float envTemp = tempSensor.getTemperature();
    float envHum = tempSensor.getHumidity();
    float envHeatIndex = tempSensor.getHeatIndex();
    Serial.print("Temp: "); Serial.print(envTemp); Serial.print(" C\t");
    Serial.print("Humidity: "); Serial.print(envHum); Serial.print(" %\t");
    Serial.print("Heat Index: "); Serial.println(envHeatIndex);
  }
}

#include <Arduino.h>

// custom library
#include "MotorDriver.h"
#include "RadioDataPacker.h"
#include "RadioCom.h"
#include "TempSensor.h"

// pins ==============================================
const byte
tempSensorPin = 255,
RF24_csn_pin = 255,
RF24_ce_pin  = 255,
driver_right_in1_pin = 255,
driver_right_in2_pin = 255,
driver_right_ena_pin = 255,
driver_left_in1_pin = 255,
driver_left_in2_pin = 255,
driver_left_ena_pin = 255,
servo1_pin = 255,
servo2_pin = 255
;
// pins end===========================================

char radioDataTransmit[32] = "";
char radioDataReceive[32] = "";


MotorDriver motor(22, 23, 2,   // kiri (IN1, IN2, ENA)
                  24, 25, 3);  // kanan(IN3, IN4, ENB)

NRF24Radio radio;
RadioDataPacker dataPacker;
Unpacked unpacked;
DHTSensor tempSensor(tempSensorPin, DHT11);

void setup() {
  Serial.begin(9600);
  motor.begin();
  tempSensor.begin();
  tempSensor.setInterval(2000);
}

void loop() {
  tempSensor.update();
  motor.stopAll();
  radio.receive(radioDataReceive, sizeof(radioDataReceive));
  RadioDataPacker packer;

  for (byte radioDataList = 0; ; radioDataList++) {
    Unpacked out = packer.unpack(radioDataReceive, radioDataList);

    if (out.name == "x") {

    }

    if (out.name == "" && out.value == 0) {
      break;
    }

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



  // motor.forward(200, 200);

  // delay(2000);

  // // belok kanan sambil maju
  // // steer = 150 → roda kanan dikurangi 150
  // motor.driveForwardWithSteer(200, 150);

  // delay(2000);

  // motor.stopAll();
}
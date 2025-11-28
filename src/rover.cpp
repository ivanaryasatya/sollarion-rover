#include <Arduino.h>

// custom library
#include "MotorDriver.h"
#include "RadioDataPacker.h"
#include "RadioCom.h"
#include "TempSensor.h"
#include "Mega2560Pin.h"
#include <powerMonitor.h>

// pins ==============================================

const byte tempSensorPin = NULL;

const byte RF24_ce_pin   = D7;
const byte RF24_csn_pin  = D8;

const byte currentSensorPin = NULL;
const byte voltageSensorPin = NULL;

// motor driver pins

// RIGHT
const byte driver_right_ena_pin = PWM2;
const byte driver_right_in1_pin = D22;
const byte driver_right_in2_pin = D23;
const byte driver_right_in3_pin = D24;
const byte driver_right_in4_pin = D25;
const byte driver_right_enb_pin = PWM3;

// LEFT
const byte driver_left_ena_pin = PWM4;
const byte driver_left_in1_pin = D26;
const byte driver_left_in2_pin = D27;
const byte driver_left_in3_pin = D28;
const byte driver_left_in4_pin = D29;
const byte driver_left_enb_pin = PWM5;

// radio communication  ==============================

const char *radioAddress = "ROVct";
char radioDataTransmit[32] = "";
char radioDataReceive[32] = "";
NRF24Radio radio(RF24_ce_pin, RF24_csn_pin, radioAddress); // CE, CSN, 5 char address

// motor driver=======================================

MotorDriver motor(
  driver_left_in1_pin,
  driver_left_in2_pin,
  driver_left_ena_pin,
  driver_right_in1_pin,
  driver_right_in2_pin,
  driver_right_ena_pin
);

// temperature & humidity sensor =====================

DHTSensor tempSensor(tempSensorPin, DHT11);

// power monitor ===================================

float currentSensorSensitivity = 0.185; // Contoh: ACS712 5A → 0.185 V/A
float voltageSensorDividerRatio = 5.0; // Contoh voltage divider: R1=30k, R2=7.5k → rasio = (R1+R2)/R2 = 5
PowerMonitor power(currentSensorPin, voltageSensorPin, currentSensorSensitivity, voltageSensorDividerRatio);

// ====================================================

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

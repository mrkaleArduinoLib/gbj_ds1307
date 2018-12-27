/*
  NAME:
  Current configuration parameters of DS1307 chip using gbjDS1307 library
  except datetime.

  DESCRIPTION:
  The sketch displays current configuration parameters of the chip as they are
  backed up by battery.
  - Connect modul's pins to microcontroller's I2C bus as described in README.md
    for used platform accordingly.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#define SKETCH "GBJ_DS1307_IDENTIFY 1.0.0"

#include "gbj_ds1307.h"


// Software configuration
gbj_ds1307 Device = gbj_ds1307();
// gbj_ds1307 Device = gbj_ds1307(gbj_ds1307::CLOCK_100KHZ, D2, D1);
// gbj_ds1307 Device = gbj_ds1307(gbj_ds1307::CLOCK_400KHZ);


void errorHandler(String location)
{
  if (Device.isSuccess()) return;
  Serial.print(location);
  Serial.print(" - Error: ");
  Serial.print(Device.getLastResult());
  Serial.print(" - ");
  switch (Device.getLastResult())
  {
    // General
    case gbj_ds1307::ERROR_ADDRESS:
      Serial.println("ERROR_ADDRESS");
      break;

    case gbj_ds1307::ERROR_PINS:
      Serial.println("ERROR_PINS");
      break;

    case gbj_ds1307::ERROR_RCV_DATA:
      Serial.println("ERROR_RCV_DATA");
      break;

    // Arduino, Esspressif specific
#if defined(__AVR__) || defined(ESP8266) || defined(ESP32)
    case gbj_ds1307::ERROR_BUFFER:
      Serial.println("ERROR_BUFFER");
      break;

    case gbj_ds1307::ERROR_NACK_DATA:
      Serial.println("ERROR_NACK_DATA");
      break;

    case gbj_ds1307::ERROR_NACK_OTHER:
      Serial.println("ERROR_NACK_OTHER");
      break;

    // Particle specific
#elif defined(PARTICLE)
    case gbj_ds1307::ERROR_BUSY:
      Serial.println("ERROR_BUSY");
      break;

    case gbj_ds1307::ERROR_END:
      Serial.println("ERROR_END");
      break;

    case gbj_ds1307::ERROR_TRANSFER:
      Serial.println("ERROR_TRANSFER");
      break;

    case gbj_ds1307::ERROR_TIMEOUT:
      Serial.println("ERROR_TIMEOUT");
      break;
#endif

    // Device specific
    default:
      Serial.println("Uknown error");
      break;
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.println(SKETCH);
  Serial.println("Libraries:");
  Serial.println(gbj_apphelpers::VERSION);
  Serial.println(gbj_twowire::VERSION);
  Serial.println(gbj_memory::VERSION);
  Serial.println(gbj_ds1307::VERSION);
  Serial.println("---");

  // Initialize
  if (Device.begin())
  {
    errorHandler("Begin");
    return;
  }
  // Configuration register
  Serial.print("Control register: 0x");
  Serial.print((Device.getControlReg() >> 4) ? "" : "0");
  Serial.print(Device.getControlReg(), HEX);
  if (Device.getPowerUp())
  {
    Serial.print(" (POWER-UP)");
  }
  Serial.println();
  // Configuration parameters
  Serial.print("SQW rate: ");
  switch (Device.getRate())
  {
    case gbj_ds1307::SQW_RATE_1HZ:
      Serial.print("1");
      break;
    case gbj_ds1307::SQW_RATE_4KHZ:
      Serial.print("4096");
      break;
    case gbj_ds1307::SQW_RATE_8KHZ:
      Serial.print("8192");
      break;
    case gbj_ds1307::SQW_RATE_32KHZ:
      Serial.print("32768");
      break;
  }
  Serial.println(" Hz");
  Serial.print("SQW stop output: ");
  Serial.println(Device.getSqwLevel() ? "HIGH" : "LOW");
  Serial.print("SQW state: ");
  Serial.println(Device.getSqwEnabled() ? "Running" : "Halted");
  Serial.println("---");
  // Clock parameters
  Serial.print("Clock mode: ");
  Serial.print(Device.getClockMode12H() ? "12" : "24");
  Serial.println(" hours");
  Serial.print("Clock state: ");
  Serial.println(Device.getClockEnabled() ? "Running" : "Halted");
  Serial.println("---");
  // Memory parameters
  Serial.print("RAM size: ");
  Serial.print(Device.getCapacityByte());
  Serial.println(" bytes");
  Serial.print("RAM range: 0x");
  Serial.print((Device.getPositionReal(0) >> 4) ? "" : "0");
  Serial.print(Device.getPositionReal(0), HEX);
  Serial.print(" ~ 0x");
  Serial.print((Device.getPositionReal(Device.getCapacityByte()) >> 4) ? "" : "0");
  Serial.println(Device.getPositionReal(Device.getCapacityByte() - 1), HEX);
  Serial.println("---");
  Serial.println("END");
}


void loop() {}

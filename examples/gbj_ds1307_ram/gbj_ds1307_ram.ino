/*
  NAME:
  Usage of battery backed up random access memory of DS1307 chip using
  gbjDS1307 library.

  DESCRIPTION:
  The sketch writes a byte, integer, and float number to the chip's RAM and
  reads it after for checking.
  - Connect modul's pins to microcontroller's I2C bus as described in README.md
    for used platform accordingly.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#define SKETCH "GBJ_DS1307_RAM 1.0.0"

#include "gbj_ds1307.h"


// Software configuration
gbj_ds1307 Device = gbj_ds1307();
// gbj_ds1307 Device = gbj_ds1307(D2, D1);

const unsigned int POSITION = 0;
byte valueByte = 0xAA;
int valueInt = 0xAA55;
float valueFloat = 123.45;


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

  // Write and read byte
  Serial.println("Stored byte: 0x" + String(valueByte, HEX));
  if (Device.store(POSITION, valueByte))
  {
    errorHandler("Store byte");
    return;
  }
  valueByte = 0;
  if (Device.retrieve(POSITION, valueByte))
  {
    errorHandler("Retrieved byte");
    return;
  }
  Serial.println("Retrieved byte: 0x" + String(valueByte, HEX));
  Serial.println("---");

  // Write and read integer
  Serial.println("Stored integer: 0x" + String(valueInt, HEX));
  if (Device.store(POSITION, valueInt))
  {
    errorHandler("Store integer");
    return;
  }
  valueInt = 0;
  if (Device.retrieve(POSITION, valueInt))
  {
    errorHandler("Retrieved integer");
    return;
  }
  Serial.println("Retrieved integer: 0x" + String(valueInt, HEX));
  Serial.println("---");

  // Write and read float
  Serial.println("Stored float: " + String(valueFloat));
  if (Device.store(POSITION, valueFloat))
  {
    errorHandler("Store float");
    return;
  }
  valueFloat = 0.0;
  if (Device.retrieve(POSITION, valueFloat))
  {
    errorHandler("Retrieved float");
    return;
  }
  Serial.println("Retrieved float: " + String(valueFloat));
  Serial.println("---");

  // Read recent position
  valueByte = 0xFF;
  if (Device.retrieveCurrent(valueByte))
  {
    errorHandler("Retrieved current");
    return;
  }
  Serial.println("Retrieved current: 0x" + String(valueByte, HEX));
  Serial.println("---");
  Serial.println("END");
}


void loop() {}

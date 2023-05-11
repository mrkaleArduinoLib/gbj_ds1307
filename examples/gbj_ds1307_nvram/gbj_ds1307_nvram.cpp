/*
  NAME:
  Usage of battery backed up non-volatile random access memory of DS1307 chip
  using gbjDS1307 library.

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
#include "gbj_ds1307.h"

gbj_ds1307 device = gbj_ds1307();
// gbj_ds1307 device = gbj_ds1307(device.CLOCK_400KHZ);
// gbj_ds1307 device = gbj_ds1307(device.CLOCK_100KHZ, D2, D1);

const unsigned int POSITION = 0;
byte valueByte = 0xA5;
int valueInt = 0xAA55;
float valueFloat = 123.45;

void errorHandler(String location)
{
  Serial.println(device.getLastErrorTxt(location));
  Serial.println("---");
  return;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("---");

  // Initialize
  if (device.isError(device.begin()))
  {
    errorHandler("Begin");
    return;
  }

  // Write and read byte
  Serial.println("Stored byte: 0x" + String(valueByte, HEX));
  if (device.isError(device.store(POSITION, valueByte)))
  {
    errorHandler("Store byte");
    return;
  }
  valueByte = 0;
  if (device.isError(device.retrieve(POSITION, valueByte)))
  {
    errorHandler("Retrieved byte");
    return;
  }
  Serial.println("Retrieved byte: 0x" + String(valueByte, HEX));
  Serial.println("---");

  // Write and read integer
  Serial.println("Stored integer: 0x" + String(valueInt, HEX));
  if (device.isError(device.store(POSITION, valueInt)))
  {
    errorHandler("Store integer");
    return;
  }
  valueInt = 0;
  if (device.isError(device.retrieve(POSITION, valueInt)))
  {
    errorHandler("Retrieved integer");
    return;
  }
  Serial.println("Retrieved integer: 0x" + String(valueInt, HEX));
  Serial.println("---");

  // Write and read float
  Serial.println("Stored float: " + String(valueFloat));
  if (device.isError(device.store(POSITION, valueFloat)))
  {
    errorHandler("Store float");
    return;
  }
  valueFloat = 0.0;
  if (device.isError(device.retrieve(POSITION, valueFloat)))
  {
    errorHandler("Retrieved float");
    return;
  }
  Serial.println("Retrieved float: " + String(valueFloat));
  Serial.println("---");

  // Read recent position
  valueByte = 0xFF;
  if (device.isError(device.retrieveCurrent(valueByte)))
  {
    errorHandler("Retrieved current");
    return;
  }
  Serial.println("Retrieved current: 0x" + String(valueByte, HEX));
  Serial.println("---");
}

void loop() {}

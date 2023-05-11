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
#include "gbj_ds1307.h"

gbj_ds1307 device = gbj_ds1307();
// gbj_ds1307 device = gbj_ds1307(device.CLOCK_400KHZ);
// gbj_ds1307 device = gbj_ds1307(device.CLOCK_100KHZ, D2, D1);

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
  // Configuration register
  Serial.print("Control register: 0x");
  Serial.print((device.getConfiguration() >> 4) ? "" : "0");
  Serial.print(device.getConfiguration(), HEX);
  if (device.getPowerUp())
  {
    Serial.print(" (POWER-UP)");
  }
  Serial.println();
  // Configuration parameters
  Serial.print("SQW rate: ");
  switch (device.getSqwRate())
  {
    case device.SQW_RATE_1HZ:
      Serial.print("1");
      break;
    case device.SQW_RATE_4KHZ:
      Serial.print("4096");
      break;
    case device.SQW_RATE_8KHZ:
      Serial.print("8192");
      break;
    case device.SQW_RATE_32KHZ:
      Serial.print("32768");
      break;
  }
  Serial.println(" Hz");
  Serial.print("SQW stop output: ");
  Serial.println(device.getSqwLevel() ? "HIGH" : "LOW");
  Serial.print("SQW state: ");
  Serial.println(device.getSqwEnabled() ? "Running" : "Halted");
  Serial.println("---");
  // Clock parameters
  Serial.print("Clock mode: ");
  Serial.print(device.getClockMode12H() ? "12" : "24");
  Serial.println(" hours");
  Serial.print("Clock state: ");
  Serial.println(device.getClockEnabled() ? "Running" : "Halted");
  Serial.println("---");
  // Memory parameters
  Serial.print("RAM size: ");
  Serial.print(device.getCapacityByte());
  Serial.println(" bytes");
  Serial.print("RAM range: 0x");
  Serial.print((device.getPositionReal(0) >> 4) ? "" : "0");
  Serial.print(device.getPositionReal(0), HEX);
  Serial.print(" ~ 0x");
  Serial.print((device.getPositionReal(device.getCapacityByte()) >> 4) ? ""
                                                                       : "0");
  Serial.println(device.getPositionReal(device.getCapacityByte() - 1), HEX);
  Serial.println("---");
}

void loop() {}

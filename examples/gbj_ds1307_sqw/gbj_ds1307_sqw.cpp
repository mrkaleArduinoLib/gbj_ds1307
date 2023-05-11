/*
  NAME:
  Generating square wave signal by DS1307 chip using gbjDS1307 library.

  DESCRIPTION:
  The sketch sets generating the square wave signal of the particular frequency.
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
  // SQW Generating
  if (device.isError(device.startSqw(device.SQW_RATE_4KHZ)))
  {
    errorHandler("Start SQW");
    return;
  }
  // Frequency
  Serial.print("Frequency: ");
  switch (device.getSqwRate())
  {
    case device.SQW_RATE_1HZ:
      Serial.print("1");
      break;
    case device.SQW_RATE_4KHZ:
      Serial.print("4096");
      break;
    case device.SQW_RATE_8KHZ:
      Serial.print("8162");
      break;
    case device.SQW_RATE_32KHZ:
      Serial.print("32768");
      break;
  }
  Serial.println(" Hz");
  Serial.println("---");
}

void loop() {}

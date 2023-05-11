/*
  NAME:
  Setting arbitrary date and time to DS1307 chip using gbjDS1307 library.

  DESCRIPTION:
  The sketch writes to time keeping register and reads it back for displaying
  and checking.
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
gbj_ds1307::Datetime rtcDateTime;

void errorHandler(String location)
{
  Serial.println(device.getLastErrorTxt(location));
  Serial.println("---");
  return;
}

void setup()
{
  // Desired datetime
  rtcDateTime.year = 2023;
  rtcDateTime.month = 05;
  rtcDateTime.day = 11;
  rtcDateTime.weekday = 4;
  rtcDateTime.hour = 17;
  rtcDateTime.minute = 44;
  rtcDateTime.second = 33;
  rtcDateTime.mode12h = false;
  rtcDateTime.pm = true;
  //
  Serial.begin(9600);
  Serial.println("---");

  // Initialize
  if (device.isError(device.begin()))
  {
    errorHandler("Begin");
    return;
  }
  // Writing datetime
  if (device.isError(device.setDateTime(rtcDateTime)))
  {
    errorHandler("Datetime write");
    return;
  }
  // Reading datetime
  if (device.isError(device.getDateTime(rtcDateTime)))
  {
    errorHandler("Datetime read");
    return;
  }
  // Date
  Serial.print("Date: ");
  Serial.print((rtcDateTime.day < 10 ? "0" : "") + String(rtcDateTime.day) +
               ".");
  Serial.print((rtcDateTime.month < 10 ? "0" : "") + String(rtcDateTime.month) +
               ".");
  Serial.println(rtcDateTime.year);
  Serial.print("Week day: ");
  Serial.println(rtcDateTime.weekday);
  Serial.println("---");
  // Time
  Serial.print("Time: ");
  Serial.print((rtcDateTime.hour < 10 ? "0" : "") + String(rtcDateTime.hour) +
               ":");
  Serial.print((rtcDateTime.minute < 10 ? "0" : "") +
               String(rtcDateTime.minute) + ":");
  Serial.print((rtcDateTime.second < 10 ? "0" : "") +
               String(rtcDateTime.second));
  Serial.println(rtcDateTime.mode12h ? (rtcDateTime.pm ? " PM" : " AM") : "");
  Serial.print("Clock mode: ");
  Serial.print(rtcDateTime.mode12h ? "12" : "24");
  Serial.println(" hours");
  Serial.println("---");
}

void loop() {}

/*
  NAME:
  Reading real date and time from DS1307 chip using gbjDS1307 library.

  DESCRIPTION:
  The sketch reads time keeping register and display real date and time from
  the chip.
  - Connect modul's pins to microcontroller's I2C bus as described in README.md
    for used platform accordingly.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#define SKETCH "GBJ_DS1307_DATETIME_GET 1.0.0"

#include "gbj_ds1307.h"


// Software configuration
gbj_ds1307 Device = gbj_ds1307();
// gbj_ds1307 Device = gbj_ds1307(gbj_ds1307::CLOCK_100KHZ, D2, D1);
// gbj_ds1307 Device = gbj_ds1307(gbj_ds1307::CLOCK_400KHZ);
gbj_ds1307::Datetime rtcDateTime;


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
  // Reading datetime
  if (Device.getDateTime(rtcDateTime))
  {
    errorHandler("Datetime read");
    return;
  }
  // Date
  Serial.print("Date: ");
  Serial.print((rtcDateTime.day < 10 ? "0" : "") + String(rtcDateTime.day) + ".");
  Serial.print((rtcDateTime.month < 10 ? "0" : "") + String(rtcDateTime.month) + ".");
  Serial.println(rtcDateTime.year);
  Serial.print("Week day: ");
  Serial.println(rtcDateTime.weekday);
  Serial.println("---");
  // Time
  Serial.print("Time: ");
  Serial.print((rtcDateTime.hour < 10 ? "0" : "") + String(rtcDateTime.hour) + ":");
  Serial.print((rtcDateTime.minute < 10 ? "0" : "") + String(rtcDateTime.minute) + ":");
  Serial.print((rtcDateTime.second < 10 ? "0" : "") + String(rtcDateTime.second));
  Serial.println(rtcDateTime.mode12h ? (rtcDateTime.pm ? " PM" : " AM") : "");
  Serial.print("Clock mode: ");
  Serial.print(rtcDateTime.mode12h ? "12" : "24");
  Serial.println(" hours");
  Serial.println("---");
  Serial.println("END");
}


void loop() {}

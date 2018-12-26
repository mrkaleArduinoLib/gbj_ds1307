/*
  NAME:
  Setting compiler date and time to DS1307 chip and starting it.

  DESCRIPTION:
  The sketch writes current date and time taken from the compiler to time keeping
  registers and reads them back for displaying and checking.
  - The sketch starts the clock (internal oscillator) as well regardless whether
    the RTC is has been running or not.
  - Overwrite weekday number to the current one for your week days sequence.
  - Connect modul's pins to microcontroller's I2C bus as described in README.md
    for used platform accordingly.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#define SKETCH "GBJ_DS1307_STARTCLOCK 1.0.0"

#include "gbj_ds1307.h"

const byte weekday = 3;  // Change to current number of your counting sequence
const char* weekdays[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

// Software configuration
gbj_ds1307 Device = gbj_ds1307();
// gbj_ds1307 Device = gbj_ds1307(D2, D1);
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
  // Start clock
  if (Device.startClock(__DATE__, __TIME__, weekday))
  {
    errorHandler("Start clock");
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
  Serial.print(rtcDateTime.weekday);
  Serial.print(" ");
  Serial.println(weekdays[rtcDateTime.weekday - 1]);
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
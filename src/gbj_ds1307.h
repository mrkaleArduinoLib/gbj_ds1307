/*
  NAME:
  gbjDS1307

  DESCRIPTION:
  Library for the real time clock DS1307 communicating on two-wire (I2C) bus.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
  (related to original code) and MIT License (MIT) for added code.

  CREDIT:
  BCD calculation - JeeLabs http://news.jeelabs.org/code/

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/gbj_ds1307.git
*/
#ifndef GBJ_DS1307_H
#define GBJ_DS1307_H

#include "gbj_memory.h"


class gbj_ds1307 : public gbj_memory
{
public:
//------------------------------------------------------------------------------
// Public constants
//------------------------------------------------------------------------------
static const String VERSION;
enum Addresses
{
  ADDRESS = 0x68,
};
enum ConvertionRate
{
  SQW_RATE_1HZ = B00,  // 1 Hz
  SQW_RATE_4KHZ = B01,  // 4096 Hz
  SQW_RATE_8KHZ = B10,  // 8192 Hz
  SQW_RATE_32KHZ = B11,  // 32768 Hz
};  // RS1 and RS0 bits of control register for rate select

struct Datetime
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t weekday;
  bool mode12h;
  bool pm;
};


//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------
/*
  Constructor taken from parent class.
*/
gbj_ds1307(uint8_t pinSDA = 4, uint8_t pinSCL = 5) \
: gbj_memory(CLOCK_100KHZ, pinSDA, pinSCL) {};


/*
  Initialize two wire bus and device with parameters stored by constructor.

  DESCRIPTION:
  The method sanitizes and stores input parameters to the class instance object,
  which determines the operation modus of the device.

  PARAMETERS: none

  RETURN:
  Result code.
*/
uint8_t begin();


/*
  Read from time keeping registers of the chip.

  DESCRIPTION:
  The method reads datetime from the chip, process it and place it to the
  referenced external structure (datetime record).
  - The method expects 21th century, so that add 2000 to the read two-digit
  year number.

  PARAMETERS:
  dtRecord - Referenced structure variable for writing read date and time.
             - Data type: Datetime
             - Default value: none
             - Limited range: address space

  RETURN:
  Result code.
*/
uint8_t getDateTime(Datetime &dtRecord);


/*
  Write to time keeping registers of the chip.

  DESCRIPTION:
  The method sanitizes datetime taken from referenced external structure
  (datetime record) and writes it to the chip.
  - The method strips century from the year and write just two-digit year number.
  - The method writes cached value to the control register of the chip as well,
    so that the chip can be set, start and configured at once.

  PARAMETERS:
  dtRecord - Referenced structure variable for desired date and time.
             - Data type: Datetime
             - Default value: none
             - Limited range: address space

  RETURN:
  Result code.
*/
uint8_t setDateTime(const Datetime &dtRecord);


//------------------------------------------------------------------------------
// Public setters - they usually return result code.
//------------------------------------------------------------------------------
/*
  Write control register value to the device.

  DESCRIPTION:
  The method sends prepared control register value to the device's control
  register.

  PARAMETERS: None

  RETURN:
  Result code.
*/
uint8_t setConfiguration();


// Preparation of timekeeping registers
inline void configClockEnable() { _rtcRecord.second &= ~(1 << CONFIG_CH); };
inline void configClockDisable() { _rtcRecord.second |= (1 << CONFIG_CH); };
// Preparation of control register value
inline void configSqwLevelHigh() { _rtcRecord.control |= (1 << CONFIG_OUT); };
inline void configSqwLevelLow() { _rtcRecord.control &= ~(1 << CONFIG_OUT); };
inline void configSqwEnable() { _rtcRecord.control |= (1 << CONFIG_SQWE); };
inline void configSqwDisable() { _rtcRecord.control &= ~(1 << CONFIG_SQWE); };


/*
  Update rate select bits in control register value.

  DESCRIPTION:
  The method updates RS1 and RS0 bits in control register value demanding
  particular rate.

  PARAMETERS:
  rate - Value of pair of RS1 and RS0 bits. It fallbacks to least significant
         2 bits.
         - Data type: non-negative integer
         - Default value: none
         - Limited range: SQW_RATE_1HZ ~ SQW_RATE_32KHZ

  RETURN: none
*/
void configRate(uint8_t rate);


//------------------------------------------------------------------------------
// Public getters
//------------------------------------------------------------------------------
inline uint8_t getControlReg() { return _rtcRecord.control; };
inline uint8_t getRate() { return (_rtcRecord.control >> CONFIG_RS0) & B11; };
inline uint8_t getSqwLevel() { return (_rtcRecord.control >> CONFIG_OUT) & B1; };
inline bool getPowerUp() { return _rtcRecord.control == PARAM_POWERUP; };
inline bool getSqwEnabled() { return ((_rtcRecord.control >> CONFIG_SQWE) & B1) == 1; };
inline bool getClockEnabled() { return ((_rtcRecord.second >> CONFIG_CH) & B1) == 0; };
inline bool getClockMode12H() { return ((_rtcRecord.hour >> CONFIG_12H) & B1) == 1; };


/*
  Read control register value from the device.

  DESCRIPTION:
  The method reads control register and its value stores in the instance
  object, so that it caches it.

  PARAMETERS: none

  RETURN:
  Result code.
*/
uint8_t getConfiguration();


private:
//------------------------------------------------------------------------------
// Private constants
//------------------------------------------------------------------------------
enum Commands
{
  CMD_REG_SECOND = 0x00,
  CMD_REG_MINUTE = 0x01,
  CMD_REG_HOUR = 0x02,
  CMD_REG_WEEKDAY = 0x03,
  CMD_REG_DAY = 0x04,
  CMD_REG_MONTH = 0x05,
  CMD_REG_YEAR = 0x06,
  CMD_REG_CONTROL = 0x07,  // Control register
  CMD_REG_RAM_MIN = 0x08,  // First memory position
  CMD_REG_RAM_MAX = 0x3F,  // Last memory position
};
enum ConfigBits
{
  CONFIG_OUT = 7,  // Output level when SQW is disabled (powerup 0)
  CONFIG_SQWE = 4,  // Square wave enabled (powerup 0)
  CONFIG_RS1 = 1,  // Rate select MSb (powerup 1)
  CONFIG_RS0 = 0,  // Rate select LSb (powerup 1)
};  // Bits order in control register
enum HourBits
{
  CONFIG_12H = 6,  // 12/24 mode
  CONFIG_PM = 5,  // AM/PM or 0 ~ 2
};  // Bits order in hours register
enum SecondBits
{
  CONFIG_CH = 7,  // Clock halt bit (powerup 1)
};  // Bits order in seconds register
enum Params
{
  PARAM_POWERUP = 0x03,  // Control register byte after power-up reset
};


//------------------------------------------------------------------------------
// Private attributes
//------------------------------------------------------------------------------
struct
{
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t weekday;
  uint8_t day;
  uint8_t month;
  uint8_t year;
  uint8_t control;
} _rtcRecord;


//------------------------------------------------------------------------------
// Private methods - they return result code if not stated else
//------------------------------------------------------------------------------
uint8_t readRtcRecord();
inline uint8_t bcd2bin (uint8_t bcdValue) { return bcdValue - 6 * (bcdValue >> 4); }
inline uint8_t bin2bcd (uint8_t binValue) { return binValue + 6 * (binValue / 10); }

};

#endif

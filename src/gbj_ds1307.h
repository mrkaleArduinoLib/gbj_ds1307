/*
  NAME:
  gbjDS1307

  DESCRIPTION:
  Library for the real time clock DS1307 communicating on two-wire (I2C) bus.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

  CREDITS:
  BCD calculation - JeeLabs http://news.jeelabs.org/code/

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/gbj_ds1307.git
*/
#ifndef GBJ_DS1307_H
#define GBJ_DS1307_H

#include "gbj_apphelpers.h"
#include "gbj_memory.h"

class gbj_ds1307 : public gbj_memory
{
public:
  enum Addresses : uint8_t
  {
    ADDRESS = 0x68,
  };
  // RS1 and RS0 bits of control register for rate select
  enum SquareWaveFrequency : uint8_t
  {
    // 1 Hz
    SQW_RATE_1HZ = B00,
    // 4096 Hz
    SQW_RATE_4KHZ = B01,
    // 8192 Hz
    SQW_RATE_8KHZ = B10,
    // 32768 Hz
    SQW_RATE_32KHZ = B11,
  };
  // External datetime structure
  using Datetime = gbj_apphelpers::Datetime;

  gbj_ds1307(ClockSpeeds clockSpeed = ClockSpeeds::CLOCK_100KHZ,
             uint8_t pinSDA = 4,
             uint8_t pinSCL = 5)
    : gbj_memory(clockSpeed, pinSDA, pinSCL)
  {
  }

  /*
    Initialize two wire bus and device with parameters stored by constructor.

    DESCRIPTION:
    The method sanitizes and stores input parameters to the class instance
    object, which determines the operation modus of the device.

    PARAMETERS: none

    RETURN: Result code
  */
  inline ResultCodes begin()
  {
    if (isError(gbj_memory::begin(Commands::CMD_REG_RAM_MAX,
                                  Commands::CMD_REG_RAM_MAX -
                                    Commands::CMD_REG_RAM_MIN + 1,
                                  Commands::CMD_REG_RAM_MIN)))
    {
      return getLastResult();
    }
    setPositionInBytes();
    if (isError(registerAddress(static_cast<uint8_t>(Addresses::ADDRESS))))
    {
      return getLastResult();
    }
    return readRtcRecord();
  }

  /*
    Convert internal structure to datetime.

    DESCRIPTION:
    The method converts already read datetime from the chip and stored in
    instance internal structure to the referenced external structure (datetime
    record).
    - The method converts recently read datetime from the chip without repeating
      the reading from it. It is useful right after begin() method, which reads
      the chip's status in either case.
    - The method expects 21th century, so that adds 2000 to the read two-digit
    year number.

    PARAMETERS:
    dtRecord - Referenced structure variable for writing date and time.
      - Data type: Datetime
      - Default value: none
      - Limited range: address space

    RETURN: none
  */
  void convertDateTime(Datetime &dtRecord);

  /*
    Read from time keeping registers of the chip.

    DESCRIPTION:
    The method reads datetime from the chip and calls convertDateTime() method
    for obtaining the datetime to the referenced external structure (datetime
    record).

    PARAMETERS:
    dtRecord - Referenced structure variable for writing read date and time.
      - Data type: Datetime
      - Default value: none
      - Limited range: address space

    RETURN: Result code
  */
  inline ResultCodes getDateTime(Datetime &dtRecord)
  {
    if (isError(readRtcRecord()))
    {
      return getLastResult();
    }
    convertDateTime(dtRecord);
    return getLastResult();
  }

  /*
    Write to time keeping registers as well as configuration register of the
    chip.

    DESCRIPTION:
    The method sanitizes datetime taken from referenced external structure
    (datetime record) and writes it to the chip.
    - The method strips century from the year and writes just two-digit year
    number.
    - The method writes cached value to the control register of the chip as
    well, so that the chip can be set, started and configured at once.

    PARAMETERS:
    dtRecord - Referenced structure variable for desired date and time.
      - Data type: Datetime
      - Default value: none
      - Limited range: address space

    RETURN: Result code
  */
  ResultCodes setDateTime(const Datetime &dtRecord);

  /*
    Setup clock and start it.

    DESCRIPTION:
    The particular method sets the date and time to the RTC chip and starts its
    internal oscillator.
    - The method is overloaded, either for flashed strings or strings in SRAM
    pointers for date and time.
    - The method sets datetime regardless the RTC chip is running or not.
    - If no input parameters are used, just the CH bit is reset with retaining
    current second.

    PARAMETERS:
    strDate - Pointer to a system date formatted string.
      - Data type: char pointer
      - Default value: none
      - Limited range: address range

    strTime - Pointer to a system time formatted string.
      - Data type: char pointer
      - Default value: none
      - Limited range: address range

    weekday - Number of current day in a week. It is up to an application to set
    the starting day in the week. If weekdays are irrelevant, the default value
    may be used. The provided weekday fallbacks to valid range.
      - Data type: positive integer
      - Default value: 1
      - Limited range: 1 ~ 7

    mode12h - Flag about using 12 hours mode.
      - Data type: boolean
      - Default value: false (24 hours mode)
      - Limited range: true = 12 hours mode, false = 24 hours mode

    RETURN: Result code
  */
  inline ResultCodes startClock(const char *strDate,
                                const char *strTime,
                                uint8_t weekday = 1,
                                bool mode12h = false)
  {
    Datetime rtcDateTime;
    rtcDateTime.weekday = weekday;
    rtcDateTime.mode12h = mode12h;
    configClockEnable();
    gbj_apphelpers::parseDateTime(rtcDateTime, strDate, strTime);
    return setDateTime(rtcDateTime);
  }
  inline ResultCodes startClock(const __FlashStringHelper *strDate,
                                const __FlashStringHelper *strTime,
                                uint8_t weekday = 1,
                                bool mode12h = false)
  {
    Datetime rtcDateTime;
    rtcDateTime.weekday = weekday;
    rtcDateTime.mode12h = mode12h;
    configClockEnable();
    gbj_apphelpers::parseDateTime(rtcDateTime, strDate, strTime);
    return setDateTime(rtcDateTime);
  }
  inline ResultCodes startClock()
  {
    bool origBusStop = getBusStop();
    setBusRepeat();
    if (isError(readRtcRecord()))
    {
      return getLastResult();
    }
    configClockEnable();
    setBusStopFlag(origBusStop);
    return busSend(Commands::CMD_REG_SECOND, _rtcRecord.second);
  }

  /*
    Stop clock.

    DESCRIPTION:
    The method resets clock halt (CH) bit of the seconds time keeping register
    with retaining current second in it.

    PARAMETERS: none

    RETURN: Result code
  */
  inline ResultCodes stopClock()
  {
    bool origBusStop = getBusStop();
    setBusRepeat();
    if (isError(readRtcRecord()))
    {
      return getLastResult();
    }
    configClockDisable();
    setBusStopFlag(origBusStop);
    return busSend(Commands::CMD_REG_SECOND, _rtcRecord.second);
  }

  /*
    Start generating square wave signal.

    DESCRIPTION:
    The method enables generating square wave signal on SQW/OUT pin of the RTC
    chip at frequency determined by the input parameter and starts the clock.
    - The method sends new value of the control register or seconds time keeping
    register to the chip only if cached value differs from desired one in order
    to avoid useless communication on the two-wire bus.

    PARAMETERS:
    rate - Value of pair of RS1 and RS0 bits. It fallbacks to least significant
    2 bits.
      - Data type: SquareWaveFrequency
      - Default value: SQW_RATE_32KHZ
      - Limited range: SQW_RATE_1HZ ~ SQW_RATE_32KHZ

    RETURN: Result code
  */
  inline ResultCodes startSqw(
    SquareWaveFrequency rate = SquareWaveFrequency::SQW_RATE_32KHZ)
  {
    // Do nothing if everything is set
    if (getClockEnabled() && getSqwEnabled() && (getSqwRate() == rate))
    {
      return getLastResult();
    }
    // Set configuration register if needed
    if (!getSqwEnabled() || getSqwRate() != rate)
    {
      configSqwEnable();
      configSqwRate(rate);
      if (isError(setConfiguration()))
      {
        return getLastResult();
      }
    }
    // Start clock if needed
    if (!getClockEnabled())
    {
      if (isError(startClock()))
      {
        return getLastResult();
      }
    }
    return getLastResult();
  }

  // Setters

  /*
    Write control register value to the device.

    DESCRIPTION:
    The method sends prepared control register value to the device's control
    register.

    PARAMETERS: none

    RETURN: Result code
  */
  inline ResultCodes setConfiguration()
  {
    return busSend(Commands::CMD_REG_CONTROL, _rtcRecord.control);
  }

  // Preparation of timekeeping registers
  inline void configClockEnable()
  {
    _rtcRecord.second &= ~(1 << SecondBits::CONFIG_CH);
  }
  inline void configClockDisable()
  {
    _rtcRecord.second |= (1 << SecondBits::CONFIG_CH);
  }
  // Preparation of control register value
  inline void configSqwLevelHigh()
  {
    _rtcRecord.control |= (1 << ConfigBits::CONFIG_OUT);
  }
  inline void configSqwLevelLow()
  {
    _rtcRecord.control &= ~(1 << ConfigBits::CONFIG_OUT);
  }
  inline void configSqwEnable()
  {
    _rtcRecord.control |= (1 << ConfigBits::CONFIG_SQWE);
  }
  inline void configSqwDisable()
  {
    _rtcRecord.control &= ~(1 << ConfigBits::CONFIG_SQWE);
  }

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
  inline void configSqwRate(SquareWaveFrequency rate)
  {
    // Clear bits
    _rtcRecord.control &= ~(B11 << ConfigBits::CONFIG_RS0);
    // Set bits
    _rtcRecord.control |= ((rate & B11) << ConfigBits::CONFIG_RS0);
  }

  // Getters
  inline uint8_t getConfiguration() { return _rtcRecord.control; }
  inline SquareWaveFrequency getSqwRate()
  {
    return static_cast<SquareWaveFrequency>(
      (_rtcRecord.control >> ConfigBits::CONFIG_RS0) & B11);
  }
  inline uint8_t getSqwLevel()
  {
    return (_rtcRecord.control >> ConfigBits::CONFIG_OUT) & B1;
  }
  inline bool getPowerUp()
  {
    return _rtcRecord.control == Params::PARAM_POWERUP;
  }
  inline bool getSqwEnabled()
  {
    return ((_rtcRecord.control >> ConfigBits::CONFIG_SQWE) & B1) == 1;
  }
  inline bool getClockEnabled()
  {
    return ((_rtcRecord.second >> SecondBits::CONFIG_CH) & B1) == 0;
  }
  inline bool getClockMode12H()
  {
    return ((_rtcRecord.hour >> HourBits::CONFIG_12H) & B1) == 1;
  }

private:
  enum Commands : uint8_t
  {
    CMD_REG_SECOND = 0x00,
    CMD_REG_MINUTE = 0x01,
    CMD_REG_HOUR = 0x02,
    CMD_REG_WEEKDAY = 0x03,
    CMD_REG_DAY = 0x04,
    CMD_REG_MONTH = 0x05,
    CMD_REG_YEAR = 0x06,
    // Control register
    CMD_REG_CONTROL = 0x07,
    // First memory position
    CMD_REG_RAM_MIN = 0x08,
    // Last memory position
    CMD_REG_RAM_MAX = 0x3F,
  };
  // Bits order in control register
  enum ConfigBits : uint8_t
  {
    // Output level when SQW is disabled (powerup 0)
    CONFIG_OUT = 7,
    // Square wave enabled (powerup 0)
    CONFIG_SQWE = 4,
    // Rate select MSb (powerup 1)
    CONFIG_RS1 = 1,
    // Rate select LSb (powerup 1)
    CONFIG_RS0 = 0,
  };
  // Bits order in hours register
  enum HourBits
  {
    // 12/24 mode
    CONFIG_12H = 6,
    // AM/PM or 0 ~ 2
    CONFIG_PM = 5,
  };
  // Bits order in seconds register
  enum SecondBits
  {
    // Clock halt bit (powerup 1)
    CONFIG_CH = 7,
  };
  enum Params
  {
    // Control register byte after power-up reset
    PARAM_POWERUP = 0x03,
  };
  struct RtcRecord
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
  inline ResultCodes readRtcRecord()
  {
    bool origBusStop = getBusStop();
    setBusRepeat();
    if (isError(busSend(CMD_REG_SECOND)))
    {
      return getLastResult();
    }
    setBusStopFlag(origBusStop);
    return busReceive(reinterpret_cast<uint8_t *>(&_rtcRecord),
                      sizeof(_rtcRecord));
  }

  inline uint8_t bcd2bin(uint8_t bcdValue)
  {
    return bcdValue - 6 * (bcdValue >> 4);
  }
  inline uint8_t bin2bcd(uint8_t binValue)
  {
    return binValue + 6 * (binValue / 10);
  }
};

#endif

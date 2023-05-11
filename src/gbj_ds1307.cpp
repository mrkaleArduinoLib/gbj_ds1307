#include "gbj_ds1307.h"

void gbj_ds1307::convertDateTime(Datetime &dtRecord)
{
  dtRecord.second = bcd2bin(_rtcRecord.second & ~(1 << SecondBits::CONFIG_CH));
  dtRecord.minute = bcd2bin(_rtcRecord.minute);
  dtRecord.hour = bcd2bin(_rtcRecord.hour);
  dtRecord.mode12h =
    static_cast<bool>(_rtcRecord.hour & (1 << HourBits::CONFIG_12H));
  dtRecord.pm = static_cast<bool>(_rtcRecord.hour & (1 << HourBits::CONFIG_PM));
  if (dtRecord.mode12h)
  {
    _rtcRecord.hour &= ~(1 << HourBits::CONFIG_12H);
    _rtcRecord.hour &= ~(1 << HourBits::CONFIG_PM);
    dtRecord.hour = bcd2bin(_rtcRecord.hour);
  }
  dtRecord.day = bcd2bin(_rtcRecord.day);
  dtRecord.month = bcd2bin(_rtcRecord.month);
  dtRecord.year = bcd2bin(_rtcRecord.year) + 2000;
  dtRecord.weekday = _rtcRecord.weekday;
}

gbj_ds1307::ResultCodes gbj_ds1307::setDateTime(const Datetime &dtRecord)
{
  Commands command = Commands::CMD_REG_SECOND;
  // Retain original clock halt bit
  _rtcRecord.second &= 1 << SecondBits::CONFIG_CH;
  _rtcRecord.second |= bin2bcd(dtRecord.second % 60);
  _rtcRecord.minute = bin2bcd(dtRecord.minute % 60);
  _rtcRecord.hour = 0;
  if (dtRecord.mode12h)
  {
    // Set 12 clock mode
    _rtcRecord.hour |= 1 << HourBits::CONFIG_12H;
    if (dtRecord.pm)
    {
      // Set PM flag
      _rtcRecord.hour |= (1 << HourBits::CONFIG_PM);
    }
    _rtcRecord.hour |=
      bin2bcd(dtRecord.hour % 12 == 0 ? 12 : dtRecord.hour % 12);
  }
  else
  {
    _rtcRecord.hour |= bin2bcd(dtRecord.hour % 24);
  }
  _rtcRecord.day = bin2bcd(constrain(dtRecord.day, 1, 31));
  _rtcRecord.month = bin2bcd(constrain(dtRecord.month, 1, 12));
  _rtcRecord.year = bin2bcd(dtRecord.year % 100);
  _rtcRecord.weekday = constrain(dtRecord.weekday, 1, 7);
  return (busSendStreamPrefixed(reinterpret_cast<uint8_t *>(&_rtcRecord),
                                sizeof(_rtcRecord),
                                false,
                                reinterpret_cast<uint8_t *>(&command),
                                sizeof(command),
                                false,
                                true));
}

#include "gbj_ds1307.h"

void gbj_ds1307::convertDateTime(Datetime &dtRecord)
{
  dtRecord.second = bcd2bin(rtcRecord_.second & ~(1 << SecondBits::CONFIG_CH));
  dtRecord.minute = bcd2bin(rtcRecord_.minute);
  dtRecord.hour = bcd2bin(rtcRecord_.hour);
  dtRecord.mode12h =
    static_cast<bool>(rtcRecord_.hour & (1 << HourBits::CONFIG_12H));
  dtRecord.pm = static_cast<bool>(rtcRecord_.hour & (1 << HourBits::CONFIG_PM));
  if (dtRecord.mode12h)
  {
    rtcRecord_.hour &= ~(1 << HourBits::CONFIG_12H);
    rtcRecord_.hour &= ~(1 << HourBits::CONFIG_PM);
    dtRecord.hour = bcd2bin(rtcRecord_.hour);
  }
  dtRecord.day = bcd2bin(rtcRecord_.day);
  dtRecord.month = bcd2bin(rtcRecord_.month);
  dtRecord.year = bcd2bin(rtcRecord_.year) + 2000;
  dtRecord.weekday = rtcRecord_.weekday;
}

gbj_ds1307::ResultCodes gbj_ds1307::setDateTime(const Datetime &dtRecord)
{
  Commands command = Commands::CMD_REG_SECOND;
  // Retain original clock halt bit
  rtcRecord_.second &= 1 << SecondBits::CONFIG_CH;
  rtcRecord_.second |= bin2bcd(dtRecord.second % 60);
  rtcRecord_.minute = bin2bcd(dtRecord.minute % 60);
  rtcRecord_.hour = 0;
  if (dtRecord.mode12h)
  {
    // Set 12 clock mode
    rtcRecord_.hour |= 1 << HourBits::CONFIG_12H;
    if (dtRecord.pm)
    {
      // Set PM flag
      rtcRecord_.hour |= (1 << HourBits::CONFIG_PM);
    }
    rtcRecord_.hour |=
      bin2bcd(dtRecord.hour % 12 == 0 ? 12 : dtRecord.hour % 12);
  }
  else
  {
    rtcRecord_.hour |= bin2bcd(dtRecord.hour % 24);
  }
  rtcRecord_.day = bin2bcd(constrain(dtRecord.day, 1, 31));
  rtcRecord_.month = bin2bcd(constrain(dtRecord.month, 1, 12));
  rtcRecord_.year = bin2bcd(dtRecord.year % 100);
  rtcRecord_.weekday = constrain(dtRecord.weekday, 1, 7);
  return (busSendStreamPrefixed(reinterpret_cast<uint8_t *>(&rtcRecord_),
                                sizeof(rtcRecord_),
                                false,
                                reinterpret_cast<uint8_t *>(&command),
                                sizeof(command),
                                false,
                                true));
}

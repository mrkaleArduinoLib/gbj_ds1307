#include "gbj_ds1307.h"
const String gbj_ds1307::VERSION = "GBJ_DS1307 1.0.0";


uint8_t gbj_ds1307::begin()
{
  if (gbj_memory::begin(CMD_REG_RAM_MAX, CMD_REG_RAM_MAX - CMD_REG_RAM_MIN + 1, \
    CMD_REG_RAM_MIN)) return getLastResult();
  setPositionInBytes();
  if (registerAddress(ADDRESS)) return getLastResult();
  if (readRtcRecord()) return getLastResult();
  return getLastResult();
}


uint8_t gbj_ds1307::readRtcRecord()
{
  bool origBusStop = getBusStop();
  setBusRpte();
  if (busSend(CMD_REG_SECOND)) return getLastResult();
  setBusStopFlag(origBusStop);
  if (busReceive((uint8_t *)&_rtcRecord, sizeof(_rtcRecord))) return getLastResult();
  return getLastResult();
}


uint8_t gbj_ds1307::getDateTime(Datetime &dtRecord)
{
  if (readRtcRecord()) return getLastResult();
  dtRecord.second = bcd2bin(_rtcRecord.second & ~(1 << CONFIG_CH));
  dtRecord.minute = bcd2bin(_rtcRecord.minute);
  dtRecord.hour = bcd2bin(_rtcRecord.hour);
  dtRecord.mode12h = (bool) (_rtcRecord.hour & (1 << CONFIG_12H));
  dtRecord.pm = (bool) (_rtcRecord.hour & (1 << CONFIG_PM));
  if (dtRecord.mode12h)
  {
    _rtcRecord.hour &= ~(1 << CONFIG_12H);
    _rtcRecord.hour &= ~(1 << CONFIG_PM);
    dtRecord.hour = bcd2bin(_rtcRecord.hour);
  }
  dtRecord.day = bcd2bin(_rtcRecord.day);
  dtRecord.month = bcd2bin(_rtcRecord.month);
  dtRecord.year = bcd2bin(_rtcRecord.year) + 2000;
  dtRecord.weekday = _rtcRecord.weekday;
  return getLastResult();
}


uint8_t gbj_ds1307::setDateTime(const Datetime &dtRecord)
{
  uint8_t command = CMD_REG_SECOND;
  //
  _rtcRecord.second &= 1 << CONFIG_CH;  // Retain original clock halt bit
  _rtcRecord.second |= bin2bcd(dtRecord.second % 60);
  //
  _rtcRecord.minute = bin2bcd(dtRecord.minute % 60);
  //
  _rtcRecord.hour = 0;
  if (dtRecord.mode12h)
  {
    _rtcRecord.hour |= 1 << CONFIG_12H;  // Set 12 clock mode
    if (dtRecord.pm) _rtcRecord.hour |= (1 << CONFIG_PM);  // Set PM flag
    _rtcRecord.hour |= bin2bcd(dtRecord.hour % 12 == 0 ? 12 : dtRecord.hour % 12);
  }
  else
  {
    _rtcRecord.hour |= bin2bcd(dtRecord.hour % 24);
  }
  //
  _rtcRecord.day = bin2bcd(constrain(dtRecord.day, 1, 31));
  _rtcRecord.month = bin2bcd(constrain(dtRecord.month, 1, 12));
  _rtcRecord.year = bin2bcd(dtRecord.year % 100);
  _rtcRecord.weekday = constrain(dtRecord.weekday, 1, 7);
  //
  if (busSendStreamPrefixed((uint8_t *)&_rtcRecord, sizeof(_rtcRecord), false, \
    (uint8_t *)&command, sizeof(command), false, true)) return getLastResult();
  return getLastResult();
}


uint8_t gbj_ds1307::startClock(const char* strDate, const char* strTime, \
  uint8_t weekday, bool mode12h)
{
  Datetime rtcDateTime;
  rtcDateTime.weekday = weekday;
  rtcDateTime.mode12h = mode12h;
  configClockEnable();
  gbj_apphelpers::parseDateTime(rtcDateTime, strDate, strTime);
  return setDateTime(rtcDateTime);
}


uint8_t gbj_ds1307::startClock(const __FlashStringHelper* strDate, \
  const __FlashStringHelper* strTime, uint8_t weekday, bool mode12h)
{
  Datetime rtcDateTime;
  rtcDateTime.weekday = weekday;
  rtcDateTime.mode12h = mode12h;
  configClockEnable();
  gbj_apphelpers::parseDateTime(rtcDateTime, strDate, strTime);
  return setDateTime(rtcDateTime);
}


uint8_t gbj_ds1307::startClock()
{
  bool origBusStop = getBusStop();
  setBusRpte();
  if (readRtcRecord()) return getLastResult();
  configClockEnable();
  setBusStopFlag(origBusStop);
  if (busSend(CMD_REG_SECOND, _rtcRecord.second)) return getLastResult();
  return getLastResult();
}


uint8_t gbj_ds1307::stopClock()
{
  bool origBusStop = getBusStop();
  setBusRpte();
  if (readRtcRecord()) return getLastResult();
  configClockDisable();
  setBusStopFlag(origBusStop);
  if (busSend(CMD_REG_SECOND, _rtcRecord.second)) return getLastResult();
  return getLastResult();
}


uint8_t gbj_ds1307::startSqw(uint8_t rate)
{
  // Do nothing if everything is set
  if (getClockEnabled() && getSqwEnabled() && (getSqwRate() == rate)) return getLastResult();
  // Set configuration register if needed
  if (!getSqwEnabled() || getSqwRate() != rate)
  {
    configSqwEnable();
    configSqwRate(rate);
    if (setConfiguration()) return getLastResult();
  }
  // Start clock if needed
  if (!getClockEnabled())
  {
    if (startClock()) return getLastResult();
  }
  return getLastResult();
}


//-------------------------------------------------------------------------
// Setters
//-------------------------------------------------------------------------
uint8_t gbj_ds1307::setConfiguration()
{
  if (busSend(CMD_REG_CONTROL, _rtcRecord.control)) return getLastResult();
  return getLastResult();
}


void gbj_ds1307::configSqwRate(uint8_t rate)
{
  _rtcRecord.control &= ~(B11 << CONFIG_RS0);  // Clear bits
  _rtcRecord.control |= ((rate & B11) << CONFIG_RS0);  // Set bits
}

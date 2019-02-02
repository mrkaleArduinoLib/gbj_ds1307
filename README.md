<a id="library"></a>
# gbjDS1307
Library for the *Dallas Semiconductor* real time clock *DS1307* chip communicating on two-wire (I2C) bus.
- Sensor has fixed addresses `0x68`.
- The library utilizes external custom data type from the library *gbjAppHelpers* as a datetime structure in the form of alias in own body.
- The library does not provide any formatting and parsing functionality for datetime structures. Use the dedicated library *gbjAppHelpers* for those funcionalities.
- Library caches configuration register of the chip.


#### Particle hardware configuration
- Connect microcontroller's pin `D0` to sensor's pin **SDA** (Serial Data).
- Connect microcontroller's pin `D1` to sensor's pin **SCL** (Serial Clock).

#### Arduino UNO hardware configuration
- Connect microcontroller's pin `A4` to sensor's pin **SDA** (Serial Data).
- Connect microcontroller's pin `A5` to sensor's pin **SCL** (Serial Clock).

#### Espressif - ESP8266, ESP32 default hardware configuration
- Connect microcontroller's pin `D2` to sensor's pin **SDA** (Serial Data).
- Connect microcontroller's pin `D1` to sensor's pin **SCL** (Serial Clock).


<a id="dependency"></a>
## Dependency

#### Particle platform
- **Particle.h**: Includes alternative (C++) data type definitions.

#### Arduino platform
- **Arduino.h**: Main include file for the Arduino SDK version greater or equal to 100.
- **WProgram.h**: Main include file for the Arduino SDK version less than 100.
- **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.
- **TwoWire**: I2C system library loaded from the file *Wire.h*.

#### Custom Libraries
- **gbjMemory**: Custom library loaded from the file *gbj_memory.h* for a generic memory on two-wire (I2C) bus as a parent class for the library [gbjDS1307](#library), which inherits common memory processing functionality from this library for utilizing non-volatile, battery backed up memory of the RTC chip.
- **gbjAppHelpers**: Custom library loaded from the file *gbj_apphelpers.h* for a generic application logic. The library [gbjDS1307](#library) utilizes custom data type from it for datetime structure definition.


<a id="constants"></a>
## Constants
- **gbj\_ds1307::VERSION**: Name and semantic version of the library.
- **gbj\_ds1307::ADDRESS**: I2C address of the RTC chip.


<a id="SQW"></a>
#### Square wave frequencies
- **gbj\_ds1307::SQW\_RATE\_1HZ**: Square wave frequency 1 Hz.
- **gbj\_ds1307::SQW\_RATE\_4KHZ**: Square wave frequency 4096 Hz.
- **gbj\_ds1307::SQW\_RATE\_8KHZ**: Square wave frequency 8192 Hz.
- **gbj\_ds1307::SQW\_RATE\_32KHZ**: Square wave frequency 32768 Hz.


<a id="errors"></a>
#### Error codes
The library does not provide any own specific error codes. All result and error codes are inhereted for the parent library [gbjMemory](#dependency).


<a id="configuration"></a>
## Configuration
The configuration of the RTC chip is realized by the configuration register, which consists of several configuration bits determining its behavior and time keeping registers for seconds and hours determining running and form of time expression. The library stores (caches) the value of those registers in its instance object.

The chip configuration implemented in the library is based on updating cached configuration values in advanced by methods of the naming convention `configXXX` and finally sending that value to the chip and write all configuration bits to appropriate registers at once in order to reduce communication on the two-wire bus in contrast to sending configuration bits to the chip individually.

Because the RTC chip does not change the content of its configuration register and status bits of time keeping registers during operation on its own, it is not necessary to read those registers right before using getters. An application may rely on their cached values.


<a id="interface"></a>
## Interface

#### Main
- [gbj_ds1307()](#gbj_ds1307)
- [begin()](#begin)
- [startClock()](#startClock)
- [stopClock()](#stopClock)
- [convertDateTime()](#convertDateTime)

#### Setters
- [setDateTime()](#setDateTime)
- [setConfiguration()](#setConfiguration)
- [configClockEnable()](#configClock)
- [configClockDisable()](#configClock)
- [configSqwEnable()](#configSqw)
- [configSqwDisable()](#configSqw)
- [configSqwLevelHigh()](#configSqwLevel)
- [configSqwLevelLow()](#configSqwLevel)
- [configSqwRate()](#configSqwRate)

#### Getters
- [getConfiguration()](#getConfiguration)
- [getPowerUp()](#getPowerUp)
- [getDateTime()](#getDateTime)
- [getClockEnabled()](#getClockEnabled)
- [getClockMode12H()](#getClockMode12H)
- [getSqwRate()](#getSqwRate)
- [getSqwLevel()](#getSqwLevel)
- [getSqwEnabled()](#getSqwEnabled)

Other possible setters and getters are inherited from the parent library [gbjMemory](#dependency) and described there.


<a id="gbj_ds1307"></a>
## gbj_ds1307()
#### Description
The library does not need special constructor and destructor, so that the inherited ones are good enough and there is no need to define them in the library, just use it with default or specific parameters as defined at constructor of parent library [gbjMemory](#dependency).
- Constructor sets parameters specific to the two-wire bus in general.
- All the constructor parameters can be changed dynamically with corresponding setters later in a sketch.
- Although the datasheet for the RTC chip claims it work on 100 kHz frequency of the two-wire bus, an experiments proved, that it works on 400 kHz frequency as well. So that the constructor does not force the 100 kHz frequency, just default it.

#### Syntax
    gbj_ds1307(uint32_t clockSpeed, uint8_t pinSDA, uint8_t pinSCL);

#### Parameters
<a id="prm_busClock"></a>
- **clockSpeed**: Two-wire bus clock frequency in Hertz. If the clock is not from enumeration, it fallbacks to 100 kHz.
  - *Valid values*: gbj\_ds1307::CLOCK\_100KHZ, gbj\_ds1307::CLOCK\_400KHZ
  - *Default value*: gbj\_ds1307::CLOCK\_100KHZ


<a id="prm_pinSDA"></a>
- **pinSDA**: Microcontroller's pin for serial data. It is not a board pin but GPIO number. For hardware two-wire bus platforms it is irrelevant and none of methods utilizes this parameter for such as platforms for communication on the bus. On the other hand, for those platforms the parameters might be utilized for storing some specific attribute in the class instance object.
  - *Valid values*: positive integer
  - *Default value*: 4 (GPIO4, D2)


<a id="prm_pinSCL"></a>
- **pinSCL**: Microcontroller's pin for serial clock. It is not a board pin but GPIO number. For hardware two-wire bus platforms it is irrelevant and none of methods utilizes this parameter for such as platforms. On the other hand, for those platforms the parameters might be utilized for storing some specific attribute in the class instance object.
  - *Valid values*: positive integer
  - *Default value*: 5 (GPIO5, D1)

#### Returns
Object performing the sensor management.
The constructor cannot return [a result or error code](#constants) directly, however, it stores them in the instance object. The result can be tested in the operational code with the method [getLastResult()](#getLastResult), [isError()](#isError), or [isSuccess()](#isSuccess).

[Back to interface](#interface)


<a id="begin"></a>
## begin()
#### Description
The method initiates the RTC chip and two-wire bus.
- The method sets parameters of non-volatile memory and reads configuration register to its cache..

#### Syntax
    uint8_t begin();

#### Parameters
None

#### Returns
Some of [result or error codes](#constants).

[Back to interface](#interface)


<a id="startClock"></a>
## startClock()
#### Description
The particular method sets the date and time to the RTC chip and starts its internal oscillator.
- The method is overloaded, either for flashed strings or strings in SRAM pointers for date and time.
- The method sets datetime regardless the RTC chip is running or not.
- If no input parameters are used, just the *clock halt* (CH) bit of seconds time keeping register is set with retaining current second.
- The methods are useful at using the compilation __DATE__ and __TIME__ constants.

#### Syntax
    uint8_t startClock(const char* strDate, const char* strTime, uint8_t weekday = 1, bool mode12h = false);
    uint8_t startClock(const __FlashStringHelper* strDate, const __FlashStringHelper* strTime, uint8_t weekday = 1, bool mode12h = false);
    uint8_t startClock();

#### Parameters
- __strDate__: Pointer to a system date formatted string.
  - *Valid values*: address range
  - *Default value*: none


- **strTime**: Pointer to a system time formatted string.
  - *Valid values*: address range
  - *Default value*: none


- **weekday**: Number of current day in a week. It is up to an application to set the starting day in the week. If weekdays are irrelevant, the default value may be used. The provided weekday fallbacks to valid range.
  - *Valid values*: 1 ~ 7
  - *Default value*: 1


- **mode12h**: Flag about using 12 hours mode.
  - *Valid values*: true = 12 hours mode, false = 24 hours mode
  - *Default value*: false (24 hours mode)

#### Returns
Some of [result or error codes](#constants).

#### Example
```cpp
gbj_ds1307 Device = gbj_ds1307();
Device.startClock(__DATE__, __TIME__, 3); // 24h mode
Device.startClock(__DATE__, __TIME__, 3, true);  // 12h mode
Device.startClock(F(__DATE__), F(__TIME__)); // Flashed strings
Device.startClock();  // Start just internal oscillator
```

#### See also
[setDateTime()](#setDateTime)

[stopClock()](#stopClock)

[Back to interface](#interface)


<a id="stopClock"></a>
## stopClock()
#### Description
The method resets *clock halt* (CH) bit of the seconds time keeping register with retaining current second in it.

#### Syntax
    uint8_t stopClock();

#### Parameters
None

#### Returns
Some of [result or error codes](#constants).

#### See also
[startClock()](#startClock)

[Back to interface](#interface)


<a id="startSqw"></a>
## startSqw()
#### Description
The method enables generating square wave signal on SQW/OUT pin of the RTC chip at frequency determined by the input parameter and starts the clock.
- The method sends new value of the control register or seconds time keeping register to the chip only if cached value differs from desired one in order to avoid useless communication on the two-wire bus.

#### Syntax
    uint8_t startSqw();

#### Parameters
- **rate**: Value of pair of RS1 and RS0 bits. It fallbacks to least significant 2 bits.
  - *Valid values*: [gbj\_ds1307::SQW\_RATE\_1HZ ~ gbj\_ds1307::SQW\_RATE\_32KHZ](#SQW)
  - *Default value*: [gbj\_ds1307::SQW\_RATE\_32KHZ](#SQW)

#### Returns
Some of [result or error codes](#constants).

#### See also
[configSqwRate()](#configSqwRate)

[configSqwEnable()](#configSqwEnable)

[startClock()](#startClock)

[Back to interface](#interface)


<a id="setDateTime"></a>
## setDateTime()
#### Description
The method sanitizes datetime parameters taken from referenced external structure (datetime record) and writes them to the RTC chip.
- The method strips century from the year and writes just two-digit year number.
- The method writes cached value to the control register of the chip as well, so that the chip can be set, started and configured at once.

#### Syntax
    uint8_t setDateTime(const Datetime &dtRecord);

#### Parameters
- **dtRecord**: Referenced structure variable for desired date and time defined in the library [gbjAppHelpers](#dependency) and declared in the library [gbjDS1307](#library) as an alias.
  - *Valid values*: as described for the library [gbjAppHelpers](#dependency)
  - *Default value*: none

#### Returns
Some of [result or error codes](#constants).

#### Example
```cpp
gbj_ds1307 Device = gbj_ds1307();
gbj_ds1307::Datetime rtcDateTime;
void setup()
{
  rtcDateTime.year = 2018;
  rtcDateTime.month = 12;
  rtcDateTime.day = 26;
  rtcDateTime.weekday = 3;
  rtcDateTime.hour = 0;
  rtcDateTime.minute = 54;
  rtcDateTime.second = 32;
  rtcDateTime.mode12h = true;
  rtcDateTime.pm = false;
  //
  Device.setDateTime(rtcDateTime);
}
```

#### See also
[getDateTime()](#getDateTime)

[startClock()](#startClock)

[Back to interface](#interface)


<a id="convertDateTime"></a>
## convertDateTime()
#### Description
The method converts already read datetime from the chip and stored in instance internal structure to the referenced external structure (datetime record).
- The method converts recently read datetime from the chip without repeating the reading from it. It is useful right after begin() method, which reads the chip's status in either case.
- The method expects 21th century, so that adds 2000 to the read two-digit year number.

#### Syntax
    void convertDateTime(Datetime &dtRecord);

#### Parameters
- **dtRecord**: Referenced structure variable for placing read date and time defined in the library [gbjAppHelpers](#dependency) and declared in the library [gbjDS1307](#library) as an alias.
  - *Valid values*: as described for the library [gbjAppHelpers](#dependency)
  - *Default value*: none

#### Returns
None

#### See also
[getDateTime()](#getDateTime)

[Back to interface](#interface)


<a id="getDateTime"></a>
## getDateTime()
#### Description
The method reads datetime from the RTC chip, converts it and place it to the referenced external structure (datetime record).
- The method reads configuration register to its cache as well.

#### Syntax
    uint8_t getDateTime(Datetime &dtRecord);

#### Parameters
- **dtRecord**: Referenced structure variable for placing read date and time defined in the library [gbjAppHelpers](#dependency) and declared in the library [gbjDS1307](#library) as an alias.
  - *Valid values*: as described for the library [gbjAppHelpers](#dependency)
  - *Default value*: none

#### Returns
Some of [result or error codes](#constants).

#### See also
[convertDateTime()](#convertDateTime)

[setDateTime()](#setDateTime)

[Back to interface](#interface)


<a id="setConfiguration"></a>
## setConfiguration()
#### Description
The method writes the new content of the configuration register stored in the instance object (configuration cache) to the chip. This content should has been prepared by methods of names `configXXX` right before.

#### Syntax
    uint8_t setConfiguration();

#### Parameters
None

#### Returns
Some of [result or error codes](#constants).

#### See also
[getConfiguration()](#getConfiguration)

[Back to interface](#interface)


<a id="getConfiguration"></a>
## getConfiguration()
#### Description
The method provides content of the configuration register from its cache read or updated recently.

#### Syntax
    uint8_t getConfiguration();

#### Parameters
None

#### Returns
Content of the configuration register cache.

#### See also
[setConfiguration()](#setConfiguration)

[Back to interface](#interface)


<a id="configClock"></a>
## configClockEnable(), configClockDisable()
#### Description
The particular method sets or resets *clock halt* (CH) bit of the seconds time keeping register. After setting this bit by sending date and time to the chip, its internal oscillator is started or stopped and real time clock is running or halted.
- The status of the CH bit is sent to the chip by the method [setDateTime()](#setDateTime), but this method does not change the CH bit on its own and retains the value set by particular `configClockXXX` method.
- The status of the CH bit is set and sent to the chip by the method [startClock()](#startClock) as well, but in this case this method sets it to enable the clock running (it calls `configClockEnable()` by itself).

#### Syntax
    void configClockEnable();
    void configClockDisable();

#### Parameters
None

#### Returns
None

[Back to interface](#interface)


<a id="configSqw"></a>
## configSqwEnable(), configSqwDisable()
#### Description
The particular method sets or resets *square wave enable* (SQWE) bit in the configuration register cache. After sending that cached value to the chip, the generating of square wave signal on the SQW/OUT pin of the chip starts or stops.

#### Syntax
    void configSqwEnable();
    void configSqwDisable();

#### Parameters
None

#### Returns
None

#### See also
[configSqwLevelHigh(), configSqwLevelLow()](#configSqwLevel)

[configSqwRate()](#configSqwRate)

[Back to interface](#interface)


<a id="configSqwLevel"></a>
## configSqwLevelHigh(), configSqwLevelLow()
#### Description
The particular method sets or resets *output control* (OUT) bit in the configuration register cache. It determines stable level of the SQW/OUT pin of the chip when generating square wave signal is disabled.

#### Syntax
    void configSqwLevelHigh();
    void configSqwLevelLow();

#### Parameters
None

#### Returns
None

#### See also
[configSqwEnable(), configSqwDisable()](#configSqw)

[Back to interface](#interface)


<a id="configSqwRate"></a>
## configSqwRate()
#### Description
The method sets *rate select* (RS0, RS1) bits in the configuration register cache. It determines frequency of the generated square wave signal on the SQW/OUT pin when the generating is enabled. The particular frequency is determined by corresponding library [constant](#SQW).

#### Syntax
    void configSqwRate(uint8_t rate);

#### Parameters
- **rate**: Value of pair of RS1 and RS0 bits. It fallbacks to least significant 2 bits.
  - *Valid values*: [gbj\_ds1307::SQW\_RATE\_1HZ ~ gbj\_ds1307::SQW\_RATE\_32KHZ](#SQW)
  - *Default value*: None

#### Returns
None

#### See also
[configSqwEnable(), configSqwDisable()](#configSqw)

[Back to interface](#interface)


<a id="getPowerUp"></a>
## getPowerUp()
#### Description
The method provides flag determining that the configuration register has the value typical for its power-up content. It does not necessary mean, that the chip has been turned off and on, just the configuration register values in the same.

#### Syntax
    bool getPowerUp();

#### Parameters
None

#### Returns
Flag about content of the configuration register typical for power-up state.

[Back to interface](#interface)


<a id="getClockEnabled"></a>
## getSqwEnabled()
#### Description
The method provides flag whether the CH bit of the seconds time keeping register is set or not, i.e., whether the internal oscillator of the chip and real time clock is running or not.

#### Syntax
    bool getClockEnabled();

#### Parameters
None

#### Returns
Flag about running real time clock.

#### See also
[configClockEnable(), configClockDisable()](#configClock)

[Back to interface](#interface)


<a id="getClockMode12H"></a>
## getClockMode12H()
#### Description
The method provides flag whether the 12 hours mode bit in the hours time keeping register is set or not, i.e., whether the time is expressed in 12 or 24 hours form.

#### Syntax
    bool getClockMode12H();

#### Parameters
None

#### Returns
Flag about active 12 hours mode.

[Back to interface](#interface)


<a id="getSqwRate"></a>
## getSqwRate()
#### Description
The method provides current set square wave frequency regardless if the generating is enabled and running or not.

#### Syntax
    uint8_t getSqwRate();

#### Parameters
None

#### Returns
SQW frequency in form of one of the library constants [gbj\_ds1307::SQW\_RATE\_1HZ ~ gbj\_ds1307::SQW\_RATE\_32KHZ](#SQW).

#### See also
[configSqwRate()](#configSqwRate)

[Back to interface](#interface)


<a id="getSqwLevel"></a>
## getSqwLevel()
#### Description
The method provides output level of the SQW/OUT pin of the chip when the generating of square wave signal is disabled.

#### Syntax
    uint8_t getSqwLevel();

#### Parameters
None

#### Returns
Output level 1 or 0, i.e., HIGH or LOW.

#### See also
[configSqwLevelHigh(), configSqwLevelLow()](#configSqwLevel)

[configSqwEnable(), configSqwDisable()](#configSqw)

[Back to interface](#interface)


<a id="getSqwEnabled"></a>
## getSqwEnabled()
#### Description
The method provides flag whether the SQWE bit of the configuration register is set or not, i.e., whether the generating of the square wave signal is enabled or not.

#### Syntax
    bool getSqwEnabled();

#### Parameters
None

#### Returns
Flag about enabled square wave signal generation.

#### See also
[configSqwEnable(), configSqwDisable()](#configSqw)

[Back to interface](#interface)

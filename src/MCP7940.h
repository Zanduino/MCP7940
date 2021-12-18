// clang-format off
/*!
@file MCP7940.h

@mainpage Arduino Library Header for the MCP7940M and MCP7940N Real-Time Clock devices

@section MCP7940_intro_section Description

Class definition header for the MCP7940 from Microchip. Both the MCP7940N (with battery backup pin
and the MCP7940M are supported with this library and they both use the same I2C address. This chip
is a Real-Time-Clock with an I2C interface. The data sheet located at
http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf describes the functionality used in
this library.\n
se is made of portions of Adafruit's RTClib Version 1.2.0 at https://github.com/adafruit/RTClib
which is a fork of the original RTClib from Jeelabs. The re-used code encompasses only the classes
for time and date.\n

@section doxygen configuration
This library is built with the standard "Doxyfile", which is located at
https://github.com/Zanduino/Common/blob/main/Doxygen. As described on that page, there are only 5
environment variables used, and these are set in the project's actions file, located at
https://github.com/Zanduino/MCP7940/blob/master/.github/workflows/ci-doxygen.yml
Edit this file and set the 5 variables: PRETTYNAME, PROJECT_NAME, PROJECT_NUMBER, PROJECT_BRIEF and
PROJECT_LOGO so that these values are used in the doxygen documentation.
The local copy of the doxyfile should be in the project's root directory in order to do local
doxygen testing, but the file is ignored on upload to GitHub.

@section clang-format
Part of the GitHub actions for CI is running every source file through "clang-format" to ensure
that coding formatting is done the same for all files. The configuration file ".clang-format" is
located at https://github.com/Zanduino/Common/tree/main/clang-format and this is used for CI tests
when pushing to GitHub. The local file, if present in the root directory, is ignored when
committing and uploading.

@section license GNU General Public License v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

 @section author Author

Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin

 @section versions Changelog

Version| Date       | Developer           | Comments
------ | ---------- | ------------------- | --------
1.2.2  | 2021-12-16 | BrotherV            | Issue #63 - Add ESP8266 support for defining SDA and SCL pins.
1.2.1  | 2021-05-23 | SV-Zanshin          | Issue #60 - Correct handling of dates < 2000-01-01
1.2.1  | 2021-01-06 | SV-Zanshin          | Issue #58 - Corrected return reference values in readRAM and readEUI and added writeEUI
1.2.1  | 2021-01-05 | masterx1981         | Issue #58 - Add support for MCP79401 and MCP79402 read EUI data
1.1.9  | 2020-11-26 | SV-Zanshin          | Issue #54 - Optimize c++ code / resilience. Uniform Initialization. Consolidated I2C calls.
1.1.8  | 2020-11-15 | SV-Zanshin          | Issue #50 - Reformat with "clang-format"
1.1.8  | 2020-11-14 | SV-Zanshin          | Issue #49 - corrections for Battery Backup mode
1.1.7  | 2020-05-18 | wvmarle             | Issue #47 - adjust() method with compiler variables fix
1.1.7  | 2019-02-07 | davidlehrian        | Issue #43 - Calib. may cause overflow of "trim" variable
1.1.7  | 2019-02-07 | davidlehrian        | Issue #42 - calibrate(DateTime) to correct current dt/tm
1.1.6  | 2019-01-27 | davidlehrian        | Issue #36 - Reopened, changed statements to avoid warn.
1.1.5  | 2019-01-19 | SV-Zanshin          | Issue #40 - Change structure and layout to use doxygen
1.1.5  | 2019-01-19 | INemesisI           | Issue #37 - AlarmPolarity bit clearing on setAlarm()
1.1.5  | 2019-01-19 | SV-Zanshin          | Issue #39 - Changes to avoid Travis-CI warnings
1.1.5  | 2019-01-18 | INemesisI           | Issue #38 - Overflow on setting new TRIM value
1.1.4  | 2018-12-15 | hexeguitar          | Issue #36 - Overflow on I2C_MODES datatype corrected
1.1.4  | 2018-12-15 | huynh213            | Issue #35 - MCP7940.adjust() resets the PWRFAIL/VBATEN
1.1.3  | 2018-08-08 | amgrays             | Issue #32 - invalid return on SetMFP corrected
1.1.3  | 2018-08-05 | HannesJo0139        | Issue #31 - Calibration trim on negative numbers
1.1.2  | 2018-08-04 | SV-Zanshin          | Issue #28 - added calibrate() ovld for freq. calibration
1.1.2  | 2018-07-08 | logicaprogrammabile | Issue #26 - regarding hour bitmasks
1.1.1  | 2018-07-07 | SV-Zanshin          | Fixed bugs introduced by 14, 20, and 21
1.1.1  | 2018-07-07 | wvmarle             | Pull #21  - Additional changes
1.1.1  | 2018-07-06 | wvmarle             | Pull #20  - Numerous changes and enhancements
1.1.0  | 2018-07-05 | wvmarle             | Pull #14  - bug fixes to alarm state and comment cleanup
1.0.8  | 2018-07-02 | SV-Zanshin          | Added guard code for multiple I2C constant definitions
1.0.8  | 2018-06-30 | SV-Zanshin          | Issue #15 - Added I2C Speed selection
1.0.7  | 2018-06-21 | SV-Zanshin          | Issue #13 - DateTime.dayOfTheWeek() is 0-6 instead 1-7
1.0.6  | 2018-04-29 | SV-Zanshin          | Issue  #7 - Moved param defaults to prototypes
1.0.6  | 2018-04-29 | SV-Zanshin          | Issue #10 - Setting of alarm with WKDAY to future date
1.0.5b | 2017-12-18 | SV-Zanshin          | Issue  #8 - Setting to 24-Hour clock
1.0.5a | 2017-10-31 | SV-Zanshin          | Issue  #6 - Remove classification on template functions
1.0.4c | 2017-08-13 | SV-Zanshin          | Issue  #5 - Remove checks after Wire.requestFrom()
1.0.4b | 2017-08-08 | SV-Zanshin          | Replaced readRAM and writeRAM with template functions
1.0.4a | 2017-08-06 | SV-Zanshin          | Removed unnecessary MCP7940_I2C_Delay and all references
1.0.3  | 2017-08-05 | SV-Zanshin          | Added calls for MCP7940N. getPowerFail(),clearPowerFail(), setBattery(). Added I2C_READ_ATTEMPTS to prevent I2C hang, added getPowerUp/Down()
1.0.3a | 2017-07-29 | SV-Zanshin          | Cleaned up comments, no code changes
1.0.3  | 2017-07-29 | SV-Zanshin          | getSQWSpeed(),setSQWSpeed(),setSQWState(),getSQWState()
1.0.2  | 2017-07-29 | SV-Zanshin          | getAlarm(),setAlarmState(),getAlarmState(),isAlarm()added optional setting setAlarm(). Fixed alarm 1 index
1.0.1  | 2017-07-25 | SV-Zanshin          | Overloaded Calibrate() to manually set trim factor
1.0.0  | 2017-07-23 | SV-Zanshin          | Cleaned up code, initial github upload
1.0.2b | 2017-07-20 | SV-Zanshin          | Added alarm handling
1.0.1b | 2017-07-19 | SV-Zanshin          | Added methods
1.0.0b | 2017-07-17 | SV-Zanshin          | Initial coding
*/

// clang-format on

#include "Arduino.h"  // Arduino data type definitions
#include "Wire.h"     // Standard I2C "Wire" library
#ifndef MCP7940_h
  /** @brief  Guard code definition */
  #define MCP7940_h  // Define the name inside guard code
/***************************************************************************************************
** Declare classes used in within the class                                                       **
***************************************************************************************************/
class TimeSpan;
  /***************************************************************************************************
  ** Declare constants used in the class **
  ***************************************************************************************************/
  #ifndef I2C_MODES  // I2C related constants
    /** @brief Guard code definition */
    #define I2C_MODES
const uint32_t I2C_STANDARD_MODE{100000};  ///< Default normal I2C 100KHz speed
const uint32_t I2C_FAST_MODE{400000};      ///< Fast mode
  #endif
  #if !defined(BUFFER_LENGTH)  // The ESP32 Wire library doesn't currently define BUFFER_LENGTH
    /** @brief If the "Wire.h" library doesn't define the buffer, do so here */
    #define BUFFER_LENGTH 32
  #endif

const uint8_t  MCP7940_ADDRESS{0x6F};          ///< Fixed I2C address, fixed
const uint8_t  MCP7940_EUI_ADDRESS{0x57};      ///< Fixed I2C address for accessing protected ROM
const uint8_t  MCP7940_RTCSEC{0x00};           ///< Timekeeping, RTCSEC Register address
const uint8_t  MCP7940_RTCMIN{0x01};           ///< Timekeeping, RTCMIN Register address
const uint8_t  MCP7940_RTCHOUR{0x02};          ///< Timekeeping, RTCHOUR Register address
const uint8_t  MCP7940_RTCWKDAY{0x03};         ///< Timekeeping, RTCWKDAY Register address
const uint8_t  MCP7940_RTCDATE{0x04};          ///< Timekeeping, RTCDATE Register address
const uint8_t  MCP7940_RTCMTH{0x05};           ///< Timekeeping, RTCMTH Register address
const uint8_t  MCP7940_RTCYEAR{0x06};          ///< Timekeeping, RTCYEAR Register address
const uint8_t  MCP7940_CONTROL{0x07};          ///< Timekeeping, RTCCONTROL Register address
const uint8_t  MCP7940_OSCTRIM{0x08};          ///< Timekeeping, RTCOSCTRIM Register address
const uint8_t  MCP7940_EEUNLOCK{0x09};         ///< Virtual unlock register on MCP7940x series
const uint8_t  MCP7940_ALM0SEC{0x0A};          ///< Alarm 0, ALM0SEC Register address
const uint8_t  MCP7940_ALM0MIN{0x0B};          ///< Alarm 0, ALM0MIN Register address
const uint8_t  MCP7940_ALM0HOUR{0x0C};         ///< Alarm 0, ALM0HOUR Register address
const uint8_t  MCP7940_ALM0WKDAY{0x0D};        ///< Alarm 0, ALM0WKDAY Register address
const uint8_t  MCP7940_ALM0DATE{0x0E};         ///< Alarm 0, ALM0DATE Register address
const uint8_t  MCP7940_ALM0MTH{0x0F};          ///< Alarm 0, ALM0MTH Register address
const uint8_t  MCP7940_ALM1SEC{0x11};          ///< Alarm 1, ALM1SEC Register address
const uint8_t  MCP7940_ALM1MIN{0x12};          ///< Alarm 1, ALM1MIN Register address
const uint8_t  MCP7940_ALM1HOUR{0x13};         ///< Alarm 1, ALM1HOUR Register address
const uint8_t  MCP7940_ALM1WKDAY{0x14};        ///< Alarm 1, ALM1WKDAY Register address
const uint8_t  MCP7940_ALM1DATE{0x15};         ///< Alarm 1, ALM1DATE Register address
const uint8_t  MCP7940_ALM1MTH{0x16};          ///< Alarm 1, ALM1MONTH Register address
const uint8_t  MCP7940_PWRDNMIN{0x18};         ///< Power-Fail, PWRDNMIN Register address
const uint8_t  MCP7940_PWRDNHOUR{0x19};        ///< Power-Fail, PWRDNHOUR Register address
const uint8_t  MCP7940_PWRDNDATE{0x1A};        ///< Power-Fail, PWDNDATE Register address
const uint8_t  MCP7940_PWRDNMTH{0x1B};         ///< Power-Fail, PWRDNMTH Register address
const uint8_t  MCP7940_PWRUPMIN{0x1C};         ///< Power-Fail, PWRUPMIN Register address
const uint8_t  MCP7940_PWRUPHOUR{0x1D};        ///< Power-Fail, PWRUPHOUR Register address
const uint8_t  MCP7940_PWRUPDATE{0x1E};        ///< Power-Fail, PWRUPDATE Register address
const uint8_t  MCP7940_PWRUPMTH{0x1F};         ///< Power-Fail, PWRUPMTH Register address
const uint8_t  MCP7940_RAM_ADDRESS{0x20};      ///< NVRAM - Start address for SRAM
const uint8_t  MCP7940_EUI_RAM_ADDRESS{0xF0};  ///< EUI - Start address for protected EEPROM
const uint8_t  MCP7940_ST{7};                  ///< MCP7940 register bits. RTCSEC reg
const uint8_t  MCP7940_12_24{6};               ///< RTCHOUR, PWRDNHOUR & PWRUPHOUR
const uint8_t  MCP7940_AM_PM{5};               ///< RTCHOUR, PWRDNHOUR & PWRUPHOUR
const uint8_t  MCP7940_OSCRUN{5};              ///< RTCWKDAY register
const uint8_t  MCP7940_PWRFAIL{4};             ///< RTCWKDAY register
const uint8_t  MCP7940_VBATEN{3};              ///< RTCWKDAY register
const uint8_t  MCP7940_LPYR{5};                ///< RTCMTH register
const uint8_t  MCP7940_OUT{7};                 ///< CONTROL register
const uint8_t  MCP7940_SQWEN{6};               ///< CONTROL register
const uint8_t  MCP7940_ALM1EN{5};              ///< CONTROL register
const uint8_t  MCP7940_ALM0EN{4};              ///< CONTROL register
const uint8_t  MCP7940_EXTOSC{3};              ///< CONTROL register
const uint8_t  MCP7940_CRSTRIM{2};             ///< CONTROL register
const uint8_t  MCP7940_SQWFS1{1};              ///< CONTROL register
const uint8_t  MCP7940_SQWFS0{0};              ///< CONTROL register
const uint8_t  MCP7940_SIGN{7};                ///< OSCTRIM register
const uint8_t  MCP7940_ALMPOL{7};              ///< ALM0WKDAY register
const uint8_t  MCP7940_ALM0IF{3};              ///< ALM0WKDAY register
const uint8_t  MCP7940_ALM1IF{3};              ///< ALM1WKDAY register
const uint32_t SECS_1970_TO_2000{946684800};   ///< Seconds between year 1970 and 2000

class DateTime {
  /*!
    @class   DateTime
    @brief   Simple general-purpose date/time class
    @details Copied from RTClib. For further information on this implementation see
             https://github.com/Zanduino/MCP7940/wiki/DateTimeClass
  */
 public:
  DateTime(uint32_t t = SECS_1970_TO_2000);
  DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t min = 0,
           uint8_t sec = 0);
  DateTime(const DateTime& copy);
  DateTime(const char* date, const char* time);
  DateTime(const __FlashStringHelper* date, const __FlashStringHelper* time);
  uint16_t year() const { /*! return the current year */
    return 2000U + yOff;
  }
  uint8_t month() const { /*! return the current month */
    return m;
  }
  uint8_t day() const { /*! return the current day of the month */
    return d;
  }
  uint8_t hour() const { /*! return the current hour */
    return hh;
  }
  uint8_t minute() const { /*! return the current minute */
    return mm;
  }
  uint8_t second() const { /*! return the current second */
    return ss;
  }
  uint8_t  dayOfTheWeek() const;            /*! return the current day of the week starting at 0 */
  long     secondstime() const;             /*! return the current seconds in the year */
  uint32_t unixtime(void) const;            /*! return the current Unixtime */
  bool     equals(const DateTime* other);   /*! return true if two datetimes hold the same value */
  DateTime operator+(const TimeSpan& span); /*! Overloaded "+" operator to add two timespans */
  DateTime operator-(const TimeSpan& span); /*! Overloaded "+" operator to add two timespans */
  TimeSpan operator-(
      const DateTime& right); /*! Overloaded "-" operator subtract add two timespans */
 protected:
  uint8_t yOff;  ///< Internal year offset value
  uint8_t m;     ///< Internal month value
  uint8_t d;     ///< Internal day value
  uint8_t hh;    ///< Internal hour value
  uint8_t mm;    ///< Internal minute value
  uint8_t ss;    ///< Internal seconds
};               // of class DateTime definition
class TimeSpan {
  /*!
   @class   TimeSpan
   @brief   Timespan class which can represent changes in time with seconds accuracy
   @details Copied from RTClib. For further information see
            https://github.com/Zanduino/MCP7940/wiki/TimeSpanClass for additional details
  */
 public:
  TimeSpan(int32_t seconds = 0);                                         ///< Default constructor
  TimeSpan(int16_t days, int8_t hours, int8_t minutes, int8_t seconds);  ///< Overloaded constructor
  TimeSpan(const TimeSpan& copy);                                        ///< Overloaded constructor
  int16_t  days() const { return _seconds / 86400L; }      ///< return the number of days
  int8_t   hours() const { return _seconds / 3600 % 24; }  ///< return number of hours
  int8_t   minutes() const { return _seconds / 60 % 60; }  ///< return number of minutes
  int8_t   seconds() const { return _seconds % 60; }       ///< return number of seconds
  int32_t  totalseconds() const { return _seconds; }       ///< return total number of seconds
  TimeSpan operator+(const TimeSpan& right);               ///< redefine "+" operator
  TimeSpan operator-(const TimeSpan& right);               ///< redefine "-" operator
 protected:
  int32_t _seconds;  ///< Internal value for total seconds
};                   // of class TimeSpan definition

class MCP7940_Class {
  /*!
   @class MCP7940_Class
   @brief Main class definition with forward declarations
  */
 public:
  MCP7940_Class(){};   ///< Unused Class constructor
  ~MCP7940_Class(){};  ///< Unused Class destructor
  bool     begin(const uint32_t i2cSpeed) const;
  bool     begin(const uint8_t sda = SDA, const uint8_t scl = SCL,
                 const uint32_t i2cSpeed = I2C_STANDARD_MODE) const;
  bool     deviceStatus() const;
  bool     deviceStart() const;
  bool     deviceStop() const;
  DateTime now() const;
  void     adjust();
  void     adjust(const DateTime& dt);
  int8_t   calibrate() const;
  int8_t   calibrate(const int8_t newTrim);
  int8_t   calibrate(const DateTime& dt);
  int8_t   calibrate(const float fMeas) const;
  int8_t   getCalibrationTrim() const;
  uint8_t  weekdayRead() const;
  uint8_t  weekdayWrite(const uint8_t dow) const;
  bool     setMFP(const bool value) const;
  uint8_t  getMFP() const;
  bool     setAlarm(const uint8_t alarmNumber, const uint8_t alarmType, const DateTime& dt,
                    const bool state = true) const;
  void     setAlarmPolarity(const bool polarity) const;
  DateTime getAlarm(const uint8_t alarmNumber, uint8_t& alarmType) const;
  bool     clearAlarm(const uint8_t alarmNumber) const;
  bool     setAlarmState(const uint8_t alarmNumber, const bool state) const;
  bool     getAlarmState(const uint8_t alarmNumber) const;
  bool     isAlarm(const uint8_t alarmNumber) const;
  uint8_t  getSQWSpeed() const;
  bool     setSQWSpeed(uint8_t frequency, bool state = true) const;
  bool     setSQWState(const bool state) const;
  bool     getSQWState() const;
  bool     setBattery(const bool state) const;
  bool     getBattery() const;
  bool     getPowerFail() const;
  bool     clearPowerFail() const;
  DateTime getPowerDown() const;
  DateTime getPowerUp() const;
  int8_t   calibrateOrAdjust(const DateTime& dt);
  int32_t  getPPMDeviation(const DateTime& dt) const;
  void     setSetUnixTime(uint32_t aTime);
  uint32_t getSetUnixTime() const;

  /*************************************************************************************************
  ** Template functions definitions are done in the header file                                   **
  ** ============================================================================================ **
  ** readRAM   read any number of bytes from the MCP7940 SRAM area                                **
  ** writRAM   write any number of bytes to the MCP7940 SRAM area                                 **
  ** readEUI   read any number of bytes from the special protected SRAM area for 79400/401/402    **
  *************************************************************************************************/
  template <typename T>
  uint8_t readRAM(const uint8_t& addr, T& value) const {
    /*!
     @brief     Template for readRAM()
     @details   As a template it can support compile-time data type definitions
     @param[in] addr Memory address
     @param[in] value    Data Type "T" to read
     @return    Pointer to return data structure
    */
    uint8_t i = I2C_read((addr % 64) + MCP7940_RAM_ADDRESS, value);
    return (i);
  }  // of method readRAM()
  template <typename T>
  uint8_t writeRAM(const uint8_t& addr, const T& value) const {
    /*!
     @brief     Template for writeRAM()
     @details   As a template it can support compile-time data type definitions
     @param[in] addr Memory address
     @param[in] value Data Type "T" to write
     @return    True if successful, otherwise false
     */
    uint8_t i = I2C_write((addr % 64) + MCP7940_RAM_ADDRESS, value);
    return i;
  }  // of method writeRAM()
  template <typename T>
  uint8_t readEUI(const uint8_t& addr, T& value) const {
    /*!
     @brief     Template for readEUI()
     @details   As a template it can support compile-time data type definitions. This is a special
                call as it access a different I2C address and a different memory block
     @param[in] addr     Memory address
     @param[in] value    Data Type "T" to read
     @return             Pointer to return data structure
    */
    uint8_t i{0};                                        // return number of bytes read
    Wire.beginTransmission(MCP7940_EUI_ADDRESS);         // Address the special I2C address
    Wire.write((addr % 8) + MCP7940_EUI_RAM_ADDRESS);    // Send register address to read from
    if (Wire.endTransmission() == 0) {                   // Close transmission and check error code
      Wire.requestFrom(MCP7940_EUI_ADDRESS, sizeof(T));  // Request a block of data, max 61 bits
      uint8_t* bytePtr = (uint8_t*)&value;               // Declare pointer to start of structure
      for (i = 0; i < sizeof(T); i++) {                  // Loop for each byte to be read
        *bytePtr++ = Wire.read();                        // Read a byte
      }                                                  // of for-next each byte
    }                                                    // if-then success
    return i;                                            // return number of bytes read
  }                                                      // of method readEUI()
  template <typename T>
  uint8_t writeEUI(const uint8_t& addr, T& value) const {
    /*!
     @brief     Template for writeEUI()
     @details   As a template it can support compile-time data type definitions. This is a special
                call as it access a different I2C address and a different memory block and also has
                to unlock the area prior to writing
     @param[in] addr     Memory address
     @param[in] value    Data Type "T" to read
     @return             Pointer to  data structure to write
    */
    uint8_t i{0};                                      // return number of bytes read
    Wire.beginTransmission(MCP7940_EUI_ADDRESS);       // Address the special I2C address
    Wire.write(MCP7940_EEUNLOCK);                      // Send special register address to write to
    Wire.write(0x55);                                  // Special write value to start unlock
    i = Wire.endTransmission();                        // close transmission of first byte
    Wire.beginTransmission(MCP7940_EUI_ADDRESS);       // Address the special I2C address
    Wire.write(MCP7940_EEUNLOCK);                      // Send special register address to write to
    Wire.write(0x55);                                  // Special write value to complete unlock
    i = Wire.endTransmission();                        // close transmission of second byte
    Wire.beginTransmission(MCP7940_EUI_ADDRESS);       // Address the special I2C address
    Wire.write((addr % 8) + MCP7940_EUI_RAM_ADDRESS);  // Send register address to read from
    Wire.write((uint8_t*)&value, sizeof(T));           // write the data
    i = Wire.endTransmission();                        // close transmission of actual write
    if (i == 0) i = sizeof(T);                         // return number of bytes on success
    return i;                                          // return number of bytes read
  }                                                    // of method writeEUI()

 private:
  uint32_t _SetUnixTime{0};  ///< UNIX time when clock last set
  /*************************************************************************************************
  ** Template functions definitions are done in the header file                                   **
  ** ============================================================================================ **
  **                                                                                              **
  ** I2C_read  read any number of bytes from the MCP7940                                          **
  ** I2C_write write any number of bytes to the MCP7940                                           **
  *************************************************************************************************/
  template <typename T>
  uint8_t I2C_read(const uint8_t address, T& value) const {
    /*!
    @brief     Template for I2C_read() generic read function
    @details   The template supports reading any number of bytes from a structure. The size of the
               structure is determined by the template and determines how often the main loop is
               entered.
    @param[in] device  I2C Device number
    @param[in] address Memory address to read from on device
    @param[in] value   Data Type "T" to read
    @return    number of bytes read
   */
    uint8_t i{0};                                    // return number of bytes read
    Wire.beginTransmission(MCP7940_ADDRESS);         // Address the I2C device
    Wire.write(address);                             // Send register address to read from
    if (Wire.endTransmission() == 0) {               // Close transmission and check error code
      Wire.requestFrom(MCP7940_ADDRESS, sizeof(T));  // Request a block of data
      uint8_t* bytePtr = (uint8_t*)&value;           // Declare pointer to start of structure
      for (i = 0; i < sizeof(T); i++) {              // Loop for each byte to be read
        *bytePtr++ = Wire.read();                    // Read a byte
      }                                              // of for-next each byte
    }                                                // if-then success
    return i;                                        // return number of bytes read
  }                                                  // end of template method "I2C_read"
  template <typename T>
  uint8_t I2C_write(const uint8_t address, const T& value) const {
    /*!
      @brief     Template for I2C_write() generic read function
      @details   The template supports writing any number of bytes into a structure. The size of the
                 structure is determined by the template and determines how many bytes are written
      @param[in] device  I2C Device number
      @param[in] address Memory address to write to on device
      @param[in] value   Data Type "T" to write
      @return    number of bytes written
     */
    Wire.beginTransmission(MCP7940_ADDRESS);   // Address the I2C device
    Wire.write(address);                       // Send register address to read from
    Wire.write((uint8_t*)&value, sizeof(T));   // write the data
    uint8_t i = Wire.endTransmission();        // close transmission and save status
    if (i == 0) i = sizeof(T);                 // return number of bytes on success
    return i;                                  // return the number of bytes written
  }                                            // end of template method "I2C_write()"
  uint8_t readByte(const uint8_t addr) const;  // Read 1 byte from address on I2C
  uint8_t bcd2int(const uint8_t bcd) const;    // convert BCD digits to integer
  uint8_t int2bcd(const uint8_t dec) const;    // convert integer to BCD
  void    clearRegisterBit(const uint8_t reg, const uint8_t b) const;  // Clear a bit, values 0-7
  void    setRegisterBit(const uint8_t reg, const uint8_t b) const;    // Set   a bit, values 0-7
  void    writeRegisterBit(const uint8_t reg, const uint8_t b,
                           bool bitvalue) const;                      // Clear a bit, values 0-7
  uint8_t readRegisterBit(const uint8_t reg, const uint8_t b) const;  // Read  a bit, values 0-7
};                                                                    // of MCP7940 class definition
#endif

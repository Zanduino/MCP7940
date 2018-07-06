/*******************************************************************************************************************
** Class definition header for the MCP7940 from Microchip. Both the MCP7940N (with battery backup pin= and the    **
** MCP7940M are supported with this library and they both use the same I2C address. This chip is a Real-Time-Clock**
** with an I2C interface. The data sheet located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf **
** describes the functionality used in this library                                                               **
**                                                                                                                **
** Use is made of portions of Adafruit's RTClib Version 1.2.0 at https://github.com/adafruit/RTClib which is a    **
** a fork of the original RTClib from Jeelabs. The code encompasses simple classes for time and date.             **
**                                                                                                                **
** Although programming for the Arduino and in c/c++ is new to me, I'm a professional programmer and have learned,**
** over the years, that it is much easier to ignore superfluous comments than it is to decipher non-existent ones;**
** so both my comments and variable names tend to be verbose. The code is written to fit in the first 80 spaces   **
** and the comments start after that and go to column 117 - allowing the code to be printed in A4 landscape mode. **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer                     Comments                                                       **
** ====== ========== ============================= ============================================================== **
** 1.1.0  2018-07-05 https://github.com/wvmarle    Pull #14 - bug fixes to alarm state and cleaned up comments    **
** 1.0.8  2018-07-02 https://github.com/SV-Zanshin Added guard code against multiple I2C constant definitions     **
** 1.0.8  2018-06-30 https://github.com/SV-Zanshin Enh #15 - Added I2C Speed selection                            **
** 1.0.7  2018-06-21 https://github.com/SV-Zanshin Bug #13 - DateTime.dayOfTheWeek() is 0-6 instead of 1-7        **
** 1.0.6  2018-04-29 https://github.com/SV-Zanshin Bug #7  - Moved setting of param defaults to prototypes        **
** 1.0.6  2018-04-29 https://github.com/SV-Zanshin Bug #10 - incorrect setting of alarm with WKDAY to future date **
** 1.0.5b 2017-12-18 https://github.com/SV-Zanshin Bug #8  - incorrect setting to 24-Hour clock                   **
** 1.0.5a 2017-10-31 https://github.com/SV-Zanshin Bug #6  - to remove classification on 2 template functions     **
** 1.0.4c 2017-08-13 https://github.com/SV-Zanshin Enhancement #5 to remove checks after Wire.requestFrom()       **
** 1.0.4b 2017-08-08 https://github.com/SV-Zanshin Replaced readRAM and writeRAM with template functions          **
** 1.0.4a 2017-08-06 https://github.com/SV-Zanshin Removed unnecessary MCP7940_I2C_Delay and all references       **
** 1.0.3  2017-08-05 https://github.com/SV-Zanshin Added calls for MCP7940N. getPowerFail(), clearPowerFail(),    **
**                                                 setBattery(). Added I2C_READ_ATTEMPTS to prevent I2C hang,     **
**                                                 added getPowerUp/Down()                                        **
** 1.0.3a 2017-07-29 https://github.com/SV-Zanshin Cleaned up comments, no code changes                           **
** 1.0.3  2017-07-29 https://github.com/SV-Zanshin Added getSQWSpeed(),setSQWSpeed(),setSQWState() & getSQWState()**
** 1.0.2  2017-07-29 https://github.com/SV-Zanshin Added getAlarm(),setAlarmState(),getAlarmState() functions and **
**                                                 added the optional setting to setAlarm(). Added isAlarm().     **
**                                                 Fixed errors with alarm 1 indexing.                            **
** 1.0.1  2017-07-25 https://github.com/SV-Zanshin Added overloaded Calibrate() to manually set the trim factor   **
** 1.0.0  2017-07-23 https://github.com/SV-Zanshin Cleaned up code, initial github upload                         **
** 1.0.2b 2017-07-20 https://github.com/SV-Zanshin Added alarm handling                                           **
** 1.0.1b 2017-07-19 https://github.com/SV-Zanshin Added methods                                                  **
** 1.0.0b 2017-07-17 https://github.com/SV-Zanshin Initial coding                                                 **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include <Wire.h>                                                             // Standard I2C "Wire" library      //
#ifndef MCP7940_h                                                             // Guard code definition            //
  #define MCP7940_h                                                           // Define the name inside guard code//
  /*****************************************************************************************************************
  ** Declare classes used in within the class                                                                     **
  *****************************************************************************************************************/
  class TimeSpan;                                                             //                                  //
  
  /*****************************************************************************************************************
  ** Declare constants used in the class                                                                          **
  *****************************************************************************************************************/
  #ifndef I2C_MODES                                                           // I2C related constants            //
    #define I2C_MODES                                                         // Guard code to prevent multiple   //
    const uint16_t I2C_STANDARD_MODE      =    100000;                        // Default normal I2C 100KHz speed  //
    const uint16_t I2C_FAST_MODE          =    400000;                        // Fast mode                        //
  #endif                                                                      //----------------------------------//
  const uint8_t  MCP7940_ADDRESS          =      0x6F;                        // Device address, fixed value      //

  // MCP7940 I2C registers.
  // Section 1: timekeeping.
  const uint8_t  MCP7940_RTCSEC           =      0x00;                        // Register definitions             //
  const uint8_t  MCP7940_RTCMIN           =      0x01;                        //                                  //
  const uint8_t  MCP7940_RTCHOUR          =      0x02;                        //                                  //
  const uint8_t  MCP7940_RTCWKDAY         =      0x03;                        //                                  //
  const uint8_t  MCP7940_RTCDATE          =      0x04;                        //                                  //
  const uint8_t  MCP7940_RTCMTH           =      0x05;                        //                                  //
  const uint8_t  MCP7940_RTCYEAR          =      0x06;                        //                                  //
  const uint8_t  MCP7940_CONTROL          =      0x07;                        //                                  //
  const uint8_t  MCP7940_OSCTRIM          =      0x08;                        //                                  //
  
  // Section 2.0: alarm 0.
  const uint8_t  MCP7940_ALM0SEC          =      0x0A;                        //                                  //
  const uint8_t  MCP7940_ALM0MIN          =      0x0B;                        //                                  //
  const uint8_t  MCP7940_ALM0HOUR         =      0x0C;                        //                                  //
  const uint8_t  MCP7940_ALM0WKDAY        =      0x0D;                        //                                  //
  const uint8_t  MCP7940_ALM0DATE         =      0x0E;                        //                                  //
  const uint8_t  MCP7940_ALM0MTH          =      0x0F;                        //                                  //

  // Section 2.1: alarm 1.
  const uint8_t  MCP7940_ALM1SEC          =      0x11;                        //                                  //
  const uint8_t  MCP7940_ALM1MIN          =      0x12;                        //                                  //
  const uint8_t  MCP7940_ALM1HOUR         =      0x13;                        //                                  //
  const uint8_t  MCP7940_ALM1WKDAY        =      0x14;                        //                                  //
  const uint8_t  MCP7940_ALM1DATE         =      0x15;                        //                                  //
  const uint8_t  MCP7940_ALM1MTH          =      0x16;                        //                                  //
  
  // Section 3.0: Power-Fail Time-Stamp
  const uint8_t  MCP7940_PWRDNMIN         =      0x18;                        //                                  //
  const uint8_t  MCP7940_PWRDNHOUR        =      0x19;                        //                                  //
  const uint8_t  MCP7940_PWRDNDATE        =      0x1A;                        //                                  //
  const uint8_t  MCP7940_PWRDNMTH         =      0x1B;                        //                                  //

  // Section 3.1: Power-Fail Time-Stamp
  const uint8_t  MCP7940_PWRUPMIN         =      0x1C;                        //                                  //
  const uint8_t  MCP7940_PWRUPHOUR        =      0x1D;                        //                                  //
  const uint8_t  MCP7940_PWRUPDATE        =      0x1E;                        //                                  //
  const uint8_t  MCP7940_PWRUPMTH         =      0x1F;                        //                                  //
  
  // NVRAM
  const uint8_t  MCP7940_RAM_ADDRESS      =      0x20;                        // Start address for SRAM           //

  // MCP7940 register bits.
  const uint8_t  MCP7940_ST               =         7;                        // RTCSEC register                  //
  const uint8_t  MCP7940_12_24            =         6;                        // RTCHOUR, PWRDNHOUR and PWRUPHOUR register //
  const uint8_t  MCP7940_AM_PM            =         5;                        // RTCHOUR, PWRDNHOUR and PWRUPHOUR register //
  const uint8_t  MCP7940_OSCRUN           =         5;                        // RTCWKDAY register                //
  const uint8_t  MCP7940_PWRFAIL          =         4;                        // RTCWKDAY register                //
  const uint8_t  MCP7940_VBATEN           =         3;                        // RTCWKDAY register                //
  const uint8_t  MCP7940_LPYR             =         5;                        // RTCMTH register                  //
  const uint8_t  MCP7940_OUT              =         7;                        // CONTROL register                 //
  const uint8_t  MCP7940_SQWEN            =         6;                        // CONTROL register                 //
  const uint8_t  MCP7940_ALM1EN           =         5;                        // CONTROL register                 //
  const uint8_t  MCP7940_ALM0EN           =         4;                        // CONTROL register                 //
  const uint8_t  MCP7940_EXTOSC           =         3;                        // CONTROL register                 //
  const uint8_t  MCP7940_CRSTRIM          =         2;                        // CONTROL register                 //
  const uint8_t  MCP7940_SQWFS1           =         1;                        // CONTROL register                 //
  const uint8_t  MCP7940_SQWFS0           =         0;                        // CONTROL register                 //
  const uint8_t  MCP7940_SIGN             =         7;                        // OSCTRIM register                 //
  const uint8_t  MCP7940_ALMPOL           =         7;                        // ALM0WKDAY register               //
  const uint8_t  MCP7940_ALM0IF           =         3;                        // ALM0WKDAY register               //
  const uint8_t  MCP7940_ALM1IF           =         3;                        // ALM1WKDAY register               //

  // Other constants.
  const uint32_t SECONDS_PER_DAY          =     86400;                        // 60 secs * 60 mins * 24 hours     //
  const uint32_t SECONDS_FROM_1970_TO_2000 = 946684800;                       //                                  //
  
  /*****************************************************************************************************************
  ** Simple general-purpose date/time class (no TZ / DST / leap second handling). Copied from RTClib. For further **
  ** information on this implementation see https://github.com/SV-Zanshin/MCP7940/wiki/DateTimeClass              **
  *****************************************************************************************************************/
  class DateTime {                                                            //                                  //
    public:                                                                   //----------------------------------//
      DateTime (uint32_t t=0);                                                // Constructor                      //
      DateTime (uint16_t year,uint8_t month,uint8_t day,uint8_t hour=0,       // Overloaded Constructors          //
                uint8_t min=0,uint8_t sec=0);                                 //                                  //
      DateTime (const DateTime& copy);                                        //                                  //
      DateTime (const char* date, const char* time);                          //                                  //
      DateTime (const __FlashStringHelper* date,                              //                                  //
                const __FlashStringHelper* time);                             //                                  //
      uint16_t year()         const { return 2000 + yOff; }                   // Return the year                  //
      uint8_t  month()        const { return m; }                             // Return the month                 //
      uint8_t  day()          const { return d; }                             // Return the day                   //
      uint8_t  hour()         const { return hh; }                            // Return the hour                  //
      uint8_t  minute()       const { return mm; }                            // Return the minute                //
      uint8_t  second()       const { return ss; }                            // Return the second                //
      uint8_t  dayOfTheWeek() const;                                          // Return the DOW                   //
      long     secondstime()  const;                                          // times as seconds since 1/1/2000  //
      uint32_t unixtime(void) const;                                          // times as seconds since 1/1/1970  //
      DateTime operator+(const TimeSpan& span);                               // addition                         //
      DateTime operator-(const TimeSpan& span);                               // subtraction                      //
      TimeSpan operator-(const DateTime& right);                              // subtraction                      //
    protected:                                                                //----------------------------------//
      uint8_t yOff, m, d, hh, mm, ss;                                         // private variables                //
  }; // of class DateTime definition                                          //                                  //
  
  /*****************************************************************************************************************
  ** Timespan class which can represent changes in time with seconds accuracy. Copied from RTClib. For further    **
  ** information see ** https://github.com/SV-Zanshin/MCP7940/wiki/TimeSpanClass                                  **
  *****************************************************************************************************************/
  class TimeSpan {                                                            //                                  //
    public:                                                                   //----------------------------------//
      TimeSpan (int32_t seconds = 0);                                         //                                  //
      TimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds);  //                                  //
      TimeSpan (const TimeSpan& copy);                                        //                                  //
      int16_t  days() const         { return _seconds / 86400L; }             //                                  //
      int8_t   hours() const        { return _seconds / 3600 % 24; }          //                                  //
      int8_t   minutes() const      { return _seconds / 60 % 60; }            //                                  //
      int8_t   seconds() const      { return _seconds % 60; }                 //                                  //
      int32_t  totalseconds() const { return _seconds; }                      //                                  //
      TimeSpan operator+(const TimeSpan& right);                              //                                  //
      TimeSpan operator-(const TimeSpan& right);                              //                                  //
    protected:                                                                //----------------------------------//
      int32_t _seconds;                                                       // internal seconds variable        //
  }; // of class TimeSpan definition                                          //                                  //
  
  /*****************************************************************************************************************
  ** Main MCP7940 class for the Real-Time clock                                                                   **
  *****************************************************************************************************************/
  class MCP7940_Class {                                                       // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      MCP7940_Class();                                                        // Class constructor                //
      ~MCP7940_Class();                                                       // Class destructor                 //
      bool     begin(const uint16_t i2cSpeed = I2C_STANDARD_MODE);            // Start I2C device communications  //
      bool     deviceStatus();                                                // return true when MCP7940 is on   //
      bool     deviceStart();                                                 // Start the MCP7940 clock          //
      bool     deviceStop();                                                  // Stop the MCP7940 clock           //
      DateTime now();                                                         // return time                      //
      void     adjust();                                                      // Set the date and time to compile //
      void     adjust(const DateTime& dt);                                    // Set the date and time            //
      int8_t   calibrate();                                                   // Reset clock calibration offset   //
      int8_t   calibrate(const int8_t);                                       // Reset clock calibration offset   //
      int8_t   calibrate(const DateTime& dt);                                 // Calibrate the clock              //
      int8_t   getCalibrationTrim();                                          // Get the trim register value      //
      uint8_t  weekdayRead();                                                 // Read weekday from RTC            //
      uint8_t  weekdayWrite(const uint8_t dow);                               // Write weekday to RTC             //
      bool     setMFP(const bool value);                                      // Set the MFP pin state            //
      uint8_t  getMFP();                                                      // Get the MFP pin state            //
      bool     setAlarm(const uint8_t alarmNumber, const uint8_t alarmType,   // Set an Alarm                     //
                        const DateTime dt, const bool state = true );         //                                  //
      void     setAlarmPolarity(const bool polarity);                         // Set the polarity of the alarm    //
      DateTime getAlarm(const uint8_t alarmNumber, uint8_t &alarmType);       // Return alarm date/time & type    //
      bool     clearAlarm(const uint8_t alarmNumber);                         // Clear an Alarm                   //
      bool     setAlarmState(const uint8_t alarmNumber, const bool state);    // Return whether alarm is on or off//
      bool     getAlarmState(const uint8_t alarmNumber);                      // Return whether alarm is on or off//
      bool     isAlarm(const uint8_t alarmNumber);                            // Return whether alarm is triggered//
      uint8_t  getSQWSpeed();                                                 // Return the SQW frequency code    //
      bool     setSQWSpeed(uint8_t frequency, bool state = true);             // Set the SQW frequency to code    //
      bool     setSQWState(const bool state);                                 // Set the SQW MFP on or off        //
      bool     getSQWState();                                                 // Return if the SQW is active      //
      bool     setBattery(const bool state);                                  // Enable or disable battery backup //
      bool     getBattery();                                                  // Get the battery backup state     //
      bool     getPowerFail();                                                // Check if power fail has occurred //
      bool     clearPowerFail();                                              // Clear the power fail flag        //
      DateTime getPowerDown();                                                // Return date when power failed    //
      DateTime getPowerUp();                                                  // Return date when power restored  //
      bool     SRAMread(uint8_t address, uint8_t *data, uint8_t n);           // Read data from SRAM              //
      bool     SRAMwrite(uint8_t address, uint8_t *data, uint8_t n);          // Write data to SRAM               //

      /*************************************************************************************************************
      ** Declare the readRAM() and writeRAM() methods as template functions to use for all I2C device I/O. The    **
      ** code has to be in the main library definition rather than the actual MCP7940.cpp library file.           **
      ** The template functions allow any type of data to be read and written, be it a byte or a character array  **
      ** or a structure.                                                                                          **
      **
      ** The MCP7940 supports 64 bytes of general purpose SRAM memory, which can be used to store data. For more  **
      ** details, see datasheet p.36.                                                                             **
      ** The data is stored in a block of 64 bytes, reading beyond the end of the block causes the address        **
      ** pointer to roll over to the start of the block.                                                          **
      *************************************************************************************************************/
      template< typename T >                                                  // method to read a structure       //
      uint8_t&  readRAM(const uint8_t addr, T &value) {                       //                                  //
        uint8_t* bytePtr    = (uint8_t*)&value;                               // Pointer to structure beginning   //
        uint8_t  structSize = sizeof(T);                                      // Number of bytes in structure     //
        uint8_t  i          = 0;                                              // loop counter                     //
        Wire.beginTransmission(MCP7940_ADDRESS);                              // Address the I2C device           //
        Wire.write((addr%64) + MCP7940_RAM_ADDRESS);                          // Send register address to write   //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        Wire.requestFrom(MCP7940_ADDRESS, structSize);                        // Request the data                 //
        for (i = 0; i < structSize; i++) {                                    // loop for each byte to be read    //
          *bytePtr++ = Wire.read();                                           //                                  //
        } // of if-then bytes received match requested                        //                                  //
        return (i);                                                           // return bytes read                //
      } // of method readRAM()                                                //----------------------------------//
      
      template<typename T>                                                    // method to write any data type to //
      bool writeRAM(const uint8_t addr, const T &value) {                     // the MCP7940 SRAM                 //
        const uint8_t* bytePtr = (const uint8_t*)&value;                      // Pointer to structure beginning   //
        Wire.beginTransmission(MCP7940_ADDRESS);                              // Address the I2C device           //
        Wire.write((addr%64) + MCP7940_RAM_ADDRESS);                          // Send register address to write   //
        for (uint8_t i = 0; i < sizeof(T); i++) {                             // loop for each byte to be written //
          Wire.write(*bytePtr++);                                             //                                  //
        }                                                                     //                                  //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        return (!_TransmissionStatus);                                        // return error status              //
      } // of method writeRAM()                                               //----------------------------------//
      
    private:                                                                  // Private methods                  //
      uint8_t  readByte(const uint8_t addr);                                  // Read 1 byte from address on I2C  //
      void     writeByte(const uint8_t addr, const uint8_t data);             // Write 1 byte at address to I2C   //
      uint8_t  bcd2int(const uint8_t bcd);                                    // convert BCD digits to integer    //
      uint8_t  int2bcd(const uint8_t dec);                                    // convert integer to BCD           //
      uint8_t  _TransmissionStatus = 0;                                       // Status of I2C transmission       //
      bool     _CrystalStatus     = false;                                    // True if RTC is turned on         //
      bool     _OscillatorStatus  = false;                                    // True if Oscillator on and working//
      uint32_t _SetUnixTime       = 0;                                        // UNIX time when clock last set    //
      uint8_t  _ss,_mm,_hh,_d,_m;                                             // Define date components           //
      uint16_t _y;                                                            // Define date components           //
      void     clearRegisterBit(uint8_t reg, uint8_t b);                      // Clear specific bit in register   //
      void     setRegisterBit(uint8_t reg, uint8_t b);                        // Set specific bit in register     //
      void     writeRegisterBit(uint8_t reg, uint8_t b, bool bitvalue);       // Write specific bit in register to bitvalue//
      uint8_t  readRegisterBit(uint8_t reg, uint8_t b);                       // Read specific bit from register  //
  }; // of MCP7940 class definition                                           //                                  //
#endif                                                                        //----------------------------------//

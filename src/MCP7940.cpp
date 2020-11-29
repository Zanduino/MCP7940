/*! @file MCP7940.cpp
 @section MCP7940cpp_intro_section Description

Arduino Library for the MCP7940M and MCP7940N Real-Time Clock devices\n\n
See main library header file for details
*/
#include "MCP7940.h"
/*! Define the number of days in each month */
const uint8_t   daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
  /*!
   * @brief   returns the number of days from a given Y M D value
   * @param[in] y Year
   * @param[in] m Month
   * @param[in] d Day
   * @return    number of days from a given Y M D value
   */
  if (y >= 2000) {
    y -= 2000;
  }  // of if-then year is greater than 2000
  uint16_t days = d;
  for (uint8_t i = 1; i < m; ++i)  // Add number of days for each month
  {
    days += pgm_read_byte(daysInMonth + i - 1);
  }                         // of for-next loop for each month
  if (m > 2 && y % 4 == 0)  // Deal with leap years
  {
    ++days;
  }                                          // of if-then a leap year
  if (((y % 100) == 0) && ((y % 400) != 0))  // if / by 100 but not by 400 then it is not a l.y.
  {
    --days;
  }                                         // of if-then special leap year
  return days + 365 * y + (y + 3) / 4 - 1;  // Return computed value
}  // of method date2days
static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
  /*!
   * @brief     return the number of seconds for a D H M S value
   * @param[in] days Days
   * @param[in] h Hours
   * @param[in] m Minutes
   * @param[in] m Seconds
   * @return    number of seconds for a Days/Hours/Minutes/Seconds value
   */
  return ((days * 24L + h) * 60 + m) * 60 + s;
}  // of method time2long
static uint8_t conv2d(const char* p) {
  /*!
   * @brief     convert character string representation to decimal
   * @param[in] p pointer to character array
   * @return    decimal value
   */
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9') {
    v = *p - '0';
  }  // of if-then character in range
  return 10 * v + *++p - '0';
}  // of method conv2d
DateTime::DateTime(uint32_t t) {
  /*!
   @brief   DateTime constructor (overloaded)
   @details Class Constructor for DateTime instantiates the class. This is an overloaded class
   constructor so there are multiple definitions. This implementation ignores time zones and DST
   changes. It also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second
   @param[in] t seconds since the year 1970 (UNIX timet) */
  t -= SECS_1970_TO_2000;  // bring to 2000 timestamp from 1970
  ss = t % 60;
  t /= 60;
  mm = t % 60;
  t /= 60;
  hh            = t % 24;
  uint16_t days = t / 24;
  uint8_t  leap;
  for (yOff = 0;; ++yOff) {
    leap = yOff % 4 == 0;
    if (days < (uint16_t)365 + leap) break;
    days -= 365 + leap;
  }  // of for-next each year
  for (m = 1;; ++m) {
    uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
    if (leap && m == 2) ++daysPerMonth;
    if (days < daysPerMonth) break;
    days -= daysPerMonth;
  }  // of for-next each month
  d = days + 1;
}  // of method DateTime()
DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min,
                   uint8_t sec) {
  /*!
   @brief   DateTime constructor (overloaded)
   @details Class Constructor for DateTime instantiates the class. This is an overloaded class
   constructor so there are multiple definitions. This implementation sets each component of the
   date/time separately
   @param[in] year Year
   @param[in] month Month
   @param[in] day Day
   @param[in] hour Hour
   @param[in] min Minute
   @param[in] sec Second */
  if (year >= 2000) {
    year -= 2000;
  }  // of if-then year is greater than 2000 for offset
  yOff = year;
  m    = month;
  d    = day;
  hh   = hour;
  mm   = min;
  ss   = sec;
}  // of method DateTime()
DateTime::DateTime(const DateTime& copy)
    : yOff(copy.yOff), m(copy.m), d(copy.d), hh(copy.hh), mm(copy.mm), ss(copy.ss) {
  /*!
  @brief   DateTime constructor (overloaded)
  @details Class Constructor for DateTime instantiates the class. This is an overloaded class
  constructor so there are multiple definitions. This implementation take a DateTime class value as
  the defining value
  @param[in] copy DateTime class */
}
DateTime::DateTime(const char* date, const char* time) {
  /*!
  @brief   DateTime constructor (overloaded)
  @details Class Constructor for DateTime instantiates the class. This is an overloaded class
  constructor so there are multiple definitions. This implementation uses a Date string formatted as
  "MMM DD YYYY" and a time string formatted as "hh:mm:ss" for the value
  @param[in] date Pointer to date string
  @param[in] time Pointer to time string */
  yOff = conv2d(date + 9);
  switch (date[0]) {
    case 'J':
      m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7);
      break;  // Jan June July
    case 'F':
      m = 2;
      break;  // February
    case 'A':
      m = date[2] == 'r' ? 4 : 8;
      break;  // April August
    case 'M':
      m = date[2] == 'r' ? 3 : 5;
      break;  // March May
    case 'S':
      m = 9;
      break;  // September
    case 'O':
      m = 10;
      break;  // October
    case 'N':
      m = 11;
      break;  // November
    case 'D':
      m = 12;
      break;              // December
  }                       // of switch for the month
  d  = conv2d(date + 4);  // Compute the day
  hh = conv2d(time);
  mm = conv2d(time + 3);
  ss = conv2d(time + 6);
}  // of method DateTime()
DateTime::DateTime(const __FlashStringHelper* date, const __FlashStringHelper* time) {
  /*!
   @brief   DateTime constructor (overloaded)
   @details Class Constructor for DateTime instantiates the class. This is an overloaded class
   constructor so there are multiple definitions. This implementation uses the compile date and
   time as the value to set. It only works when the compiler runs in English. This implementation
   uses a Date string formatted as "MMM DD YYYY" and a time string formatted as "hh:mm:ss" for the
   value
   @param[in] date Pointer to date string
   @param[in] time Pointer to time string */
  char date_buff[12];
  memcpy_P(date_buff, date, 12);
  char time_buff[8];
  memcpy_P(time_buff, time, 8);
  DateTime(date_buff, time_buff);  // Call actual DateTime constructor
}  // of method DateTime()
uint8_t DateTime::dayOfTheWeek() const {
  /*!
  @brief     return the current day-of-week where Monday is day 1, Sunday is 7
  @return    integer day-of-week 1-7 */
  uint16_t day = date2days(yOff, m, d);  // compute the number of days
  uint8_t  dow = ((day + 6) % 7);        // Jan 1, 2000 is a Saturday, i.e. 6
  if (dow == 0)                          // Correction for Sundays
  {
    dow = 7;
  }  // of if-then day should be Sunday
  return dow;
}  // of method dayOfTheWeek()
uint32_t DateTime::unixtime(void) const {
  /*!
  @brief     return the UNIX time, which is seconds since 1970-01-01 00:00:00
  @return    number of seconds since 1970-01-01 00:00:00 (Unixtime)
  */
  uint16_t days = date2days(yOff, m, d);        // Compute days
  uint32_t t    = time2long(days, hh, mm, ss);  // Compute seconds
  t += SECS_1970_TO_2000;                       // Add time form 1970 to 2000
  return t;
}  // of method unixtime()
long DateTime::secondstime(void) const {
  /*!
  @brief     return the time in seconds since the year 2000
  @return    number of seconds since 2000-01-01 00:00:00
  */
  uint16_t days = date2days(yOff, m, d);
  long     t    = time2long(days, hh, mm, ss);
  return t;
}  // of method secondstime()
DateTime DateTime::operator+(const TimeSpan& span) {
  /*!
  @brief     overloaded "+" operator for class DateTime
  @return    Sum of two DateTime class instances
  */
  return DateTime(unixtime() + span.totalseconds());
}  // of overloaded + function
DateTime DateTime::operator-(const TimeSpan& span) {
  /*!
  @brief     overloaded "+" operator for class DateTime
  @return    Sum of DateTime class and TimeSpan
  */
  return DateTime(unixtime() - span.totalseconds());
}  // of overloaded - function
TimeSpan DateTime::operator-(const DateTime& right) {
  /*!
  @brief     overloaded "-" operator for class DateTime
  @return    Difference of two DateTime class instances
  */
  return TimeSpan(unixtime() - right.unixtime());
}  // of overloaded - function
/***************************************************************************************************
** Implementation of TimeSpan                                                                     **
***************************************************************************************************/
TimeSpan::TimeSpan(int32_t seconds) : _seconds(seconds) {}
TimeSpan::TimeSpan(int16_t days, int8_t hours, int8_t minutes, int8_t seconds)
    : _seconds((int32_t)days * 86400L + (int32_t)hours * 3600 + (int32_t)minutes * 60 + seconds) {}
TimeSpan::TimeSpan(const TimeSpan& copy) : _seconds(copy._seconds) {}
TimeSpan TimeSpan::operator+(const TimeSpan& right) {
  return TimeSpan(_seconds + right._seconds);
}  // of overloaded add
TimeSpan TimeSpan::operator-(const TimeSpan& right) {
  return TimeSpan(_seconds - right._seconds);
}  // of overloaded subtract
bool MCP7940_Class::begin(const uint32_t i2cSpeed) const {
  /*!
      @brief     Start I2C device communications
      @details   Starts I2C comms with the device, using a default speed if one is not specified
      @param[in] i2cSpeed defaults to I2C_STANDARD_MODE, otherwise use speed in Herz
      @return    true if successfully started communication, otherwise false
  */
  Wire.begin();                             // Start I2C as master device
  Wire.setClock(i2cSpeed);                  // Set the I2C bus speed
  Wire.beginTransmission(MCP7940_ADDRESS);  // Address the MCP7940
  if (Wire.endTransmission() == 0)          // If there a device present
  {
    clearRegisterBit(MCP7940_RTCHOUR, MCP7940_12_24);  // Use 24 hour clock
    setRegisterBit(MCP7940_CONTROL, MCP7940_ALMPOL);   // assert alarm low, default high
    return true;                                       // return success
  } else {
    return false;  // return error if no device found
  }                // of if-then-else device detected
}  // of method begin()
uint8_t MCP7940_Class::readByte(const uint8_t addr) const {
  /*!
      @brief     Read a single byte from the device address
      @param[in] addr I2C device register address to read from
      @return    Byte read from the I2C device
  */
  uint8_t dataByte{0};
  I2C_read(addr, dataByte);
  return dataByte;
}  // of method readByte()
void MCP7940_Class::clearRegisterBit(const uint8_t reg, const uint8_t b) const {
  /*!
      @brief     clears a specified bit in a register on the device
      @param[in] reg Register to write to
      @param[in] b   Bit (0-7) to clear
  */
  //  writeByte(reg, readByte(reg) & ~(1 << b));
  I2C_write(reg, (uint8_t)(readByte(reg) & ~(1 << b)));
}  // of method clearRegisterBit()
void MCP7940_Class::setRegisterBit(const uint8_t reg, const uint8_t b) const {
  /*!
      @brief     sets a specified bit in a register on the device
      @param[in] reg Register to write to
      @param[in] b   Bit (0-7) to set
  */
  I2C_write(reg, (uint8_t)(readByte(reg) | (1 << b)));
}  // of method setRegisterBit()
void MCP7940_Class::writeRegisterBit(const uint8_t reg, const uint8_t b,
                                     const bool bitvalue) const {
  /*!
      @brief     Sets or clears the specified bit based on bitvalue
      @param[in] reg      Register to write to
      @param[in] b        Bit (0-7) to set
      @param[in] bitvalue boolean with "true" for 1 and "false" for 0
  */
  bitvalue ? setRegisterBit(reg, b) : clearRegisterBit(reg, b);
}  // of method writeRegisterBit()
uint8_t MCP7940_Class::readRegisterBit(const uint8_t reg, const uint8_t b) const {
  /*!
      @brief     read a specific bit from a register
      @param[in] reg      Register to read from
      @param[in] b        Bit (0-7) to read
      @return    0 for "false" and 1 for "true"
   */
  return bitRead(readByte(reg), b);
}  // of method readRegisterBit()
uint8_t MCP7940_Class::bcd2int(const uint8_t bcd) const {
  /*!
      @brief     converts a BCD encoded value into number representation
      @param[in] bcd Binary-Encoded-Decimal value
      @return    integer representation of BCD value
   */
  return ((bcd / 16 * 10) + (bcd % 16));
}  // of method bcd2int
uint8_t MCP7940_Class::int2bcd(const uint8_t dec) const {
  /*!
      @brief     converts an integer to a BCD encoded value
      @param[in] dec Integer value
      @return    BCD representation
   */
  return ((dec / 10 * 16) + (dec % 10));
}  // of method int2bcd
bool MCP7940_Class::deviceStatus() const {
  /*!
      @brief  checks to see if the MCP7940 crystal has been turned on or off
      @brief  Sets the status register to turn on the device clock
      @return Success status true if the crystal is on, otherwise false if off
   */
  return readRegisterBit(MCP7940_RTCSEC, MCP7940_ST);
}  // of method DeviceStatus
bool MCP7940_Class::deviceStart() const {
  /*!
      @brief  Start the MCP7940 device
      @details Sets the status register to turn on the device clock
      @return Success status true if successful otherwise false
   */
  uint8_t oscillatorStatus{0};                 // define temporary variable
  setRegisterBit(MCP7940_RTCSEC, MCP7940_ST);  // Set the ST bit
  for (uint8_t j = 0; j < 255; j++)            // Loop until changed or overflow
  {
    oscillatorStatus =
        readRegisterBit(MCP7940_RTCWKDAY, MCP7940_OSCRUN);  // Wait for oscillator to start
    if (oscillatorStatus) break;                            // Exit loop on success
    delay(1);                                               // Allow oscillator time to start
  }                                                         // of for-next oscillator loop
  return oscillatorStatus;
}  // of method deviceStart
bool MCP7940_Class::deviceStop() const {
  /*!
      @brief  Stop the MCP7940 device
      @details Sets the status register to turn off the device clock
      @return true if the oscillator is still running, otherwise 0 if the oscillator has stopped
   */
  clearRegisterBit(MCP7940_RTCSEC, MCP7940_ST);  // clear the ST bit.
  uint8_t oscillatorStatus{0};                   // temporary status variable
  for (uint8_t j = 0; j < 255; j++) {            // Loop until changed or overflow
    oscillatorStatus =
        readRegisterBit(MCP7940_RTCWKDAY, MCP7940_OSCRUN);  // Wait for oscillator to stop
    if (!oscillatorStatus) break;                           // Exit loop on success
    delay(1);                                               // Allow oscillator time to stop
  }                                                         // of for-next oscillator loop
  return oscillatorStatus;
}  // of method deviceStop
DateTime MCP7940_Class::now() const {
  /*!
      @brief   returns the current date/time
      @details If the device is stopped then the stop time is returned
      @return  DateTime class value for the current Date/Time
   */
  uint8_t readBuffer[7] = {0};
  I2C_read(MCP7940_RTCSEC, readBuffer);
  return DateTime(bcd2int(readBuffer[6]) + 2000, bcd2int(readBuffer[5] & 0x1F),
                  bcd2int(readBuffer[4] & 0x3F), bcd2int(readBuffer[2] & 0x3F),
                  bcd2int(readBuffer[1] & 0x7F), bcd2int(readBuffer[0] & 0x7F));
}  // of method now
DateTime MCP7940_Class::getPowerDown() const {
  /*!
      @brief   returns the date/time that the power went off
      @details This is set back to zero once the power fail flag is reset.
      @return  DateTime class value for the current Date/Time
   */
  uint8_t min{0}, hr{0}, day{0}, mon{0};  // temporary storage set to 0
  uint8_t readBuffer[4];
  if (I2C_read(MCP7940_PWRDNMIN, readBuffer) > 0) {
    min = bcd2int(readBuffer[0] & 0x7F);     // Clear high bit in minutes
    hr  = bcd2int(readBuffer[1] & 0x3F);     // Clear all but 6 LSBs
    day = bcd2int(readBuffer[2] & 0x3F);     // Clear 2 high bits for day-of-month
    mon = bcd2int(readBuffer[3] & 0x1F);     // Clear 3 high bits for Month
  }                                          // if-then success
  return DateTime(0, mon, day, hr, min, 0);  // Return class value
}  // of method getPowerDown()
DateTime MCP7940_Class::getPowerUp() const {
  /*!
      @brief   returns the date/time that the power went back on
      @return  DateTime class value for the power-on Date/Time
   */
  uint8_t min{0}, hr{0}, day{0}, mon{0};  // temporary storage set to 0
  uint8_t readBuffer[4];
  if (I2C_read(MCP7940_PWRUPMIN, readBuffer) > 0) {
    min = bcd2int(readBuffer[0] & 0x7F);     // Clear high bit in minutes
    hr  = bcd2int(readBuffer[1] & 0x3F);     // Clear all but 6 LSBs
    day = bcd2int(readBuffer[2] & 0x3F);     // Clear 2 high bits for day-of-month
    mon = bcd2int(readBuffer[3] & 0x1F);     // Clear 3 high bits for Month
  }                                          // if-then success
  return DateTime(0, mon, day, hr, min, 0);  // Return class value
}  // of method getPowerUp()
void MCP7940_Class::adjust() {
  /*!
      @brief   sets the current date/time (overloaded)
      @details This is an overloaded function. With no parameters then the RTC is set to the
     date/time when the program was compiled. Since different core version handle these compiler
     variables differently, the values are copied to temporary strings and then passed to the
     adjust() function
  */
  char tempDate[13], tempTime[13];     // define temporary character arrays to hold values
  strcpy_P(tempDate, PSTR(__DATE__));  // Copy values to buffer
  strcpy_P(tempTime, PSTR(__TIME__));  // Copy values to buffer
  DateTime tempDt = DateTime(tempDate, tempTime);  // Create a datetime
  adjust(tempDt);                                  // Set to library compile Date/Time //
}  // of method "adjust()"
void MCP7940_Class::adjust(const DateTime& dt) {
  /*!
     @brief   sets the current date/time (overloaded)
     @details This is an overloaded function. Set to the DateTime class instance value. The
     oscillator is stopped during the process and is restarted upon completion.
  */
  deviceStop();  // Stop the oscillator
  I2C_write(MCP7940_RTCSEC, int2bcd(dt.second()));
  I2C_write(MCP7940_RTCMIN, int2bcd(dt.minute()));
  I2C_write(MCP7940_RTCHOUR, int2bcd(dt.hour()));
  weekdayWrite(dt.dayOfTheWeek());                        // Update the weekday
  I2C_write(MCP7940_RTCDATE, int2bcd(dt.day()));          // Write the day of month
  I2C_write(MCP7940_RTCMTH, int2bcd(dt.month()));         // Month, ignore R/O leapyear bit
  I2C_write(MCP7940_RTCYEAR, int2bcd(dt.year() - 2000));  // Write the year
  deviceStart();                                          // Restart the oscillator
  _SetUnixTime = dt.unixtime();                           // Store time of last change
}  // of method adjust
uint8_t MCP7940_Class::weekdayRead() const {
  /*!
      @brief   return the weekday number from the RTC
      @details This number is user-settable and is incremented when the day shifts. It is set as
     part of the adjust() method where Monday is weekday 1
      @return  Values 1-7 for the current weekday
  */
  return readByte(MCP7940_RTCWKDAY) & 0x07;  // no need to convert, values 1-7
}  // of method weekdayRead()
uint8_t MCP7940_Class::weekdayWrite(const uint8_t dow) const {
  /*!
      @brief   set the RTC weekday number
      @details This number is user-settable and is incremented when the day shifts. The library uses
     1 for Monday and 7 for Sunday
      @param[in] dow Day of week (1-7)
      @return    Values 1-7 for the day set, returns MCP7940 value if "dow" is out of range
  */
  uint8_t retval = (readByte(MCP7940_RTCWKDAY) & B11111000) | dow;  // Read, mask DOW bits & add DOW
  if (dow > 0 && dow < 8)  // If parameter is in range, then
  {
    I2C_write(MCP7940_RTCWKDAY, retval);  // Write the register
    retval = dow;                         // set the return value
  }                                       // of if-then we have a good DOW
  return retval;
}  // of method weekdayWrite()
int8_t MCP7940_Class::calibrate() const {
  /*!
      @brief   Calibrate the MCP7940 (overloaded)
      @details When called with no parameters the internal calibration is reset to 0
      @return  Always returns 0
  */
  clearRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);  // fine trim mode on, to be safe
  I2C_write(MCP7940_OSCTRIM, (uint8_t)0);              // Write zeros to the trim register
  return (0);
}  // of method calibrate()
int8_t MCP7940_Class::calibrate(const int8_t newTrim) {
  /*!
      @brief   Calibrate the MCP7940 (overloaded)
      @details When called with one int8 parameter that value is used as the new trim value
      @param[in] newTrim New signed integer value to use for the trim register
      @return  Returns the input "newTrim" value
  */
  int8_t trim = abs(newTrim);  // Make a local copy of absolute value
  if (newTrim < 0)             // if the trim is less than 0
  {
    trim = 0x80 | trim;                                // set non-excess 128 negative val
  }                                                    // of if-then value of trim is less than 0
  clearRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);  // fine trim mode on, to be safe    //
  I2C_write(MCP7940_OSCTRIM, trim);                    // Write value to the trim register //
  _SetUnixTime = now().unixtime();                     // Store time of last change        //
  return newTrim;
}  // of method calibrate()
int8_t MCP7940_Class::calibrate(const DateTime& dt) {
  /*!
     @brief   Calibrate the MCP7940 (overloaded)
     @details When called with a DateTime class value then an internal calibration is performed.
     Accepts a current date/time value and compares that to the current date/time of the RTC and
     computes a calibration factor depending upon the time difference between the two and how long
     the timespan between the two is. The longer the period between setting the clock and comparing
     the difference between real time and indicated time the better the resulting calibration
     accuracy will be. The datasheet explains the calibration formula on page 28. First, the error
     in parts-per-million is computed using PPM = (SecDev/ExpectedSeconds) * 10000000. Then the trim
     register is computed using the formula TRIMVAL = (PPM*32768*60)/(1000000*2). The code below is
     designed for easy reading, not for performance. The ppm <> 130 is computed by solving the
     equation for the maximum TRIM value of 127 and computing the corresponding max and min values
     for ppm. This allows the trim variable to be one byte rather than a long integer.
     @param[in] dt Actual Date/time
     @return  Returns the new calculated trim value
  */
  int32_t ppm = getPPMDeviation(dt);
  adjust(dt);  // set the new Date-Time value
  ppm          = constrain(ppm, -130, 130);
  int16_t trim = readByte(MCP7940_OSCTRIM);  // Read current trim register value
  if (trim >> 7)                             // use negative value if necessary
  {
    trim = (~0x80 & trim) * -1;
  }                                    // of if-then trim is set
  trim += ppm * 32768 * 60 / 2000000;  // compute the new trim value
  trim = constrain(trim, -127, 127);   // Clamp to value range
  return calibrate((const int8_t)trim);
}  // of method calibrate()
int8_t MCP7940_Class::calibrate(const float fMeas) const {
  /*!
      @brief   Calibrate the MCP7940 (overloaded)
      @details When called with one floating point value then that is used as the measured frequency
      @param[in] fMeas Measured frequency in Herz
      @return  Returns the new trim value
  */
  int16_t  trim   = getCalibrationTrim();  // Get the current trim
  uint32_t fIdeal = getSQWSpeed();         // read the current SQW Speed code
  switch (fIdeal)                          // set variable to real SQW speed
  {
    case 0:
      fIdeal = 1;
      break;
    case 1:
      fIdeal = 4096;
      break;
    case 2:
      fIdeal = 8192;
      break;
    case 4:
      fIdeal = 64;
      break;
    case 3:
      fIdeal = 32768;
      trim   = 0;  // Trim is ignored on 32KHz signal
      break;
  }  // of switch SQWSpeed value
  trim +=
      ((fMeas - (float)fIdeal) * (32768.0 / fIdeal) * 60.0) / 2.0;  // Use formula from datasheet
  if (trim > 127)  // Force number ppm to be in range
  {
    trim = 127;
  } else if (trim < -127) {
    trim = -127;
  }                                             // of if-then-else trim out of range
  int8_t returnTrim = calibrate((int8_t)trim);  // Set the new trim value
  return (returnTrim);
}  // of method calibrate()
int8_t MCP7940_Class::calibrateOrAdjust(const DateTime& dt) {
  /*!
    @brief   Calibrate the MCP7940 if the ppm deviation is < 130 and > -130 else Adjust the
    datetime.
    @details If the time had changed significantly (like happens during daylight savings time) then
    just adjust the time to be the new time and leave the trim value alone.  One hour deviation in 6
    months is 225 ppm.  If the ppm deviation is within -130 to 130 then assume we are just
    calibrating the clock.
    @param[in] dt Actual Date/time
    @return  Returns the trim value
   */
  int32_t ppm = getPPMDeviation(dt);
  if ((ppm > 130) ||
      (ppm < -130)) {  // calibration is out of range so just set the time  DML 2/5/2019
    adjust(dt);
    return readByte(MCP7940_OSCTRIM);
  } else {
    return calibrate(dt);
  }  // if-then-else ppm out of range
}  // method calibrateOrAdjust()
int32_t MCP7940_Class::getPPMDeviation(const DateTime& dt) const {
  /*!
      @brief   Calculate the ppm deviation since the last time the clock was set
      @details The internal _SetUniTime value is set when the clock is set. On this call the current
     time is converted to unitime and compared with the stored value to determine the
     part-per-million difference.  One hour deviation in 6 months is 225 ppm
      @param[in] dt Actual Date/time
      @return  difference in ppm
  */
  int32_t SecDeviation = dt.unixtime() - now().unixtime();  // Get difference in seconds
  int32_t ExpectedSec  = dt.unixtime() - _SetUnixTime;      // Get number of seconds since set
  int32_t ppm = 1000000 * SecDeviation / ExpectedSec;       // Multiply first to avoid truncation
  return ppm;
}
void MCP7940_Class::setSetUnixTime(uint32_t aTime) {
  /*!
      @brief   Set the time the clock was last calibrated or adjusted.
      @details Set the internal variable _SetUnixTime.  This is the time the clock was last
     calibrated or adjusted.  This should only used to testing.
      @param[in] aTime is the Unix time to set the time the clock was last calibrated or adjusted
     to.
      @return  void
  */
  _SetUnixTime = aTime;
}
uint32_t MCP7940_Class::getSetUnixTime() const {
  /*!
    @brief   Get the time the clock was last calibrated or adjusted.
    @details Returns the internal variable _SetUnixTime.  This is the time the clock was last
    calibrated or adjusted.  This should only used to testing.
    @return  Returns the internal variable _SetUnixTime.
  */
  return _SetUnixTime;
}
int8_t MCP7940_Class::getCalibrationTrim() const {
  /*!
      @brief   Return the TRIMVAL trim value
      @details Since the number in the register can be negative but is not in excess-128 format any
     negative numbers need to be manipulated before returning
      @return  Returns the current trim value
  */
  uint8_t trim = readByte(MCP7940_OSCTRIM);  // read the register
  if (trim >> 7) {                           // If trim is negative, the convert
    trim = (0x7F & trim) * -1;               // convert to excess128 value
  }                                          // of if-then less than zero trim
  return ((int8_t)trim);
}  // of method getCalibrationTrim()
bool MCP7940_Class::setMFP(const bool value) const {
  /*!
      @brief   Sets the MFP (Multifunction Pin) to the requested state
      @return  Returns true on success otherwise false
  */
  if ((readByte(MCP7940_CONTROL) & 0x70) !=
      0)  // Get Control register, error if SQWEN/ALM1EN/ALM0EN set
  {
    return false;
  }  // of if-then an invalid value
  writeRegisterBit(MCP7940_CONTROL, MCP7940_OUT, value);
  return true;
}  // of method setMFP()
uint8_t MCP7940_Class::getMFP() const {
  /*!
      @brief   Gets the MFP (Multifunction Pin) value
      @details On is true and Off is false. This is read from the control register if no alarms are
     enabled, otherwise the two alarm states must be checked.
      @return  Returns one of the following values;\n
               0 = pin set LOW.\n
               1 = pin set HIGH.\n
               2 = pin controlled by alarms.\n
               3 = pin controlled by square wave output.\n
  */
  uint8_t controlRegister = readByte(MCP7940_CONTROL);  // Get control register contents
  if (controlRegister & (1 << MCP7940_SQWEN))           // Square wave output enabled
  {
    return 3;  // MFP in SQW output mode
  } else if ((controlRegister & (1 << MCP7940_ALM0EN)) |
             (controlRegister &
              (1 << MCP7940_ALM1EN))) {  // One or both alarms enabled, and no square wave output.
    return 2;                            // MFP in alarm output mode
  }                                      // of if-then-else square wave enabled
  return bitRead(controlRegister, MCP7940_OUT);  // MFP in manual mode, return value
}  // of method getMFP()
bool MCP7940_Class::setAlarm(const uint8_t alarmNumber, const uint8_t alarmType, const DateTime& dt,
                             const bool state) const {
  /*!
      @brief   Sets one of the 2 alarms
      @details In order to configure the alarm modules, the following steps need to be performed in
     order:\n
               1. Load the timekeeping registers and enable the oscillator\n
               2. Configure the ALMxMSK<2:0> bits to select the desired alarm mask\n
               3. Set or clear the ALMPOL bit according to the desired output polarity\n
               4. Ensure the ALMxIF flag is cleared\n
               5. Based on the selected alarm mask, load the alarm match value into the appropriate
     register(s)\n
               6. Enable the alarm module by setting the ALMxEN bit\n
               There are two ALMPOL bits - one in the ALM0WKDAY register which can be written, one
     in the ALM1WKDAY register which is read-only and reflects the value of the ALMPOL in ALM0WKDAY.
      @param[in] alarmNumber Alarm 0 or Alarm 1
      @param[in] alarmType   Alarm type from 0 to 7, see detailed description
      @param[in] dt          DateTime alarm value used to set the alarm
      @param[in] state       Alarm state to set to (0 for "off" and 1 for "on")
      @return  Returns true for success otherwise false
  */
  if (alarmNumber < 2 && alarmType < 8 && alarmType != 5 && alarmType != 6 &&
      deviceStart()) {  // if parameters and oscillator OK
    clearRegisterBit(MCP7940_CONTROL,
                     alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN);  // Turn off the alarm
    uint8_t offset        = 7 * alarmNumber;                          // Offset to be applied
    uint8_t wkdayRegister = readByte(MCP7940_ALM0WKDAY + offset);     // Load register to memory
    wkdayRegister &=
        ((1 << MCP7940_ALM0IF) | (1 << MCP7940_ALMPOL));        // Keep ALMPOL and ALMxIF bits
    wkdayRegister |= alarmType << 4;                            // Set 3 bits from alarmType
    wkdayRegister |= (dt.dayOfTheWeek() & 0x07);                // Set 3 bits for dow from date
    I2C_write(MCP7940_ALM0WKDAY + offset, wkdayRegister);       // Write alarm mask
    I2C_write(MCP7940_ALM0SEC + offset, int2bcd(dt.second()));  // Write seconds, keep device off
    I2C_write(MCP7940_ALM0MIN + offset, int2bcd(dt.minute()));  // Write the minutes value
    I2C_write(MCP7940_ALM0HOUR + offset, int2bcd(dt.hour()));   // Also re-sets the 24Hour clock on
    I2C_write(MCP7940_ALM0DATE + offset, int2bcd(dt.day()));    // Write the day of month
    I2C_write(MCP7940_ALM0MTH + offset, int2bcd(dt.month()));   // Month, ignore R/O leap-year bit
    setAlarmState(alarmNumber, state);                          // Set the requested alarm to state
    return true;
  }              // of if-then alarmNumber and alarmType are valid and device running
  return false;  // error if we get here
}  // of method setAlarm()
void MCP7940_Class::setAlarmPolarity(const bool polarity) const {
  /*!
      @brief   Sets the alarm polarity
      @details Alarm polarity (see also TABLE 5-10 on p.27 of the datasheet). Note: the MFP pin is
     open collector, it needs an external pull-up resistor.\n If only one alarm is set:\n polarity =
     0: MFP high when no alarm, low when alarm.\n polarity = 1: MFP low when no alarm, high when
     alarm.\n\n If both alarms are set:\n polarity = 0: MFP high when no alarm or one alarm, low
     when both alarms go off.\n polarity = 1: MFP low when no alarm, high when eihter or both alarms
     go off.\n\n In most situations you will want to set polarity to 1 if you have two alarms set,
     to be able to see when an alarm goes off using the MFP pin.
      @param[in] polarity Boolean value to for polarity
  */
  writeRegisterBit(MCP7940_ALM0WKDAY, MCP7940_ALMPOL,
                   polarity);  // Write polarity to the ALMPOL bit
  return;
}  // of method setAlarmPolarity()
DateTime MCP7940_Class::getAlarm(const uint8_t alarmNumber, uint8_t& alarmType) const {
  /*!
      @brief   Gets the DateTime for the given alarm
      @details update the alarmType parameter with the alarm type that was set
      @param[in] alarmNumber Alarm number 0 or 1
      @param[out] alarmType See detailed description for list of alarm types 0-7
      @return DateTime value of alarm
  */
  if (alarmNumber > 1)  // return an error if bad alarm number
  {
    return DateTime(0);
  }                                  // of if-then bad alarm number
  uint8_t offset = 7 * alarmNumber;  // Offset to be applied
  alarmType      = (readByte(MCP7940_ALM0WKDAY + offset) >> 4) & B111;  // get 3 bits for alarmType
  uint8_t ss     = bcd2int(readByte(MCP7940_ALM0SEC + offset) & 0x7F);  // Clear high bit in seconds
  uint8_t mm     = bcd2int(readByte(MCP7940_ALM0MIN + offset) & 0x7F);  // Clear high bit in minutes
  uint8_t hh     = bcd2int(readByte(MCP7940_ALM0HOUR + offset) & 0x3F);  // Clear high bits in hours
  uint8_t d =
      bcd2int(readByte(MCP7940_ALM0DATE + offset) & 0x3F);  // Clear 2 high bits for day-of-month
  uint8_t  m = bcd2int(readByte(MCP7940_ALM0MTH + offset) & 0x1F);  // Clear 3 high bits for Month
  uint16_t y = 0;  // Year is not part of the alarms
  return DateTime(y, m, d, hh, mm, ss);
}  // of method getAlarm()
bool MCP7940_Class::clearAlarm(const uint8_t alarmNumber) const {
  /*!
      @brief   Clears the given alarm
      @param[in] alarmNumber Alarm number 0 or 1
      @return False if the alarmNumber is out of range, otherwise true
  */
  if (alarmNumber > 1) {
    return false;
  }  // of if-then a bad alarm number
  clearRegisterBit(alarmNumber ? MCP7940_ALM1WKDAY : MCP7940_ALM0WKDAY,
                   MCP7940_ALM0IF);  // reset register bit
  return true;
}  // of method clearAlarm()
bool MCP7940_Class::setAlarmState(const uint8_t alarmNumber, const bool state) const {
  /*!
      @brief   Turns an alarm on or off without changing the alarm condition
      @param[in] alarmNumber Alarm number 0 or 1
      @param[in] state State to the set the alarm to
      @return False if the alarmNumber is out of range, otherwise true
  */
  if (alarmNumber > 1) {
    return false;
  }  // of if-then a bad alarm number
  writeRegisterBit(MCP7940_CONTROL, alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN,
                   state);  // Overwrite register bit
  return true;
}  // of setAlarmState()
bool MCP7940_Class::getAlarmState(const uint8_t alarmNumber) const {
  /*!
      @brief   Return whether a given alarm is on or off
      @param[in] alarmNumber Alarm number 0 or 1
      @return False if the alarmNumber is out of range or off, otherwise true
  */
  if (alarmNumber > 1) {
    return false;
  }  // of if-then a bad alarm number
  return readRegisterBit(MCP7940_CONTROL,
                         alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN);  // Get state of alarm
}  // of getAlarmState()
bool MCP7940_Class::isAlarm(const uint8_t alarmNumber) const {
  /*!
      @brief   Return whether a given alarm is active or not
      @param[in] alarmNumber Alarm number 0 or 1
      @return False if the alarmNumber is out of range or off, otherwise true
  */
  if (alarmNumber > 1) {
    return false;
  }  // of if-then a bad alarm number
  return readRegisterBit(alarmNumber ? MCP7940_ALM1WKDAY : MCP7940_ALM0WKDAY,
                         MCP7940_ALM0IF);  // Get alarm state
}  // of method isAlarm()
uint8_t MCP7940_Class::getSQWSpeed() const {
  /*!
      @brief  returns the list value for the frequency of the square wave
      @return Integer value index for the square wave frequency:\n
      0 = 1Hz\n
      1 = 4.096KHz\n
      2 = 8.192KHz\n
      3 32.768KHz\n
      If square wave is not turned on then a 0 is returned
  */
  uint8_t frequency = readByte(MCP7940_CONTROL);  // Read the control register
  if (frequency & 0x40) {                         // return 2 bits if SQW enabled
    return (frequency & 0x03);
  }  // of if-then square wave frequency set
  else
    return 0;
}  // of method getSQWSpeed()
bool MCP7940_Class::setSQWSpeed(uint8_t frequency, bool state) const {
  /*!
      @brief  set the square wave speed to a value
      @param[in] frequency Integer value index for the square wave frequency:\n
                          0 = 1Hz\n
                          1 = 4.096KHz\n
                          2 = 8.192KHz\n
                          3 = 32.768KHz\n
                          4 = 64Hz\n
      @param[in] state Boolean value set to false if square wave is to be turned off, otherwise true
      @return boolean state of the square wave
  */
  if (frequency < 4)  // If the frequency is < 64Hz
  {
    uint8_t registerValue = readByte(MCP7940_CONTROL);  // read the register to a variable
    bitWrite(registerValue, MCP7940_SQWEN, state);
    bitWrite(registerValue, MCP7940_SQWFS0, bitRead(frequency, 0));
    bitWrite(registerValue, MCP7940_SQWFS1, bitRead(frequency, 1));
    clearRegisterBit(registerValue, MCP7940_CRSTRIM);  // CRSTRIM bit must be cleared
    I2C_write(MCP7940_CONTROL, registerValue);         // Write register settings
  } else if (frequency == 4)                           // If the frequency is 64Hz
  {
    setRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);         // CRSTRIM bit must be set for 64Hz
    writeRegisterBit(MCP7940_CONTROL, MCP7940_SQWEN, state);  // Set then square wave enable bit
  }  // of if-then-else less then 64Hz or equal
  return state;
}  // of method setSQWState()
bool MCP7940_Class::setSQWState(const bool state) const {
  /*!
      @brief  set the square wave generator on or off
      @param[in] state Boolean value set to false if square wave is to be turned off, otherwise true
      @return boolean state of the square wave
  */
  writeRegisterBit(MCP7940_CONTROL, MCP7940_SQWEN, state);  // set the one bit to state
  return state;
}  // of method setSQWState
bool MCP7940_Class::getSQWState() const {
  /*!
      @brief  Get the square wave generator state
      @return boolean state of the square wave, "true" for on and "false" for off
  */
  return readRegisterBit(MCP7940_CONTROL, MCP7940_SQWEN);
}  // of method getSQWState()
bool MCP7940_Class::setBattery(const bool state) const {
  /*!
      @brief     Enable or disable the battery backup
      @details   Has no effect on the MCP7940M, only on the MCP7940N
      @param[in] state True for "on", False for "off"
      @return    boolean state of the battery backup. "true" for on and "false" for off
  */
  writeRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN, state);
  return (state);
}  // of method setBattery()
bool MCP7940_Class::getBattery() const {
  /*!
      @brief     Return the battery backup state
      @details   Has no effect on the MCP7940M, only on the MCP7940N
      @return    boolean state of the battery backup mode. "true" for on and "false" for off
  */
  return readRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN);
}  // of method setBattery()
bool MCP7940_Class::getPowerFail() const {
  /*!
      @brief     Return the power failure status
      @return    boolean state of the power failure status. "true" if a power failure has occured,
     otherwise "false"
  */
  bool status = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_PWRFAIL);
  return status;
}  // of method getPowerFail()
bool MCP7940_Class::clearPowerFail() const {
  /*!
      @brief     Clears the power failure status flag
      @return    Always returns true
  */
  I2C_write(MCP7940_RTCWKDAY, readByte(MCP7940_RTCWKDAY));
  return true;
}  // of method clearPowerFail()

/*******************************************************************************************************************
** MCP7940 class method definitions. See the header file for program details and version information              **
**                                                                                                                **
** GNU General Public License v3.0                                                                                **
** ===============================                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
*******************************************************************************************************************/
#include "MCP7940.h"                                                          // Include the header definition    //
                                                                              //                                  //
const uint8_t daysInMonth [] PROGMEM = {31,28,31,30,31,30,31,31,30,31,30,31}; // Numbers of days in each month    //

/*******************************************************************************************************************
** function date2days returns the number of days from a given Y M D value                                         **
*******************************************************************************************************************/
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {                 //                                  //
  if (y >= 2000) {                                                            // Remove year offset               //
    y -= 2000;                                                                //                                  //
  } // of if-then year is greater than 2000                                   //                                  //
  uint16_t days = d;                                                          // Store numbers of days            //
  for (uint8_t i = 1; i < m; ++i) {                                           // Add number of days for each month//
    days += pgm_read_byte(daysInMonth + i - 1);                               //                                  //
  } // of for-next loop for each month                                        //                                  //
  if (m > 2 && y % 4 == 0) {                                                  // Deal with leap years             //
    ++days;                                                                   //                                  //
  } // of if-then a leap year                                                 //                                  //
  if ( ((y % 100) == 0) && ((y % 400) !=0 )) {                                // if year is divisible by 100 but  //
    --days;                                                                   // not by 400 then it is not a l.y. //
  } // of if-then special leap year                                           //                                  //
  return days + 365 * y + (y + 3) / 4 - 1;                                    // Return computed value            //
} // of method date2days                                                      //                                  //

/*******************************************************************************************************************
** function time2long return the number of seconds for a D H M S value                                            **
*******************************************************************************************************************/
static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {       //                                  //
  return ((days * 24L + h) * 60 + m) * 60 + s;                                //                                  //
} // of method time2long                                                      //                                  //

/*******************************************************************************************************************
** Function con2d() converts character string representation to decimal                                           **
*******************************************************************************************************************/
static uint8_t conv2d(const char* p) {                                        //                                  //
  uint8_t v = 0;                                                              //                                  //
  if ('0' <= *p && *p <= '9') {                                               //                                  //
    v = *p - '0';                                                             //                                  //
  } // of if-then character in range                                          //                                  //
  return 10 * v + *++p - '0';                                                 //                                  //
} // of method conv2d                                                         //                                  //

/*******************************************************************************************************************
** Class Constructor for DateTime instantiates the class. This is an overloaded class constructor so there are    **
** multiple definitions. This implementation ignores time zones and DST changes. It also ignores leap seconds, see**
** http://en.wikipedia.org/wiki/Leap_second                                                                       **
*******************************************************************************************************************/
DateTime::DateTime (uint32_t t) {                                             //                                  //
  t -= SECONDS_FROM_1970_TO_2000;                                             // bring to 2000 timestamp from 1970//
  ss = t % 60;                                                                //                                  //
  t /= 60;                                                                    //                                  //
  mm = t % 60;                                                                //                                  //
  t /= 60;                                                                    //                                  //
  hh = t % 24;                                                                //                                  //
  uint16_t days = t / 24;                                                     //                                  //
  uint8_t leap;                                                               //                                  //
  for (yOff = 0; ; ++yOff) {                                                  //                                  //
    leap = yOff % 4 == 0;                                                     //                                  //
    if (days < (uint16_t)365 + leap)                                          //                                  //
      break;                                                                  //                                  //
    days -= 365 + leap;                                                       //                                  //
  } // of for-next each year                                                  //                                  //
  for (m = 1; ; ++m) {                                                        //                                  //
    uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);                //                                  //
    if (leap && m == 2)                                                       //                                  //
      ++daysPerMonth;                                                         //                                  //
    if (days < daysPerMonth)                                                  //                                  //
      break;                                                                  //                                  //
    days -= daysPerMonth;                                                     //                                  //
  } // of for-next each month                                                 //                                  //
  d = days + 1;                                                               //                                  //
} // of method DateTime()                                                     //----------------------------------//
DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour,  // Overloaded Definition            //
                    uint8_t min, uint8_t sec) {                               //                                  //
  if (year >= 2000) {                                                         //                                  //
    year -= 2000;                                                             //                                  //
  } // of if-then year is greater than 2000 for offset                        //                                  //
  yOff = year;                                                                //                                  //
  m    = month;                                                               //                                  //
  d    = day;                                                                 //                                  //
  hh   = hour;                                                                //                                  //
  mm   = min;                                                                 //                                  //
  ss   = sec;                                                                 //                                  //
} // of method DateTime()                                                     //----------------------------------//
DateTime::DateTime (const DateTime& copy):                                    // Overloaded Definition            //
  yOff(copy.yOff),                                                            //                                  //
  m(copy.m),                                                                  //                                  //
  d(copy.d),                                                                  //                                  //
  hh(copy.hh),                                                                //                                  //
  mm(copy.mm),                                                                //                                  //
  ss(copy.ss)                                                                 //                                  //
{} // of method DateTime()                                                    //                                  //

/*******************************************************************************************************************
** Constructor for DateTime getting a string date and a string time formatted as MMM DD YYYY and HH:NN:_SS        **
** N.B. This only works for English language output.                                                              **
*******************************************************************************************************************/
DateTime::DateTime (const char* date, const char* time) {                     // User compiler time to see RTC    //
  yOff = conv2d(date + 9);                                                    // Compute the year offset          //
  switch (date[0]) {                                                          // Switch for month detection       //
    case 'J': m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7); break;   // Jan Feb Mar Apr May Jun Jul Aug  //
    case 'F': m = 2; break;                                                   // Sep Oct Nov Dec                  //
    case 'A': m = date[2] == 'r' ? 4 : 8; break;                              //                                  //
    case 'M': m = date[2] == 'r' ? 3 : 5; break;                              //                                  //
    case 'S': m = 9; break;                                                   //                                  //
    case 'O': m = 10; break;                                                  //                                  //
    case 'N': m = 11; break;                                                  //                                  //
    case 'D': m = 12; break;                                                  //                                  //
  } // of switch for the month                                                //                                  //
  d  = conv2d(date + 4);                                                      // Compute the day                  //
  hh = conv2d(time    );                                                      //                                  //
  mm = conv2d(time + 3);                                                      //                                  //
  ss = conv2d(time + 6);                                                      //                                  //
} // of method DateTime()                                                     //                                  //
/*******************************************************************************************************************
** Constructor for using "the compiler's time": DateTime now (__DATE__, __TIME__); The compiler date and time     **
** arrive in string format as follows: date = "Dec 26 2009", time = "12:34:56". N.B. This only works for English  **
** language output.                                                                                               **
*******************************************************************************************************************/
DateTime::DateTime (const __FlashStringHelper* date,                          // Overloaded function call         //
                    const __FlashStringHelper* time) {                        //                                  //
  char date_buff[12];                                                         //                                  //
  memcpy_P(date_buff, date, 12);                                              //                                  //
  char time_buff[8];                                                          //                                  //
  memcpy_P(time_buff, time, 8);                                               //                                  //
  DateTime(date_buff, time_buff);                                             // Call actual DateTime constructor //
  yOff = conv2d(date_buff + 9);                                               // Compute the year offset          //
  switch (date_buff[0]) {                                                     // Switch for month detection       //
    case 'J': m =(date_buff[1]=='a')?1:((date_buff[2]=='n')?6:7); break;      // Jan Feb Mar Apr May Jun Jul Aug  //
    case 'F': m = 2; break;                                                   // Sep Oct Nov Dec                  //
    case 'A': m = date_buff[2] == 'r' ? 4 : 8; break;                         //                                  //
    case 'M': m = date_buff[2] == 'r' ? 3 : 5; break;                         //                                  //
    case 'S': m = 9; break;                                                   //                                  //
    case 'O': m = 10; break;                                                  //                                  //
    case 'N': m = 11; break;                                                  //                                  //
    case 'D': m = 12; break;                                                  //                                  //
  } // of switch for the month                                                //                                  //
  d  = conv2d(date_buff + 4);                                                 // Compute the day                  //
  hh = conv2d(time_buff    );                                                 //                                  //
  mm = conv2d(time_buff + 3);                                                 //                                  //
  ss = conv2d(time_buff + 6);                                                 //                                  //
} // of method DateTime()                                                     //                                  //

/*******************************************************************************************************************
** Function dayOfTheWeek() to return the day-of-week where Monday is day 1                                        **
*******************************************************************************************************************/
uint8_t DateTime::dayOfTheWeek() const {                                      // Compute the DOW                  //
  uint16_t day = date2days(yOff, m, d);                                       // compute the number of days       //
  uint8_t dow = ((day + 6) % 7);                                              // Jan 1, 2000 is a Saturday, i.e. 6//
  if (dow == 0) {                                                             // Correction for Sundays           //
    dow = 7;                                                                  //                                  //
  } // of if-then day should be Sunday                                        //                                  //
  return dow;                                                                 //                                  //
} // of method dayOfTheWeek()                                                 //                                  //

/*******************************************************************************************************************
** Function unixtime() to return the UNIX time, which is seconds since 1970-01-01 00:00:00                        **
*******************************************************************************************************************/
uint32_t DateTime::unixtime(void) const {                                     //                                  //
  uint32_t t;                                                                 // Declare return variable          //
  uint16_t days = date2days(yOff, m, d);                                      // Compute days                     //
  t = time2long(days, hh, mm, ss);                                            // Compute seconds                  //
  t += SECONDS_FROM_1970_TO_2000;                                             // Add seconds from 1970 to 2000    //
  return t;                                                                   //                                  //
} // of method unixtime()                                                     //                                  //

/*******************************************************************************************************************
** Function secondstime() to return the time, in seconds since 2000                                               **
*******************************************************************************************************************/
long DateTime::secondstime(void) const {                                      //                                  //
  long t;                                                                     //                                  //
  uint16_t days = date2days(yOff, m, d);                                      //                                  //
  t = time2long(days, hh, mm, ss);                                            //                                  //
  return t;                                                                   //                                  //
} // of method secondstime()                                                  //                                  //

/*******************************************************************************************************************
** Overloaded functions to allow math operations on the date/time                                                 **
*******************************************************************************************************************/
DateTime DateTime::operator + (const TimeSpan& span) {                        //                                  //
  return DateTime(unixtime() + span.totalseconds());                          //                                  //
} // of overloaded + function                                                 //                                  //
DateTime DateTime::operator - (const TimeSpan& span) {                        //                                  //
  return DateTime(unixtime() - span.totalseconds());                          //                                  //
} // of overloaded - function                                                 //                                  //
TimeSpan DateTime::operator - (const DateTime& right) {                       //                                  //
  return TimeSpan(unixtime() - right.unixtime());                             //                                  //
} // of overloaded - function                                                 //                                  //

/*******************************************************************************************************************
** Implementation of TimeSpan                                                                                     **
*******************************************************************************************************************/
TimeSpan::TimeSpan (int32_t seconds): _seconds(seconds) {}                    //                                  //
TimeSpan::TimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds): //                               //
  _seconds((int32_t)days * 86400L + (int32_t)hours * 3600 + (int32_t)minutes * 60 + seconds) {} //                //
TimeSpan::TimeSpan (const TimeSpan& copy): _seconds(copy._seconds) {}         //                                  //
TimeSpan TimeSpan::operator + (const TimeSpan& right) {                       //                                  //
  return TimeSpan(_seconds + right._seconds);                                 //                                  //
} // of overloaded add                                                        //                                  //
TimeSpan TimeSpan::operator - (const TimeSpan& right) {                       //                                  //
  return TimeSpan(_seconds - right._seconds);                                 //                                  //
} // of overloaded subtract                                                   //                                  //

/*******************************************************************************************************************
** Class Constructor instantiates the class                                                                       **
*******************************************************************************************************************/
MCP7940_Class::MCP7940_Class()  {} // of class constructor                    //                                  //

/*******************************************************************************************************************
** Class Destructor currently does nothing and is included for compatibility purposes                             **
*******************************************************************************************************************/
MCP7940_Class::~MCP7940_Class() {} // of class destructor                     //                                  //

/*******************************************************************************************************************
** Method begin starts I2C communications with the device, using a default address if one is not specified and    **
** return true if the device has been detected and false if it was not                                            **
*******************************************************************************************************************/
bool MCP7940_Class::begin(const uint32_t i2cSpeed) {                          // Start I2C communications         //
  Wire.begin();                                                               // Start I2C as master device       //
  Wire.setClock(i2cSpeed);                                                    // Set the I2C bus speed            //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the MCP7940M             //
  uint8_t errorCode = Wire.endTransmission();                                 // See if there's a device present  //
  if (errorCode == 0) {                                                       // If we have a MCP7940M            //
    clearRegisterBit(MCP7940_RTCHOUR, MCP7940_12_24);                         // Use 24 hour clock                //
    setRegisterBit(MCP7940_CONTROL, MCP7940_ALMPOL);                          // assert alarm low, default high   //
    _CrystalStatus = readRegisterBit(MCP7940_RTCSEC, MCP7940_ST);             // Status bit from register         //
    _OscillatorStatus = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_OSCRUN);    // Oscillator state from register   //
  } else {                                                                    //                                  //
    return false;                                                             // return error if no device found  //
  } // of if-then-else device detected                                        //                                  //
  return true;                                                                // return success                   //
} // of method begin()                                                        //                                  //

/*******************************************************************************************************************
** Method readByte reads 1 byte from the specified address                                                        **
*******************************************************************************************************************/
uint8_t MCP7940_Class::readByte(const uint8_t addr) {                         //                                  //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  Wire.requestFrom(MCP7940_ADDRESS, (uint8_t)1);                              // Request 1 byte of data           //
  return Wire.read();                                                         // read it and return it            //
} // of method readByte()                                                     //                                  //

/*******************************************************************************************************************
** Method writeByte write 1 byte to the specified address                                                         **
*******************************************************************************************************************/
void MCP7940_Class::writeByte(const uint8_t addr, const uint8_t data) {       //                                  //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(addr);                                                           // Send register address to write   //
  Wire.write(data);                                                           // Send data to write to register   //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
} // of method writeByte()                                                    //                                  //

/*******************************************************************************************************************
** Method clearRegisterBit clears the specified bit in the register                                               **
*******************************************************************************************************************/
void MCP7940_Class::clearRegisterBit(const uint8_t reg, const uint8_t b) {    //                                  //
  writeByte(reg, readByte(reg) & ~(1 << b));                                  //                                  //
} // of method clearRegisterBit()                                             //                                  //

/*******************************************************************************************************************
** Method setRegisterBit sets the specified bit in the register                                                   **
*******************************************************************************************************************/
void MCP7940_Class::setRegisterBit(const uint8_t reg, const uint8_t b) {
  writeByte(reg, readByte(reg) | (1 << b));                                   //                                  //
} // of method setRegisterBit()                                               //                                  //

/*******************************************************************************************************************
** Method writeRegisterBit sets or clears the specified bit based on bitvalue                                     **
*******************************************************************************************************************/
void MCP7940_Class::writeRegisterBit(const uint8_t reg, const uint8_t b,      //                                  //
                                     const bool bitvalue) {                   //                                  //
  bitvalue ? setRegisterBit(reg, b) : clearRegisterBit(reg, b);               //                                  //
} // of method writeRegisterBit()                                             //                                  //

/*******************************************************************************************************************
** Method readRegisterBit reads a specific bit from the register                                                  **
*******************************************************************************************************************/
uint8_t MCP7940_Class::readRegisterBit(const uint8_t reg, const uint8_t b) {  //                                  //
  return bitRead(readByte(reg), b);                                           //                                  //
} // of method readRegisterBit()                                              //                                  //

/*******************************************************************************************************************
** Method bcd2dec converts a BCD encoded value into number representation                                         **
*******************************************************************************************************************/
uint8_t MCP7940_Class::bcd2int(const uint8_t bcd) {                           // convert BCD digits to integer    //
  return ((bcd / 16 * 10) + (bcd % 16));                                      //                                  //
} // of method bcd2int                                                        //                                  //

/*******************************************************************************************************************
** Method dec2bcd converts an integer to BCD encoding                                                             **
*******************************************************************************************************************/
uint8_t MCP7940_Class::int2bcd(const uint8_t dec) {                           // convert BCD digits to integer    //
  return ((dec / 10 * 16) + (dec % 10));                                      //                                  //
} // of method int2bcd                                                        //                                  //

/*******************************************************************************************************************
** Method deviceStatus checks to see if the MCP7940 crystal has been turned on or off                             **
*******************************************************************************************************************/
bool MCP7940_Class::deviceStatus() {                                          // See if the crystal is running    //
  return _CrystalStatus;                                                      // Return internal variable         //
} // of method DeviceStatus                                                   //                                  //

/*******************************************************************************************************************
** Method deviceStart sets the status register to turn on the clock                                               **
*******************************************************************************************************************/
bool MCP7940_Class::deviceStart() {                                           // Start crystal                    //
  setRegisterBit(MCP7940_RTCSEC, MCP7940_ST);                                 // Set the ST bit                   //
  _CrystalStatus = readRegisterBit(MCP7940_RTCSEC, MCP7940_ST);               // Get status bit from register     //
  for (uint8_t j = 0; j < 255; j++) {                                         // Loop until changed or overflow   //
    _OscillatorStatus = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_OSCRUN);    // Wait for oscillator to start     //
    if (_OscillatorStatus) break;                                             // Exit loop on success             //
    delay(1);                                                                 // Allow oscillator time to start   //
  } // of for-next oscillator loop                                            //                                  //
  return _OscillatorStatus;                                                   // Return state                     //
} // of method deviceStart                                                    //                                  //

/*******************************************************************************************************************
** Method deviceStop sets the status register to turn off the clock                                               **
*******************************************************************************************************************/
bool MCP7940_Class::deviceStop() {                                            // Stop crystal                     //
  clearRegisterBit(MCP7940_RTCSEC, MCP7940_ST);                               // clear the ST bit.                //
  _CrystalStatus = 0;                                                         // set to false for off status      //
  for (uint8_t j = 0; j < 255; j++) {                                         // Loop until changed or overflow   //
    _OscillatorStatus = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_OSCRUN);    // Wait for oscillator to stop      //
    if (!_OscillatorStatus) break;                                            // Exit loop on success             //
    delay(1);                                                                 // Allow oscillator time to stop    //
  } // of for-next oscillator loop                                            //                                  //
  return _OscillatorStatus;                                                   // Return state                     //
} // of method deviceStop                                                     //                                  //

/*******************************************************************************************************************
** Method now() returns the current date/time                                                                     **
** then the power down time is returned, if the timeType = 2 then the power up time is returned. These options are**
** not documented as the official calls are "getPowerOffDate" and "getPowerOnDate"                                **
*******************************************************************************************************************/
DateTime MCP7940_Class::now() {                                               // Return current date/time         //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(MCP7940_RTCSEC);                                                 // Start at specified register      //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  Wire.requestFrom(MCP7940_ADDRESS, (uint8_t)7);                              // Request 7 bytes of data          //
  _ss = bcd2int(Wire.read() & 0x7F);                                          // Clear high bit in seconds        //
  _mm = bcd2int(Wire.read() & 0x7F);                                          // Clear high bit in minutes        //
  _hh = bcd2int(Wire.read() & 0x3F);                                          // Keep only 6 LSB bits             //
  Wire.read();                                                                // Ignore Day-Of-Week register      //
  _d = bcd2int(Wire.read()  & 0x3F);                                          // Clear 2 high bits for day-o-month//
  _m = bcd2int(Wire.read()  & 0x1F);                                          // Clear 3 high bits for Month      //
  _y = bcd2int(Wire.read()) + 2000;                                           // Add 2000 to internal year        //
  return DateTime (_y, _m, _d, _hh, _mm, _ss);                                // Return class value               //
} // of method now                                                            //                                  //

/*******************************************************************************************************************
** Method getPowerDown() returns the date/time that the power went off. This is set back to zero once the power   **
** fail flag is reset.                                                                                            **
*******************************************************************************************************************/
DateTime MCP7940_Class::getPowerDown() {                                      // Get the Date when power went off //
  uint8_t min, hr, day, mon;                                                  // temporary storage                //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(MCP7940_PWRDNMIN);                                               // Start at specified register      //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  Wire.requestFrom(MCP7940_ADDRESS, (uint8_t)4);                              // Request 4 bytes of data          //
  min = bcd2int(Wire.read() & 0x7F);                                          // Clear high bit in minutes        //
  hr  = bcd2int(Wire.read() & 0x3F);                                          // Clear all but 6 LSBs             //
  day = bcd2int(Wire.read() & 0x3F);                                          // Clear 2 high bits for day-o-month//
  mon = bcd2int(Wire.read() & 0x1F);                                          // Clear 3 high bits for Month      //
  return DateTime (0, mon, day, hr, min, 0);                                  // Return class value               //
} // of method getPowerDown()                                                 //                                  //

/*******************************************************************************************************************
** Method getPowerUp() returns the date/time that the power went off. This is set back to zero once the power     **
** fail flag is reset.                                                                                            **
*******************************************************************************************************************/
DateTime MCP7940_Class::getPowerUp() {                                        // Get the Date when power came back//
  uint8_t min, hr, day, mon;                                                  // temporary storage                //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(MCP7940_PWRUPMIN);                                               // Start at specified register      //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  Wire.requestFrom(MCP7940_ADDRESS, (uint8_t)4);                              // Request 4 bytes of data          //
  min = bcd2int(Wire.read() & 0x7F);                                          // Clear high bit in minutes        //
  hr  = bcd2int(Wire.read() & 0x3F);                                          // Clear 2  high bits               //
  day = bcd2int(Wire.read() & 0x3F);                                          // Clear 2 high bits for day-o-month//
  mon = bcd2int(Wire.read() & 0x1F);                                          // Clear 3 high bits for Month      //
  return DateTime (0, mon, day, hr, min, 0);                                  // Return class value               //
} // of method getPowerUp()                                                   //                                  //

/*******************************************************************************************************************
** Method adjust() sets the current date/time. This is an overloaded function, if called with no parameters then  **
** the RTC is set to the date/time when the program was compiled and uploaded. Otherwise the values are set, but  **
** the oscillator is stopped during the process and needs to be restarted upon completion.                        **
*******************************************************************************************************************/
void MCP7940_Class::adjust() {                                                // Set the RTC date and Time        //
  adjust(DateTime(F(__DATE__), F(__TIME__)));                                 // Set to compile time              //
} // of method adjust                                                         /-----------------------------------//
void MCP7940_Class::adjust(const DateTime& dt) {                              // Set the RTC date and Time        //
  deviceStop();                                                               // Stop the oscillator              //
  writeByte(MCP7940_RTCSEC,   int2bcd(dt.second()));                          // Write seconds, keep device off   //
  writeByte(MCP7940_RTCMIN,   int2bcd(dt.minute()));                          // Write the minutes value          //
  writeByte(MCP7940_RTCHOUR,  int2bcd(dt.hour()));                            // Also re-sets the 24Hour clock on //
  weekdayWrite(dt.dayOfTheWeek());                                            // Update the weekday               //
  writeByte(MCP7940_RTCDATE,  int2bcd(dt.day()));                             // Write the day of month           //
  writeByte(MCP7940_RTCMTH,   int2bcd(dt.month()));                           // Month, ignore R/O leapyear bit   //
  writeByte(MCP7940_RTCYEAR,  int2bcd(dt.year() - 2000));                     // Write the year                   //
  deviceStart();                                                              // Restart the oscillator           //
  _SetUnixTime = now().unixtime();                                            // Store time of last change        //
} // of method adjust                                                         //                                  //

/*******************************************************************************************************************
** Method weekdayRead will read the weekday number from the RTC. This number is user-settable and is incremented  **
** when the day shifts. It is set as part of the adjust() method where Monday is weekday 1                        **
*******************************************************************************************************************/
uint8_t MCP7940_Class::weekdayRead() {                                        // Read the DOW from the RTC        //
  return readByte(MCP7940_RTCWKDAY) & 0x07;                                   // no need to convert, values 1-7   //
} // of method weekdayRead()                                                  //                                  //

/*******************************************************************************************************************
** Method weekdaywrite() will write the weekday (1-7) to the register and return the setting. If the input value  **
** is out of range then nothing will be written and a 0 will be returned. The unused bits of the register are     **
** preserved                                                                                                      **
*******************************************************************************************************************/
uint8_t MCP7940_Class::weekdayWrite(const uint8_t dow) {                      // Write the DOW to the RTC         //
  uint8_t retval = (readByte(MCP7940_RTCWKDAY) & B11111000) | dow;            // Read, mask DOW bits & add DOW    //
  if (dow > 0 && dow < 8) {                                                   // If parameter is in range, then   //
    writeByte(MCP7940_RTCWKDAY, retval);                                      // Write the register               //
    retval = dow;                                                             // set the return value             //
  } // of if-then we have a good DOW                                          //                                  //
  return retval;                                                              // return the value                 //
} // of method weekdayWrite()                                                 //                                  //

/*******************************************************************************************************************
** Method calibrate(). This is an overloaded definition whose functionality differs between the different call    **
** types.                                                                                                         **
**                                                                                                                **
** 1. calibrate();                                                                                                **
**    Reset the fine trim calibration offset to 0                                                                 **
** 2. calibrate(newTrim);                                                                                         **
**    Set the trim value to the fine-trim offset specified in the parameter                                       **
** 3. calibrate(DateTime);                                                                                        **
**    Accepts a current date/time value and compares that to the current date/time of the RTC and computes a cal- **
**    ibration factor depending upon the time difference between the two and how long the timespan between the two**
**    is. The longer the period between setting the clock and comparing the difference between real time and ind- **
**    icated time the better the resulting calibration accuracy will be. The datasheet explains the calibration   **
**    formula on page 28. First, the error in parts-per-million is computed using PPM = (SecDev/ExpectedSeconds)* **
**    10000000.  Then the trim register is computed using the formula TRIMVAL = (PPM*32768*60)/(1000000*2). The   **
**    code below is designed for easy reading, not for performance. The ppm <> 130 is computed by solving the     **
**    equation for the maximum TRIM value of 127 and computing the corresponding max and min values for ppm. This **
**    allows the trim variable to be one byte rather than a long integer.                                         **
** 4. calibrate(Frequency);                                                                                       **
**    The last calibration method is to measure the square wave output and compute the trim value according to the**
**    difference between the actual frequency and the expected frequency.                                         **
*******************************************************************************************************************/
int8_t MCP7940_Class::calibrate() {                                           // Calibrate the RTC                //
  clearRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);                         // fine trim mode on, to be safe    //
  writeByte(MCP7940_OSCTRIM, (uint8_t)0);                                     // Write zeros to the trim register //
  return(0);                                                                  // Return new calibration value     //
} // of method calibrate()                                                    //----------------------------------//
int8_t MCP7940_Class::calibrate(const int8_t newTrim) {                       // Calibrate the RTC                //
  int8_t trim = abs(newTrim);                                                 // Make a local copy of absolute val//
  if (newTrim < 0) {                                                          // if the trim is less than 0, then //
    trim = 0x80 | trim;                                                       // set non-excess 128 negative val  //
  } // of if-then value of trim is less than 0                                //                                  //
  clearRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);                         // fine trim mode on, to be safe    //
  writeByte(MCP7940_OSCTRIM, trim);                                           // Write value to the trim register //
  _SetUnixTime = now().unixtime();                                            // Store time of last change        //
  return newTrim;                                                             // return the computed trim value   //
} // of method calibrate()                                                    //----------------------------------//
int8_t MCP7940_Class::calibrate(const DateTime& dt) {                         // Overloaded calibrate definition  //
  int32_t SecDeviation = dt.unixtime() - now().unixtime();                    // Get difference in seconds        //
  int32_t ExpectedSec  = now().unixtime() - _SetUnixTime;                     // Get number of seconds since set  //
  int32_t ppm          = 1000000 * SecDeviation / ExpectedSec;                // Multiply first to avoid trunc    //
  if (ppm > 130) {                                                            // Force number ppm to be in range  //
    ppm = 130;                                                                //                                  //
  } else if (ppm < -130) {                                                    // check for low out-of-bounds too  //
    ppm = -130;                                                               //                                  //
  } // of if-then-else ppm out of range                                       //                                  //
  int8_t trim = readByte(MCP7940_OSCTRIM);                                    // Read current trim register value //
  if (trim >> 7) {                                                            // use negative value if necessary  //
    trim = (~0x80 & trim) * -1;                                               //                                  //
  } // of if-then trim is set                                                 //                                  //
  trim += ppm * 32768 * 60 / 2000000;                                         // compute the new trim value       //
  return calibrate((const int8_t)trim);                                       //                                  //
} // of method calibrate()                                                    //----------------------------------//
int8_t MCP7940_Class::calibrate(const float fMeas) {                          // Calibrate according to frequency //
  int16_t  trim   = getCalibrationTrim();                                     // Get the current trim             //
  uint32_t fIdeal = getSQWSpeed();                                            // read the current SQW Speed code  //
  switch (fIdeal) {                                                           // set variable to real SQW speed   //
    case 0: fIdeal =     1;                                                   //                                  //
            break;                                                            //                                  //
    case 1: fIdeal =  4096;                                                   //                                  //
            break;                                                            //                                  //
    case 2: fIdeal =  8192;                                                   //                                  //
            break;                                                            //                                  //
    case 3: fIdeal = 32768;                                                   //                                  //
            trim   =     0;                                                   // Trim is ignored on 32KHz signal  //
            break;                                                            //                                  //
    case 4: fIdeal =    64;                                                   //                                  //
    } // of switch SQWSpeed value                                             //                                  //
  trim += ((fMeas-(float)fIdeal) * (32768.0/fIdeal) * 60.0) / 2.0;            // Use formula from datasheet       //
  if (trim > 127) {                                                           // Force number ppm to be in range  //
    trim = 127;                                                               //                                  //
    } else if (trim < -127) {                                                 // check for low out-of-bounds too  //
    trim = -127;                                                              //                                  //
  } // of if-then-else trim out of range                                      //                                  //
  trim = calibrate((int8_t)trim);                                             // Set the new trim value           //
  return((int8_t)trim);                                                       //                                  //
} // of method calibrate()                                                    //----------------------------------//

/*******************************************************************************************************************
** Method getCalibrationTrim(). This function returns the TRIMVAL trim values. Since the number in the register   **
** can be negative but is not in excess-128 format any negative numbers need to be manipulated before returning   **
*******************************************************************************************************************/
int8_t MCP7940_Class::getCalibrationTrim() {                                  // Get the trim register value      //
  uint8_t trim = readByte(MCP7940_OSCTRIM);                                   // read the register                //
  if (trim >> 7) {                                                            // If trim is negative, the convert //
    trim = (0x7F & trim) * -1;                                                // convert to excess128 value       //
  } // of if-then less than zero trim                                         //                                  //
  return ((int8_t)trim);                                                      // return the trim value            //
} // of method getCalibrationTrim()                                           //                                  //

/*******************************************************************************************************************
** Method setMFP() will set the MFP (Multifunction Pin) to the requested state and return success if, and only if,**
** the Square Wave enable is not turned on and both ALARM 0 and 1 are turned off, other it returns a false        **
*******************************************************************************************************************/
bool MCP7940_Class::setMFP(const bool value) {                                // Set the MFP pin state            //
  uint8_t registerValue = readByte(MCP7940_CONTROL);                          // Get Control register             //
  if ((registerValue & 0x70) != 0) {                                          // Error if SQWEN/ALM1EN/ALM0EN set //
    return false;                                                             //                                  //
  } // of if-then an invalid value                                            //                                  //
  writeRegisterBit(MCP7940_CONTROL, MCP7940_OUT, value);                      //                                  //
  return true;                                                                // Return success                   //
} // of method setMFP()                                                       //                                  //

/*******************************************************************************************************************
** Method getMFP() will get the MFP (Multifunction Pin) state. On is true and Off is false. This is read from the **
** control register if no alarms are enabled, otherwise the two alarm states must be checked.                     **
** Returns one of the following values:                                                                           **
** 0 = pin set LOW.                                                                                               **
** 1 = pin set HIGH.                                                                                              **
** 2 = pin controlled by alarms.                                                                                  **
** 3 = pin controlled by square wave output.                                                                      **
*******************************************************************************************************************/
uint8_t MCP7940_Class::getMFP() {                                             // Get the MFP pin state            //
  uint8_t controlRegister = readByte(MCP7940_CONTROL);                        // Get control register contents    //
  if (controlRegister & (1 << MCP7940_SQWEN)) {                               // Square wave output enabled       //
    return 3;                                                                 // MFP in SQW output mode           //
  } else if ((controlRegister & (1 << MCP7940_ALM0EN)) |                      // One or both alarms enable, and   //
             (controlRegister & (1 << MCP7940_ALM1EN))) {                     // no square wave output.           //
    return 2;                                                                 // MFP in alarm output mode         //
  } // of if-then-else square wave enabled                                    //                                  //
  return bitRead(controlRegister, MCP7940_OUT);                               // MFP in manual mode, return value //
} // of method getMFP()                                                       //                                  //

/*******************************************************************************************************************
** Method setAlarm will set one of the 2 alarms.                                                                  **
**                                                                                                                **
** In order to configure the alarm modules, the following steps need to be performed in order:                    **
** 1. Load the timekeeping registers and enable the oscillator                                                    **
** 2. Configure the ALMxMSK<2:0> bits to select the desired alarm mask                                            **
** 3. Set or clear the ALMPOL bit according to the desired output polarity                                        **
** 4. Ensure the ALMxIF flag is cleared                                                                           **
** 5. Based on the selected alarm mask, load the alarm match value into the appropriate register(s)               **
** 6. Enable the alarm module by setting the ALMxEN bit                                                           **
**                                                                                                                **
** There are two ALMPOL bits - one in the ALM0WKDAY register which can be written, one in the ALM1WKDAY register  **
** which is read-only and reflects the value of the ALMPOL in ALM0WKDAY.                                          **
*******************************************************************************************************************/
bool MCP7940_Class::setAlarm(const uint8_t alarmNumber,                       // Alarm number 0 or 1              //
                             const uint8_t alarmType,                         // Alarm type 0-7, see above        //
                             const DateTime dt,                               // Date/Time to set alarm from      //
                             const bool state) {                              // Alarm on (true) or off (false)   //
  bool success = false;                                                       // Assume no success                //
  if (alarmNumber < 2 &&                                                      //                                  //
      alarmType   < 8 &&                                                      //                                  //
      alarmType  != 5 &&                                                      //                                  //
      alarmType  != 6 &&                                                      //                                  //
      deviceStart()      ) {                                                  // if parameters and oscillator OK  //
    clearRegisterBit(MCP7940_CONTROL, alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN); // Turn off the alarm       //
    uint8_t offset = 7 * alarmNumber;                                         // Offset to be applied             //
    uint8_t wkdayRegister = readByte(MCP7940_ALM0WKDAY + offset);             // Load register to memory          //
    wkdayRegister &= ((1 << MCP7940_ALM0IF) | (1 << MCP7940_ALMPOL));         // Keep ALMPOL and ALMxIF bits      //
    wkdayRegister |= alarmType << 4;                                          // Set 3 bits from alarmType        //
    wkdayRegister |= (dt.dayOfTheWeek() & 0x07);                              // Set 3 bits for dow from date     //
    writeByte(MCP7940_ALM0WKDAY + offset, wkdayRegister);                     // Write alarm mask                 //
    writeByte(MCP7940_ALM0SEC + offset, int2bcd(dt.second()));                // Write seconds, keep device off   //
    writeByte(MCP7940_ALM0MIN + offset, int2bcd(dt.minute()));                // Write the minutes value          //
    writeByte(MCP7940_ALM0HOUR + offset, int2bcd(dt.hour()));                 // Also re-sets the 24Hour clock on //
    writeByte(MCP7940_ALM0DATE + offset, int2bcd(dt.day()));                  // Write the day of month           //
    writeByte(MCP7940_ALM0MTH + offset, int2bcd(dt.month()));                 // Month, ignore R/O leap-year bit  //
    setAlarmState(alarmNumber, state);                                        // Set the requested alarm to state //
    success = true;                                                           // Set return status to success     //
  } // of if-then alarmNumber and alarmType are valid and device running      //                                  //
  return success;                                                             // return the status                //
} // of method setAlarm                                                       //                                  //

/*******************************************************************************************************************
** Alarm polarity (see also TABLE 5-10 on p.27 of the datasheet).                                                 **
** Note: the MFP pin is open collector, it needs an external pull-up resistor.                                    **
**                                                                                                                **
** If only one alarm is set:                                                                                      **
** polarity = 0: MFP high when no alarm, low when alarm.                                                          **
** polarity = 1: MFP low when no alarm, high when alarm.                                                          **
**                                                                                                                **
** If both alarms are set:                                                                                        **
** polarity = 0: MFP high when no alarm or one alarm, low when both alarms go off.                                **
** polarity = 1: MFP low when no alarm, high when eihter or both alarms go off.                                   **
**                                                                                                                **
** In most situations you will want to set polarity to 1 if you have two alarms set, to be able to see when an    **
** alarm goes off using the MFP pin.                                                                              **
**                                                                                                                **
*******************************************************************************************************************/
void MCP7940_Class::setAlarmPolarity(const bool polarity) {                   // Set the polarity of the alarm    //
  writeRegisterBit(MCP7940_ALM0WKDAY, MCP7940_ALMPOL, polarity);              // Write polarity to the ALMPOL bit.//
  return;                                                                     //                                  //
} // of method setAlarmPolarity()                                             //                                  //

/*******************************************************************************************************************
** Method getAlarm will return the date/time settings for the given alarm and update the alarmType parameter with **
** the alarm type that was set                                                                                    **
*******************************************************************************************************************/
DateTime MCP7940_Class::getAlarm(const uint8_t alarmNumber,                   // Return alarm date/time & type    //
                                 uint8_t &alarmType) {                        //                                  //
  if (alarmNumber > 1) {                                                      // return an error if bad alarm no. //
    return DateTime(0);                                                       //                                  //
  } // of if-then bad alarm number                                            //                                  //
  uint8_t offset = 7 * alarmNumber;                                           // Offset to be applied             //
  alarmType = (readByte(MCP7940_ALM0WKDAY + offset) >> 4) & B111;             // get 3 bits for alarmType         //
  uint8_t ss = bcd2int(readByte(MCP7940_ALM0SEC + offset) & 0x7F);            // Clear high bit in seconds        //
  uint8_t mm = bcd2int(readByte(MCP7940_ALM0MIN + offset) & 0x7F);            // Clear high bit in minutes        //
  uint8_t hh = bcd2int(readByte(MCP7940_ALM0HOUR + offset) & 0x3F);           // Clear high bits in hours         //
  uint8_t d  = bcd2int(readByte(MCP7940_ALM0DATE + offset) & 0x3F);           // Clear 2 high bits for day-o-month//
  uint8_t m  = bcd2int(readByte(MCP7940_ALM0MTH + offset) & 0x1F);            // Clear 3 high bits for Month      //
  uint16_t y = 0;                                                             // Year is not part of the alarms   //
  return DateTime (y, m, d, hh, mm, ss);                                      // Return class value               //
} // of method getAlarm()                                                     //                                  //

/*******************************************************************************************************************
** Method clearAlarm will clear a set alarm by re-writing the same contents back to the register                  **
*******************************************************************************************************************/
bool MCP7940_Class::clearAlarm(const uint8_t alarmNumber) {                   // Clear an Alarm                   //
  if (alarmNumber > 1) {                                                      // return an error if bad alarm no. //
    return false;                                                             //                                  //
  } // of if-then a bad alarm number                                          //                                  //
  clearRegisterBit(alarmNumber ? MCP7940_ALM1WKDAY : MCP7940_ALM0WKDAY, MCP7940_ALM0IF);// reset register bit     //
  return true;                                                                // return success                   //
} // of method clearAlarm()                                                   //                                  //

/*******************************************************************************************************************
** Method setAlarmState() will turn an alarm on or off without changing the alarm condition                       **
*******************************************************************************************************************/
bool MCP7940_Class::setAlarmState(const uint8_t alarmNumber,const bool state){// Set alarm to on or off           //
  if (alarmNumber > 1) {                                                      // if not alarm 0 or 1 then error   //
    return false;                                                             //                                  //
  } // of if-then a bad alarm number                                          //                                  //
  writeRegisterBit(MCP7940_CONTROL, alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN, state); // Overwrite reg bit   //
  return true;                                                                // Return success                   //
} // of setAlarmState()                                                       //                                  //

/*******************************************************************************************************************
** Method getAlarmState() will return whether an alarm is turned on or off                                        **
*******************************************************************************************************************/
bool MCP7940_Class::getAlarmState(const uint8_t alarmNumber) {                //                                  //
  if (alarmNumber > 1) {                                                      // if not alarm 0 or 1 then error   //
    return false;                                                             //                                  //
} // of if-then a bad alarm number                                            //                                  //
  return readRegisterBit(MCP7940_CONTROL, alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN); // Get state of alarm   //
} // of getAlarmState()                                                       //                                  //

/*******************************************************************************************************************
** Method isAlarm will return whether an alarm is active or not                                                   **
*******************************************************************************************************************/
bool MCP7940_Class::isAlarm(const uint8_t alarmNumber) {                      // Return alarm status              //
  if (alarmNumber > 1) {                                                      // return an error if bad alarm no. //
    return false;                                                             //                                  //
  } // of if-then a bad alarm number                                          //                                  //
  return readRegisterBit(alarmNumber ? MCP7940_ALM1WKDAY : MCP7940_ALM0WKDAY, MCP7940_ALM0IF); // Get alarm state //
} // of method clearAlarm()                                                   //                                  //

/*******************************************************************************************************************
** Method getSQWSpeed will return the list value for the frequency of the square wave. Values are B00 for 1Hz,    **
** B01 for 4.096kHz, B10 for 8.192kHz and B11 for 32.768kHz. If square wave is not turned on then a 0 is returned **
*******************************************************************************************************************/
uint8_t MCP7940_Class::getSQWSpeed() {                                        // Return the SQW frequency code    //
  uint8_t frequency = readByte(MCP7940_CONTROL);                              // Read the control register        //
  if (frequency & 0x40) {                                                     // return 2 bits if SQW enabled     //
    return (frequency & 0x03);                                                //                                  //
  } // of if-then square wave frequency set                                   //                                  //
  else return 0;                                                              // otherwise return 0               //
} // of method getSQWSpeed()                                                  //                                  //

/*******************************************************************************************************************
** Method setSQWSpeed will set the square wave speed to a value. Values are B00 for 1Hz, B01 for 4.096kHz, B10    **
** for 8.192kHz and B11 for 32.768kHz. By default the square wave is also turned on, but the optional setState    **
** parameter changes that initial state. The return value is the state of the SQW after setting                   **
** 0 = 1       Hz                                                                                                 **
** 1 = 4.096  kHz                                                                                                 **
** 2 = 8.192  kHz                                                                                                 **
** 3 = 32.768 kHz                                                                                                 **
** 4 = 64      Hz                                                                                                 **
*******************************************************************************************************************/
bool MCP7940_Class::setSQWSpeed(uint8_t frequency, bool state) {              // Set the SQW frequency code       //
  if (frequency < 4) {                                                        // If the frequency is < 64Hz       //
    uint8_t registerValue = readByte(MCP7940_CONTROL);                        // read the register to a variable  //
    bitWrite(registerValue, MCP7940_SQWEN, state);                            //                                  //
    bitWrite(registerValue, MCP7940_SQWFS0, bitRead(frequency, 0));           // 2 bits are used for frequency    //
    bitWrite(registerValue, MCP7940_SQWFS1, bitRead(frequency, 1));           //                                  //
    clearRegisterBit(registerValue, MCP7940_CRSTRIM);                         // CRSTRIM bit must be cleared      //
    writeByte(MCP7940_CONTROL, registerValue);                                // Write register settings          //
  } else if (frequency == 4) {                                                // If the frequency is 64Hz         //
    setRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);                         // CRSTRIM bit must be set for 64Hz //
    writeRegisterBit(MCP7940_CONTROL, MCP7940_SQWEN, state);                  // Set then square wave enable bit  //
  }  // of if-then-else less then 64Hz or equal                               //                                  //
  return state;                                                               // Return whether enabled or not    //
} // of method setSQWState()                                                  //                                  //

/*******************************************************************************************************************
** Method setSQWState will turn on the square wave generator bit                                                  **
*******************************************************************************************************************/
bool MCP7940_Class::setSQWState(const bool state) {                           // Set the SQW frequency state      //
  writeRegisterBit(MCP7940_CONTROL, MCP7940_SQWEN, state);                    // set the one bit to state         //
  return state;                                                               // Return whether enabled or not    //
} // of method setSQWState                                                    //                                  //

/*******************************************************************************************************************
** Method getSQWState will turn on the square wave generator bit                                                  **
*******************************************************************************************************************/
bool MCP7940_Class::getSQWState() {                                           // Get the SQW frequency state      //
  return readRegisterBit(MCP7940_CONTROL, MCP7940_SQWEN);                     //                                  //
} // of method getSQWState()                                                  //                                  //

/*******************************************************************************************************************
** Method setBattery() will enable or disable battery backup for the MCP7940N and have no effect on the MCP7940M  **
*******************************************************************************************************************/
bool MCP7940_Class::setBattery(const bool state) {                             // Enable or disable battery backup//
  writeRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN, state);                   //                                 //
  return (state);                                                              //                                 //
} // of method setBattery()                                                    //                                 //

/*******************************************************************************************************************
** Method getBattery() will return true if the battery backup mode is enabled, otherwise return a 0.              //
*******************************************************************************************************************/
bool MCP7940_Class::getBattery() {                                             // Return battery backup state     //
  return readRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN);                    //                                 //
} // of method setBattery()                                                    //                                 //

/*******************************************************************************************************************
** Method getPowerFail() will return true if a power fail has occurred and the flag hasn't been reset             **
*******************************************************************************************************************/
bool MCP7940_Class::getPowerFail() {                                           // Return true on power fail state //
  return readRegisterBit(MCP7940_RTCWKDAY, MCP7940_PWRFAIL);                   //                                 //
} // of method getPowerFail()                                                  //                                 //

/*******************************************************************************************************************
** Method clearPowerFail() will clear the power fail flag                                                         **
** Note: this will also clear the power fail and power up timestamps. Read those before clearing this bit.        **
*******************************************************************************************************************/
bool MCP7940_Class::clearPowerFail() {                                         // Clear the power fail flag       //
  writeByte(MCP7940_RTCWKDAY, readByte(MCP7940_RTCWKDAY));                     // Write back register to clear    //
  return true;                                                                 //                                 //
} // of method clearPowerFail()                                                //                                 //

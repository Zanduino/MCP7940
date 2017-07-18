/*******************************************************************************************************************
** MCP7940 class method definitions. See the header file for program details and version information              **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
*******************************************************************************************************************/
#include "MCP7940.h"                                                          // Include the header definition    //
/*******************************************************************************************************************
** Class Constructor instantiates the class                                                                       **
*******************************************************************************************************************/
MCP7940_Class::MCP7940_Class()  {} // of class constructor                    //                                  //
/*******************************************************************************************************************
** Class Destructor currently does nothing and is included for compatibility purposes                             **
*******************************************************************************************************************/
MCP7940_Class::~MCP7940_Class() {} // of class destructor                    //                                  //
/*******************************************************************************************************************
** Method begin starts I2C communications with the device, using a default address if one is not specified and    **
** return true if the device has been detected and false if it was not                                            **
*******************************************************************************************************************/
bool MCP7940_Class::begin( ) {                                                // Start I2C Comms with device      //
  Wire.begin();                                                               // Start I2C as master device       //
  delay(MCP7940_I2C_DELAY);                                                   // Give the MCP7940 time to process //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the MCP7940M             //
  uint8_t errorCode = Wire.endTransmission();                                 // See if there's a device present  //
  if (errorCode == 0) {                                                       // If we have a MCP7940M            //
    Wire.beginTransmission(MCP7940_ADDRESS);                                  // Address the MCP7940M             //
    errorCode = readByte(0);Serial.print("0 ");Serial.println(errorCode);
    errorCode = readByte(1);Serial.print("1 ");Serial.println(errorCode);
    errorCode = readByte(2);Serial.print("2 ");Serial.println(errorCode);
    errorCode = readByte(3);Serial.print("3 ");Serial.println(errorCode);
    errorCode = readByte(4);Serial.print("4 ");Serial.println(errorCode);
    errorCode = readByte(5);Serial.print("5 ");Serial.println(errorCode);
    errorCode = readByte(6);Serial.print("6 ");Serial.println(errorCode);
    errorCode = readByte(7);Serial.print("7 ");Serial.println(errorCode);
  } // if no error, then set up device
  else return false;                                                          // return error if no device found  //
  return true;                                                                // return success                   //
} // of method begin()                                                        //                                  //
/*******************************************************************************************************************
** Method readByte reads 1 byte from the specified address                                                        **
*******************************************************************************************************************/
uint8_t MCP7940_Class::readByte(const uint8_t addr) {                         //                                  //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  delayMicroseconds(MCP7940_I2C_DELAY);                                       // delay required for sync          //
  Wire.requestFrom(MCP7940_ADDRESS, (uint8_t)1);                              // Request 1 byte of data           //
  while(!Wire.available());                                                   // Wait until the byte is ready     //
  return Wire.read();                                                         // read it and return it            //
} // of method readByte()                                                     //                                  //
/*******************************************************************************************************************
** Method readWord reads 2 bytes from the specified address                                                       **
*******************************************************************************************************************/
uint16_t MCP7940_Class::readWord(const uint8_t addr) {                        //                                  //
  uint16_t returnData;                                                        // Store return value               //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  delayMicroseconds(MCP7940_I2C_DELAY);                                       // delay required for sync          //
  Wire.requestFrom(MCP7940_ADDRESS, (uint8_t)2);                              // Request 2 consecutive bytes      //
  while(!Wire.available());                                                   // Wait until the byte is ready     //
  returnData  = Wire.read();                                                  // Read the msb                     //
  returnData  = returnData<<8;                                                // shift the data over              //
  returnData |= Wire.read();                                                  // Read the lsb                     //
  return returnData;                                                          // read it and return it            //
} // of method readWord()                                                     //                                  //
/*******************************************************************************************************************
** Method writeByte write 1 byte to the specified address                                                         **
*******************************************************************************************************************/
void MCP7940_Class::writeByte(const uint8_t addr, const uint8_t data) {       //                                  //
  Wire.beginTransmission(MCP7940_ADDRESS);                                    // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  Wire.write(data);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
} // of method writeByte()                                                    //                                  //
/*******************************************************************************************************************
** Method bcd2dec converts a BCD encoded value into number representation                                         **
*******************************************************************************************************************/
uint8_t MCP7940_Class::bcd2int(const uint8_t bcd){                            // convert BCD digits to integer    //
  return (bcd - 6 * (bcd>>4));
//  return ((bcd/16*10)+(bcd%16));                                              // convert 2 BCD digits into integer//
} // of method bcd2int                                                        //                                  //

/*******************************************************************************************************************
** Method dec2bcd converts an integer to BCD encoding                                                             **
*******************************************************************************************************************/
uint8_t MCP7940_Class::int2bcd(const uint8_t dec){                            // convert BCD digits to integer    //
  return (val + 6 * (val/10)));
//  	return ((dec/10*16)+(dec%10));                                            // convert 2 BCD digits into decimal//
} // of method int2bcd                                                        //                                  //


static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
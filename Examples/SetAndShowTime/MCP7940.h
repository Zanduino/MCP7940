/*******************************************************************************************************************
** Class definition header for the MCP7940 from Microchip. This is a Real-Time-Clock with I2C interface. The data **
** sheet can be found at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf                            **
**                                                                                                                **
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
** Vers.  Date       Developer           Comments                                                                 **
** ====== ========== =================== ======================================================================== **
** 1.0.0  2017-07-17 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include <Wire.h>                                                             // Standard I2C "Wire" library      //
#ifndef MCP7940_h                                                             // Guard code definition            //
  #define MCP7940_h                                                           // Define the name inside guard code//
    /***************************************************************************************************************
    ** Declare constants used in the class                                                                        **
    ***************************************************************************************************************/
    const uint8_t  MCP7940_ADDRESS                 = 0x13;                    // Device address, fixed value      //
    const uint8_t  MCP7940_I2C_DELAY               = 2500;                    // Microseconds wait time           //
  class MCP7940_Class {                                                       // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      MCP7940_Class();                                                              // Class constructor                //
      ~MCP7940_Class();                                                             // Class destructor                 //
      bool     begin();                                                       // Start I2C Comms with device      //
    private:                                                                  // Private methods                  //
      uint8_t  readByte(const uint8_t addr);                                  // Read 1 byte from address on I2C  //
      uint16_t readWord(const uint8_t addr);                                  // Read 2 bytes from address on I2C //
      void     writeByte(const uint8_t addr, const uint8_t data);             // Write 1 byte at address to I2C   //
      uint8_t _TransmissionStatus;                                            // Status of I2C transmission       //
  }; // of MCP7940 class definition                                           //                                  //
#endif                                                                        //----------------------------------//

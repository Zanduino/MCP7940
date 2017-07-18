/*******************************************************************************************************************
** Class definition header for the VCNL4010 chip from Vishay Semiconductors. This is a Fully Integrated Proximity **
** and Ambient Light Sensor with Infrared Emitter, I2C Interface and Interrupt Function. The data sheet for the   **
** VCNL4010 can be found at http://www.vishay.com/docs/83462/vcnl4010.pdf and there is a breakout board from      **
** Adafruit with additional information located at https://www.adafruit.com/product/466.                          **
**                                                                                                                **
** When the class is instantiated the default mode is to have triggered readings for the ambient light sensor as  **
** well as for the proximity sensor.  In order to speed up processing the class will trigger the two types of     **
** measurements preemptively, meaning that as soon that either of the results have been read the next measurement **
** is started automatically.  If the continuous mode is activated for either of the ambient or proximity sensors  **
** then no specific measurements are triggered since the device takes care of that internally.                    **
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
** 1.0.1  2017-01-02 Arnd@SV-Zanshin.Com Moved readByte function back into priate area                            **
** 1.0.0  2017-01-01 Arnd@SV-Zanshin.Com Fixed error on continuous mode with proximity, ready for release         **
** 1.0.b2 2016-12-31 Arnd@SV-Zanshin.Com Continued coding                                                         **
** 1.0.b1 2016-12-30 Arnd@SV-Zanshin.Com Created class                                                            **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include <Wire.h>                                                             // Standard I2C "Wire" library      //
#ifndef VCNL4010_h                                                            // Guard code definition            //
  #define VCNL4010_h                                                          // Define the name inside guard code//
    /***************************************************************************************************************
    ** Declare constants used in the class                                                                        **
    ***************************************************************************************************************/
    const uint8_t  VCNL4010_ADDRESS                = 0x13;                    // Device address, fixed value      //
    const uint8_t  VCNL4010_COMMAND_REG            = 0x80;                    // Register containing commands     //
    const uint8_t  VCNL4010_PRODUCT_ID_REG         = 0x81;                    // Register containing product ID   //
    const uint8_t  VCNL4010_PROXIMITY_RATE_REG     = 0x82;                    // Register containing sampling rate//
    const uint8_t  VCNL4010_LED_CURRENT_REG        = 0x83;                    // Register containing IR LED mA    //
    const uint8_t  VCNL4010_AMBIENT_PARAMETER_REG  = 0x84;                    // Register containing ambient set  //
    const uint8_t  VCNL4010_AMBIENT_LIGHT_REG      = 0x85;                    // Register containing ambient data //
    const uint8_t  VCNL4010_PROXIMITY_REG          = 0x87;                    // Register containing Proximity    //
    const uint8_t  VCNL4010_INTERRUPT_REG          = 0x89;                    // Register containing Interrupts   //
    const uint8_t  VCNL4010_LOW_THRESHOLD_MSB_REG  = 0x8A;                    // MSB of low threshold value       //
    const uint8_t  VCNL4010_LOW_THRESHOLD_LSB_REG  = 0x8B;                    // LSB of low threshold value       //
    const uint8_t  VCNL4010_HIGH_THRESHOLD_MSB_REG = 0x8C;                    // MSB of high threshold value      //
    const uint8_t  VCNL4010_HIGH_THRESHOLD_LSB_REG = 0x8D;                    // LSB of high threshold value      //
    const uint8_t  VCNL4010_INTERRUPT_STATUS_REG   = 0x8E;                    // Interrupt status register        //
    const uint8_t  VCNL4010_PROXIMITY_TIMING_REG   = 0x8F;                    // Register containing ProxTiming   //
    const uint8_t  VCNL4010_PRODUCT_VERSION        = 0x21;                    // Current product ID               //
    const uint8_t  VCNL4010_I2C_DELAY              = 0170;                    // Microseconds delay on I2C write  //
  class VCNL4010 {                                                            // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      VCNL4010();                                                             // Class constructor                //
      ~VCNL4010();                                                            // Class destructor                 //
      bool     begin();                                                       // Start I2C Comms with device      //
      void     setProximityHz(const uint8_t Hz=2);                            // Set proximity Hz sampling rate   //
      void     setLEDmA(const uint8_t mA=20);                                 // Set milliamps used by IR LED     //
      void     setProximityFreq(const uint8_t value=0);                       // Set Frequency value from list    //
      void     setAmbientLight(const uint8_t sample=2, const uint8_t avg=32); // Set samples and averaging        //
      void     setAmbientContinuous(const bool ContinuousMode=true);          // Cont. Ambient sampling on/off    //
      void     setProximityContinuous(const bool ContinuousMode=true);        // Cont. Proximity sampling on/off  //
      void     setInterrupt(const uint8_t  count         = 1,                 //                                  //
                            const bool     ProxReady     = false,             //                                  //
                            const bool     ALSReady      = false,             //                                  //
                            const bool     ProxThreshold = false,             //                                  //
                            const bool     ALSThreshold  = false,             //                                  //
                            const uint16_t lowThreshold  = 0,                 //                                  //
                            const uint16_t highThreshold = UINT16_MAX );      //                                  //
      uint16_t getAmbientLight();                                             // Retrieve ambient light reading   //
      uint16_t getProximity();                                                // Retrieve proximity reading       //
      uint8_t  getInterrupt();                                                // Retrieve Interrupt bits          //
      void     clearInterrupt(const uint8_t intVal=0);                        // Clear Interrupt bits             //
    private:                                                                  // Private methods                  //
      uint8_t  readByte(const uint8_t addr);                                  // Read 1 byte from address on I2C  //
      uint16_t readWord(const uint8_t addr);                                  // Read 2 bytes from address on I2C //
      void     writeByte(const uint8_t addr, const uint8_t data);             // Write 1 byte at address to I2C   //
      uint8_t _TransmissionStatus;                                            // Status of I2C transmission       //
      bool    _ContinuousAmbient   = false;                                   // If mode turned on for Ambient    //
      bool    _ContinuousProximity = false;                                   // If mode turned on for Proximity  //
  }; // of VCNL4010 class definition                                          //                                  //
#endif                                                                        //----------------------------------//

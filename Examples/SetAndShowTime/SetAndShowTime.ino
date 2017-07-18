/*******************************************************************************************************************
** Example program for using the MCP7940 library which allows access to the MCP7940M real-time-clock chip.        **
**                                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip                    **
**                                                                                                                **
**                                                                                                                **
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
#include "MCP7940.h"                                                          // Include the library              //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED = 115200;                                         // Set the baud rate for Serial I/O //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
//MCP7940_Class MCP7940;
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
MCP7940_Class MCP7940;
  delay(3000);                                                                // Some Atmel Serials need time     //
  Serial.println(F("Starting SetAndShowTime program"));                       //                                  //
  while (!MCP7940.begin()) {                                                  // Start communication with RTC     //
    Serial.println(F("Unable to find MCP7940M. Checking again in 5s."));      // Show error text                  //
    delay(5000);                                                              // wait                             //
  } // of loop until device is located                                        //                                  //
  Serial.println("Device found.")  ;
//  while (!Sensor.begin()) {                                                   // Loop until sensor found          //
//    Serial.println(F("Error, unable to find or identify VCNL4010."));         // Show error message               //
//    delay(5000);                                                              // Wait 5 seconds before retrying   //
//  } // of if-then we can't initialize or find the device                      //                                  //
  Serial.println(F("MCP7940 initialized.\n\n"));                              //                                  //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
} // of method loop()                                                         //----------------------------------//

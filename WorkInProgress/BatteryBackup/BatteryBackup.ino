/*******************************************************************************************************************
** Example program for using the MCP7940 library and a MCP7940N with power fail and battery backup options        **
**                                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also used  **
** the class definitions of the standard RTClib library from Adafruit/Jeelabs.                                    **
**                                                                                                                **
** The data sheet for the MCP7940M is located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf.   **
** The description for the GPS from adafruit can be found at https://learn.adafruit.com/adafruit-ultimate-gps/.   **
**                                                                                                                **
** The program demonstrates the battery backup related functions in the library                                   **
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
** 1.0.0  2017-07-30 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include <MCP7940.h>                                                          // Include the MCP7940 RTC library  //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED            = 115200;                              // Set the baud rate for Serial I/O //
const uint8_t  SPRINTF_BUFFER_SIZE     =     64;                              // Buffer size for sprintf()        //
const uint8_t  LED_PIN                 =     13;                              // Built in LED                     //
const uint8_t  MFP_PIN                 =      2;                              // Pin used for the MCP7940 MFP     //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MCP7940_Class     MCP7940;                                                    // Create an instance of the MCP7940//
char              inputBuffer[SPRINTF_BUFFER_SIZE];                           // Buffer for sprintf()/sscanf()    //
DateTime          now;                                                        // Global definition of a date/time //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  pinMode(MFP_PIN,INPUT);                                                     // MCP7940 Alarm MFP digital pin    //
  pinMode(LED_PIN,OUTPUT);                                                    // Green built in LED on Arduino    //
  digitalWrite(LED_PIN,LOW);                                                  // Turn off the LED light           //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    delay(3000);                                                              // wait 3 seconds for the serial    //
  #endif                                                                      // interface to initialize          //
  sprintf(inputBuffer,"Starting BatteryBackup program\n- c++ compiler version %s\n- Compiled on %s at %s\n",
          __VERSION__,__DATE__,__TIME__);                                     //                                  //
  Serial.print(inputBuffer);                                                  // Display program and compile date //
  sprintf(inputBuffer,"- Arduino IDE V%d\, CPU Frequency %dmHz\n",            //                                  //
          ARDUINO, F_CPU/1000000);                                            //                                  //
  Serial.print(inputBuffer);                                                  // Display IDE and speed information//
  Serial.print(F("- Initializing MCP7940 Real-Time-Clock\n"));                //                                  //
  while (!MCP7940.begin()) {                                                  // Initialize RTC communications    //
    Serial.println(F("-  Unable to find MCP7940M. Waiting 3 seconds."));      // Show error text                  //
    delay(3000);                                                              // wait three seconds               //
  } // of loop until device is located                                        //                                  //
  MCP7940.adjust();                                                           // Set the RTC time to compile time //
  sprintf(inputBuffer,"- Time set to %04d-%02d-%02d %02d:%02d:%02d\n",        // Use sprintf() to pretty print    //
          now.year(),now.month(),now.day(),now.hour(),now.minute(),           // output with leading zeroes       //
          now.second());                                                      //                                  //
  Serial.print(inputBuffer);                                                  // Output the sprintf buffer        //
  MCP7940.setBattery(true);                                                   // enable Battery Backup            //
  Serial.print("- Battery Backup mode is ");                                  //                                  //
  Serial.println(MCP7940.getBattery());                                       //                                  //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint8_t lastSeconds = 0;                                             // Store last time seconds displayed//
  now = MCP7940.now();                                                        // get the current date/time        //
  if (now.second()!=lastSeconds)  {                                           // If the seconds have changed      //
    sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d",                      // Use sprintf() to pretty print    //
            now.year(),now.month(),now.day(),now.hour(),now.minute(),         // output with leading zeroes       //
            now.second());                                                    //                                  //
    Serial.print(inputBuffer);                                                // Output the sprintf buffer        //
    if (MCP7940.getPowerFail()) {                                             // If a power failure is detected   //
      Serial.print(" *Power Fail*");                                          //                                  //
    } // of if-then power has failed                                          //                                  //
    Serial.println();                                                         //                                  //
    lastSeconds = now.second();                                               // Set the seconds                  //
  } // of if-then seconds have changed                                        //                                  //
} // of method loop()                                                         //----------------------------------//

/*******************************************************************************************************************
** Example program for using the MCP7940 library which demonstrate the use of alarms on the MCP7940N/M real time  **
** clock chip. The library as well as the most current version of this program is available at GitHub using the   **
** address https://github.com/SV-Zanshin/MCP7940 and a more detailed description of this program can be found at  **
** https://github.com/SV-Zanshin/MCP7940/wiki/SetAlarms.ino                                                       **
**                                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also used  **
** the class definitions of the standard RTClib library from Adafruit/Jeelabs.                                    **
**                                                                                                                **
** The data sheet for the MCP7940M is located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf.   **
** The MCP7940N has extra functionality revolving around battery backup but this library's functionality covers   **
** that chip as well.                                                                                             **
**                                                                                                                **
** The MCP7940 has two alarms that can be set. Either can be set to trigger once at a specific point in time or   **
** can be set to be a recurring alarm. When the alarm goes off it sets an alarm bit which can be read using the   **
** the library routine getAlarmState() or via an input pin if the MCP7940's multifunction pin is attached to it.  **
**                                                                                                                **
** This example program sets alarm 0 to trigger every time the RTC's seconds reads "0". Alarm 1 is set to trigger **
** at a time "ALARM1_INTERVAL" seconds from the last time it was triggered.                                       **
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
** 1.0.0  2017-07-29 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include <MCP7940.h>                                                          // Include the MCP7940 RTC library  //
/*******************************************************************************************************************
** Declare all program constants and enumerated types                                                             **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED        = 115200;                                  // Set the baud rate for Serial I/O //
const uint8_t  LED_PIN             =     13;                                  // Arduino built-in LED pin number  //
const uint8_t  SPRINTF_BUFFER_SIZE =     32;                                  // Buffer size for sprintf()        //
const uint8_t  ALARM1_INTERVAL     =     15;                                  // Interval seconds for alarm       //
enum alarmTypes { matchSeconds, matchMinutes, matchHours, matchDayOfWeek,     // Enumeration of alarm types       //
                  matchDayOfMonth, Unused1, Unused2, matchAll, Unknown };     //                                  //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MCP7940_Class MCP7940;                                                        // Create instance of the MCP7940M  //
char          inputBuffer[SPRINTF_BUFFER_SIZE];                               // Buffer for sprintf() / sscanf()  //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    delay(3000);                                                              // wait 3 seconds for the serial    //
  #endif                                                                      // interface to initialize          //
  Serial.print(F("\nStarting SetAlarms program\n"));                          // Show program information         //
  Serial.print(F("- Compiled with c++ version "));                            //                                  //
  Serial.print(F(__VERSION__));                                               // Show compiler information        //
  Serial.print(F("\n- On "));                                                 //                                  //
  Serial.print(F(__DATE__));                                                  //                                  //
  Serial.print(F(" at "));                                                    //                                  //
  Serial.print(F(__TIME__));                                                  //                                  //
  Serial.print(F("\n"));                                                      //                                  //
  while (!MCP7940.begin()) {                                                  // Initialize RTC communications    //
    Serial.println(F("Unable to find MCP7940M. Checking again in 3s."));      // Show error text                  //
    delay(3000);                                                              // wait a second                    //
  } // of loop until device is located                                        //                                  //
  Serial.println(F("MCP7940 initialized."));                                  //                                  //
  while (!MCP7940.deviceStatus()) {                                           // Turn oscillator on if necessary  //
    Serial.println(F("Oscillator is off, turning it on."));                   //                                  //
    bool deviceStatus = MCP7940.deviceStart();                                // Start oscillator and return state//
    if (!deviceStatus) {                                                      // If it didn't start               //
      Serial.println(F("Oscillator did not start, trying again."));           // Show error and                   //
      delay(1000);                                                            // wait for a second                //
    } // of if-then oscillator didn't start                                   //                                  //
  } // of while the oscillator is off                                         //                                  //
  Serial.println("Setting MCP7940M to date/time of library compile");         //                                  //
  MCP7940.adjust();                                                           // Use compile date/time for clock  //
  Serial.print("Date/Time set to ");                                          //                                  //
  DateTime now = MCP7940.now();                                               // get the current time             //
  sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),            // Use sprintf() to pretty print    //
          now.month(), now.day(), now.hour(), now.minute(), now.second());    // date/time with leading zeroes    //
  Serial.println(inputBuffer);                                                // Display the current date/time    //
  Serial.println("Setting alarm 0 for every minute at 0 seconds.");           //                                  //
  MCP7940.setAlarm(0,matchSeconds,now-TimeSpan(0,0,0,now.second()));          // Match once a minute at 0 seconds //
  Serial.print("Setting alarm 1 to go off at ");                              //                                  //
  now = now + TimeSpan(0,0,0,ALARM1_INTERVAL);                                // Add interval to current time     //
  sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),            // Use sprintf() to pretty print    //
          now.month(), now.day(), now.hour(), now.minute(), now.second());    // date/time with leading zeroes    //
  Serial.println(inputBuffer);                                                // Display the current date/time    //
  MCP7940.setAlarm(1,matchAll,now);                                           // Set alarm to go off then         //
  pinMode(LED_PIN,OUTPUT);                                                    // Declare built-in LED as output   //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint8_t secs;                                                        // store the seconds value          //
  DateTime now = MCP7940.now();                                               // get the current time             //
  if (secs != now.second()) {                                                 // Output if seconds have changed   //
    sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),          // Use sprintf() to pretty print    //
            now.month(), now.day(), now.hour(), now.minute(), now.second());  // date/time with leading zeroes    //
    Serial.print(inputBuffer);                                                // Display the current date/time    //
    secs = now.second();                                                      // Set the counter variable         //
    if (MCP7940.isAlarm(0)) {                                                 // When alarm 0 is triggered        //
      Serial.print(" *Alarm0*");                                              //                                  //
      MCP7940.clearAlarm(0);                                                  // Clear the alarm                  //
    } // of if Alarm 0 has been triggered                                     //                                  //
    if (MCP7940.isAlarm(1)) {                                                 // When alarm 1 is triggered        //
      Serial.print(" *Alarm1* resetting to go off next at ");                 //                                  //
      now = now + TimeSpan(0,0,0,ALARM1_INTERVAL);                            // Add interval to current time     //
      sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),        // Use sprintf() to pretty print    //
              now.month(), now.day(), now.hour(), now.minute(), now.second());// date/time with leading zeroes    //
      Serial.print(inputBuffer);                                              // Display the current date/time    //
      MCP7940.setAlarm(1,matchAll,now);                                       // Set alarm to go off in 10s again //
    } // of if Alarm 0 has been triggered                                     // Alarm is cleared when resetting  //
    Serial.println();                                                         //                                  //
  } // of if the seconds have changed                                         //                                  //
} // of method loop()                                                         //----------------------------------//
/*! @file SetAlarms.ino

 @section SetAlarms_intro_section Description

Example program for using the MCP7940 library which demonstrate the use of alarms on the MCP7940N/M
real time clock chip. The library as well as the most current version of this program is available
at GitHub using the address https://github.com/Zanduino/MCP7940 and a more detailed description of
this program can be found at https://github.com/Zanduino/MCP7940/wiki/SetAlarms.ino \n\n The
MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also
used the class definitions of the standard RTClib library from Adafruit/Jeelabs.\n\n The data sheet
for the MCP7940M is located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf. The
MCP7940N has extra functionality revolving around battery backup but this library's functionality
covers that chip as well.\n\n The MCP7940 has two alarms that can be set. Either can be set to
trigger once at a specific point in time or can be set to be a recurring alarm. When the alarm goes
off it sets an alarm bit which can be read using the the library routine getAlarmState() or via an
input pin if the MCP7940's multifunction pin is attached to it.\n\n This example program sets alarm
0 to trigger every time the RTC's seconds reads "0". Alarm 1 is set to trigger at a time
"ALARM1_INTERVAL" seconds from the last time it was triggered.\n\n

@section SetAlarms_license GNU General Public License v3.0
This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

@section SetAlarms_author Author

Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin

@section SetAlarms_Versions Changelog

Version| Date       | Developer           | Comments
------ | ---------- | ------------------- | --------
1.0.3  | 2020-11-16 | SV-Zanshin          | Issue #50 - Reformatted with clang-format
1.0.2  | 2019-01-19 | SV-Zanshin          | Changed comments to doxygen formatting
1.0.1  | 2018-07-07 | SV-Zanshin          | Changed code for compatability reasons to new library
1.0.0  | 2017-07-29 | SV-Zanshin          | Initial coding
*/

#include <MCP7940.h>  // Include the MCP7940 RTC library
/***************************************************************************************************
** Declare all program constants and enumerated types                                             **
***************************************************************************************************/
const uint32_t SERIAL_SPEED{115200};     ///< Set the baud rate for Serial I/O
const uint8_t  LED_PIN{13};              ///< Arduino built-in LED pin number
const uint8_t  SPRINTF_BUFFER_SIZE{32};  ///< Buffer size for sprintf()
const uint8_t  ALARM1_INTERVAL{15};      ///< Interval seconds for alarm
/*! ///< Enumeration of MCP7940 alarm types */
enum alarmTypes {
  matchSeconds,
  matchMinutes,
  matchHours,
  matchDayOfWeek,
  matchDayOfMonth,
  Unused1,
  Unused2,
  matchAll,
  Unknown
};
/***************************************************************************************************
** Declare global variables and instantiate classes                                               **
***************************************************************************************************/
MCP7940_Class MCP7940;                           ///< Create instance of the MCP7940M
char          inputBuffer[SPRINTF_BUFFER_SIZE];  ///< Buffer for sprintf() / sscanf()

void setup() {
  /*!
      @brief  Arduino method called once upon start or restart.
  */
  Serial.begin(SERIAL_SPEED);  // Start serial port at specified Baud rate
#ifdef __AVR_ATmega32U4__      // If this is a 32U4 processor, then wait 3 seconds for the serial
                               // interface to initialize
  delay(3000);
#endif
  Serial.print(F("\nStarting SetAlarms program\n"));
  Serial.print(F("- Compiled with c++ version "));
  Serial.print(F(__VERSION__));  // Show compiler information
  Serial.print(F("\n- On "));
  Serial.print(F(__DATE__));
  Serial.print(F(" at "));
  Serial.print(F(__TIME__));
  Serial.print(F("\n"));
  while (!MCP7940.begin())  // Loop until the RTC communications are established
  {
    Serial.println(F("Unable to find MCP7940. Checking again in 3s."));
    delay(3000);
  }  // of loop until device is located
  Serial.println(F("MCP7940 initialized."));
  while (!MCP7940.deviceStatus())  // Turn oscillator on if necessary
  {
    Serial.println(F("Oscillator is off, turning it on."));
    bool deviceStatus = MCP7940.deviceStart();  // Start oscillator and return state
    if (!deviceStatus)                          // If it didn't start
    {
      Serial.println(F("Oscillator did not start, trying again."));
      delay(1000);
    }  // of if-then oscillator didn't start
  }    // of while the oscillator is of
  Serial.println("Setting MCP7940M to date/time of library compile");
  MCP7940.adjust();  // Use compile date/time to set clock
  Serial.print("Date/Time set to ");
  DateTime now = MCP7940.now();  // get the current time
  // Use sprintf() to pretty print date/time with leading zeroes
  sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  Serial.println(inputBuffer);
  Serial.println("Setting alarm 0 for every minute at 0 seconds.");
  MCP7940.setAlarm(0, matchSeconds, now - TimeSpan(0, 0, 0, now.second()),
                   true);  // Match once a minute at 0 seconds
  Serial.print("Setting alarm 1 to go off at ");
  now = now + TimeSpan(0, 0, 0, ALARM1_INTERVAL);  // Add interval to current time
  sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  Serial.println(inputBuffer);
  MCP7940.setAlarm(1, matchAll, now, true);  // Set alarm to go off then
  pinMode(LED_PIN, OUTPUT);                  // Declare built-in LED as output
}  // of method setup()

void loop() {
  /*!
      @brief  Arduino method called after setup() which loops forever
  */
  static uint8_t secs;
  DateTime       now = MCP7940.now();  // get the current time
  if (secs != now.second())            // Output if seconds have changed
  {
    sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
            now.hour(), now.minute(), now.second());
    Serial.print(inputBuffer);
    secs = now.second();     // Set the counter for later comparision
    if (MCP7940.isAlarm(0))  // When alarm 0 is triggered
    {
      Serial.print(" *Alarm0*");
      MCP7940.clearAlarm(0);
    }                        // of if Alarm 0 has been triggered
    if (MCP7940.isAlarm(1))  // When alarm 0 is triggered
    {
      Serial.print(" *Alarm1* resetting to go off next at ");
      now = now + TimeSpan(0, 0, 0, ALARM1_INTERVAL);  // Add interval to current time
      sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
              now.hour(), now.minute(), now.second());
      Serial.print(inputBuffer);
      MCP7940.setAlarm(1, matchAll, now, true);  // Set alarm to go off in 10s again
    }                                            // of if Alarm 0 has been triggered
    Serial.println();
  }  // of if the seconds have changed
}  // of method loop()

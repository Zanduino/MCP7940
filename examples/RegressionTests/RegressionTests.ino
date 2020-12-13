/*! @file RegressionTests.ino

 @section RegressionTests_intro_section Description

This program is used to execute all public functions in the MCP7940 library and check, insofar as
possible, whether they functions as expected. This is used during a development cycle to see if any
changes or enhancements may have broken previously working functionality.\n The MCP7940 library as
well as the most current version of this program is available at GitHub using the address
https://github.com/Zanduino/MCP7940 and a more detailed description of this program can be found
at https://github.com/Zanduino/MCP7940/wiki/RegressionTests.ino \n\n The MCP7940 library uses the
standard SPI Wire library for communications with the RTC chip and has also used the class
definitions of the standard RTClib library from Adafruit/Jeelabs.\n\n The data sheet for the
MCP7940M is located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf. The MCP7940N
has extra functionality revolving around battery backup but this library's functionality covers that
chip as well.\n\n

@section RegressionTestss_license GNU General Public License v3.0
This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

@section RegressionTests_author Author

Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin

@section RegressionTests_Versions Changelog

Version| Date       | Developer           | Comments
------ | ---------- | ------------------- | --------
1.0.0  | 2020-11-27 | SV-Zanshin          | Issue #55 - Created sketch
*/

#include <MCP7940.h>                              ///< Include the MCP7940 RTC library
const uint32_t SERIAL_SPEED{115200};              ///< Set the baud rate for Serial I/O
const uint8_t  LED_PIN{13};                       ///< Arduino built-in LED pin number
const uint8_t  SPRINTF_BUFFER_SIZE{32};           ///< Buffer size for sprintf()
MCP7940_Class  MCP7940;                           ///< Create an instance of the MCP7940
char           inputBuffer[SPRINTF_BUFFER_SIZE];  ///< Buffer for sprintf()/sscanf()

void showTime(const DateTime& now) {
  /*!
    @brief     Display the DateTime
    @details   Converts the DateTime value to a string and display without CR
    @param[in] now is a DateTime construct to display
    @return    void
  */
  sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  Serial.print(inputBuffer);
}  // of method "showTime()"

void setup() {
  /*!
    @brief  Arduino method called once upon start or restart.
  */
  pinMode(LED_PIN, OUTPUT);    // Declare built-in LED as output
  Serial.begin(SERIAL_SPEED);  // Start serial port at Baud rate
#ifdef __AVR_ATmega32U4__      // If a 32U4 processor, then wait
  delay(3000);                 // 3 seconds for the serial port to
#endif                         // initialize, otherwise continue
  Serial.print(F("\nStarting RegressionTests program\n"));
  Serial.print(F("- Compiled with c++ version "));
  Serial.print(F(__VERSION__));  // Show compiler information
  Serial.print(F("\n- On "));
  Serial.print(F(__DATE__));
  Serial.print(F(" at "));
  Serial.print(F(__TIME__));
  Serial.print(F("\n"));
  while (!MCP7940.begin()) {  // Initialize RTC communications
    Serial.println(F("Unable to find MCP7940. Checking again in 3s."));  // Show error text
    delay(3000);                                                         // wait three seconds
  }  // of loop until device is located
  Serial.println(F("MCP7940 initialized."));
  while (!MCP7940.deviceStatus()) {  // Turn oscillator on if necessary
    Serial.println(F("Oscillator is off, turning it on."));
    bool deviceStatus = MCP7940.deviceStop();
    deviceStatus      = MCP7940.deviceStart();  // Start oscillator and return state
    if (!deviceStatus) {                        // If it didn't start
      Serial.println(F("Oscillator did not start, trying again."));  // Show error and
      delay(1000);                                                   // wait for a second
    }  // of if-then oscillator didn't start
  }    // of while the oscillator is off

  char     tempDate[13], tempTime[13];  // define temporary character arrays to hold values
  uint8_t  x, y, i, j;
  DateTime tempDt, now;
  bool     b;
  /*************************************************************************************************
  ** Check the adjust() functionality                                                             **
  *************************************************************************************************/
  strcpy_P(tempDate, PSTR(__DATE__));  // Copy values to buffer
  strcpy_P(tempTime, PSTR(__TIME__));  // Copy values to buffer
  tempDt = DateTime(tempDate, tempTime);
  MCP7940.adjust(tempDt);  // Set to library compile Date/Time
  now = MCP7940.now();
  if (tempDt.unixtime() != now.unixtime()) {
    Serial.println(F("!! Error in adjust() function"));
    Serial.print(F("!! compile date="));
    showTime(tempDt);
    Serial.print(F("\n!! now         ="));
    showTime(now);
    Serial.println();
  } else {
    Serial.print(F("adjust() successful\n"));
  }

  /*************************************************************************************************
  ** Check the Square Wave functionality                                                          **
  *************************************************************************************************/
  if (MCP7940.setSQWState(false) != false)
    Serial.println(F("!! Error in setSQWState"));
  else {
    if (MCP7940.setSQWState(true) != true)
      Serial.println(F("!! Error in setSQWState"));
    else
      Serial.println(F("setSQWState() successful"));
  }
  if (MCP7940.getSQWState() != true)
    Serial.println(F("!! Error in getSQWState()"));
  else
    Serial.println(F("getSQWState() successful"));

  b = MCP7940.setSQWSpeed(2);  // Set to 8.192KHz
  if (MCP7940.getSQWSpeed() != 2)
    Serial.println(F("!! Error in setSQWSpeed"));
  else
    Serial.println(F("setSQWSpeed() to 8.192KHz successful"));

  /*************************************************************************************************
  ** Check the calibrate() functionality                                                          **
  *************************************************************************************************/
  MCP7940.calibrate((int8_t)42);  // set the trim register value to 42
  if (MCP7940.getCalibrationTrim() != 42)
    Serial.println(F("!! Error setting/reading Trim"));
  else
    Serial.println(F("calibrate(int) successful"));
  MCP7940.calibrate();  // this should reset trim to 0
  if (MCP7940.getCalibrationTrim() != 0)
    Serial.println(F("!! Error resetting Trim"));
  else
    Serial.println(F("calibrate() successful"));
  DateTime now2(MCP7940.now().unixtime() + 1);  // add one second
  if (MCP7940.calibrate(now2) != 127)
    Serial.println(F("!! Error computing calibrate"));
  else
    Serial.println(F("calibrate(DateTime) successful"));
  //  int8_t xx = MCP7940.calibrate(f);

  /*************************************************************************************************
  ** Check the battery backup functionality                                                       **
  *************************************************************************************************/
  if (!MCP7940.getBattery())
    Serial.println(F("!! Error setting Battery Backup"));
  else
    Serial.println(F("setBattery() and getBattery() successful"));

  /*************************************************************************************************
  ** Check the alarm functionality                                                                **
  *************************************************************************************************/
  b = MCP7940.clearAlarm(0);
  MCP7940.setAlarmPolarity(true);
  now = MCP7940.now();
  DateTime alarmDate =
      DateTime(2000, now.month(), now.day(), now.hour(), now.minute(), ((now.second() + 2) % 60));
  MCP7940.setAlarm(0, 0, alarmDate, true);  // Match every minute at current seconds plus 2
  if (!MCP7940.getAlarmState(0)) Serial.println(F("!! Alarm not set correctly"));
  now2 = MCP7940.getAlarm(0, x);
  if (alarmDate.unixtime() == now2.unixtime() && x == 0) {
    Serial.println("Waiting for seconds to match alarm...");
    if (MCP7940.isAlarm(0))
      Serial.println(F("!! Alarm  turned on too early"));
    else {
      delay(3000);  // wait 3 seconds for alarm to trigger
      if (MCP7940.isAlarm(0))
        Serial.println(F("setAlarm() and isAlarm() successful"));
      else
        Serial.print(F("!! SetAlarm() failed"));
    }
  } else {
    Serial.print(F("!! Alarm set to "));
    showTime(alarmDate);
    Serial.print(F(" but read with getAlarm() as "));
    showTime(now2);
    Serial.print(F(", and alarm type is "));
    Serial.println(x);
  }

  /*************************************************************************************************
  ** Check the weekday functionality                                                              **
  *************************************************************************************************/
  MCP7940.weekdayWrite(5);
  if (MCP7940.weekdayRead() != 5) Serial.print(F("!! weekdayWrite() or weekdayRead() failed"));
  MCP7940.weekdayWrite(4);
  if (MCP7940.weekdayRead() != 4)
    Serial.print(F("!! weekdayWrite() or weekdayRead() failed"));
  else
    Serial.println(F("weekdayWrite() and WeekdayRead() successful"));

  /*************************************************************************************************
  ** Check the write and read SRAM functionality                                                  **
  *************************************************************************************************/
  sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  MCP7940.writeRAM(0, inputBuffer);
  char inputBuffer2[SPRINTF_BUFFER_SIZE];  // Buffer for sprintf()/sscanf()
  MCP7940.readRAM(0, inputBuffer2);
  if (strcmp(inputBuffer, inputBuffer2) != 0) {
    Serial.println(F("!! Error in writeRAM() or readRAM()"));
    Serial.println(inputBuffer);
    Serial.println(inputBuffer2);
  } else
    Serial.println(F("writeRAM() and readRAM() successful"));

  /*************************************************************************************************
  ** Check the set and get MFP functionality                                                      **
  *************************************************************************************************/
  MCP7940.setMFP(true);
  if (MCP7940.getMFP() != true) {
    MCP7940.setMFP(false);
    if (MCP7940.getMFP() == true) {
      Serial.println(F("!! 2 Error in SetMFP() or GetMFP()"));
    } else {
      Serial.println(F("SetMFP() and GetMFP() successful"));
    }
  } else
    Serial.println(F("!! 1 Error in SetMFP() or GetMFP()"));

  /*************************************************************************************************
  ** Check the setUnixTime functionality                                                          **
  *************************************************************************************************/
  uint32_t xxx = now.unixtime() - 42;
  MCP7940.setSetUnixTime(xxx);
  if (xxx == MCP7940.getSetUnixTime())
    Serial.print(F("setSetUnixTime() and getSetUnixTime() successful"));
  else
    Serial.print(F("!! Error in setSetUnixTime() or getSetUnixTime()"));

  b          = MCP7940.getPowerFail();
  b          = MCP7940.clearPowerFail();
  now        = MCP7940.getPowerDown();
  now        = MCP7940.getPowerUp();
  x          = MCP7940.calibrateOrAdjust(MCP7940.now());
  int32_t bi = MCP7940.getPPMDeviation(MCP7940.now());

}  // of method setup()

void loop() {
  /*!
    @brief  Arduino method corresponding to a "main()" call
  */
  static uint8_t mins;                 // store the minutes
  DateTime       now = MCP7940.now();  // get the current time
  if (mins != now.minute()) {          // Output if seconds have changed
    showTime(now);
    mins = now.minute();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // Toggle the LED
    Serial.println();
  }            // of if the seconds have changed
  delay(200);  // Do nothing for 100 milliseconds
}  // of method loop()

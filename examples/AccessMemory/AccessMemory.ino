/*! @file AccessMemory.ino

 @section AccessMemory_intro_section Description

Example program for using the MCP7940 library which demonstrates writing to and reading from the 64
bytes of RAM available to the user on the chip. The readRAM() and writeRAM() functions accept any
data type and this example will accept characters typed in on the serial port and write them
byte-by-bytte to RAM. Then the whole character array is read and displayed with one readRAM()
function call.\n\n The library as well as the most current version of this program is available on
GitHub at https://github.com/Zanduino/MCP7940 and a more detailed description of this program (and
the library) can be found at https://github.com/Zanduino/MCP7940/wiki/SetAndCalibrate.ino \n\n The
MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also
used the class definitions of the standard RTClib library from Adafruit/Jeelabs. The data sheet for
the MCP7940M is located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf. The
MCP7940N has extra functionality revolving around battery backup but this library's functionality
covers that chip as well.\n\n On-chip digital trimming can be used to adjust for frequency variance
caused by crystal tolerance and temperature. Since the chip only output full seconds, a calibration
cycle won't be effective until there is at least one second difference between the RTC time and real
time, and this might take a day or more to do, particularly if the RTC and crystal is already close
to being accurate.\n\n

@section AccessMemory_license GNU General Public License v3.0
This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

@section AccessMemory_author Author

Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin

@section AccessMemory_Versions Changelog

Version| Date       | Developer           | Comments
------ | ---------- | ------------------- | --------
1.0.3  | 2020-11-15 | SV-Zanshin          | Issue #50 - clang-formatted document
1.0.2  | 2019-01-20 | SV-Zanshin          | Changed comments to doxygen formatting
1.0.1  | 2018-07-07 | SV-Zanshin          | Changed code for compatability reasons to new library
1.0.0  | 2017-08-08 | SV-Zanshin          | Initial coding
*/
#include <MCP7940.h>  // Include the MCP7940 RTC library
/***************************************************************************************************
** Declare all program constants                                                                  **
***************************************************************************************************/
const uint32_t SERIAL_SPEED        = 115200;  ///< Set the baud rate for Serial I/O
const uint8_t  LED_PIN             = 13;      ///< Arduino built-in LED pin number
const uint8_t  SPRINTF_BUFFER_SIZE = 32;      ///< Buffer size for sprintf()
const uint8_t  MCP7940_MEMORY_SIZE = 64;      ///< Number of bytes in memory
/***************************************************************************************************
** Declare global variables and instantiate classes                                               **
***************************************************************************************************/
MCP7940_Class MCP7940;                           ///< Create an instance of the MCP7940
char          inputBuffer[SPRINTF_BUFFER_SIZE];  ///< Buffer for sprintf()/sscanf()

void setup() {
  /*!
      @brief  Arduino method called once upon start or restart.
  */
  Serial.begin(SERIAL_SPEED);
#ifdef __AVR_ATmega32U4__  // If on a 32U4 processor, wait 3s for serial interface to initialize
  delay(3000);
#endif
  Serial.print(F("\nStarting AccessMemory program\n"));
  Serial.print(F("- Compiled with c++ version "));
  Serial.print(F(__VERSION__));
  Serial.print(F("\n- On "));
  Serial.print(F(__DATE__));
  Serial.print(F(" at "));
  Serial.print(F(__TIME__));
  Serial.print(F("\n"));
  while (!MCP7940.begin())  // Initialize I2C communications with RTC
  {
    Serial.println(F("Unable to find MCP7940M. Checking again in 3s."));
    delay(3000);
  }  // of loop until device is located
  Serial.println(F("MCP7940 initialized."));
  while (!MCP7940.deviceStatus())  // Turn oscillator on if necessary
  {
    Serial.println(F("Oscillator is off, turning it on."));
    bool deviceStatus = MCP7940.deviceStart();  // Start oscillator and return new state
    if (!deviceStatus) {
      Serial.println(F("Oscillator did not start, trying again."));
      delay(1000);
    }                // of if-then oscillator didn't start
  }                  // of while the oscillator is off
  MCP7940.adjust();  // Set Date/time to that of compile
  Serial.println(F("Enter any text in the serial monitor and hit send"));
  pinMode(LED_PIN, OUTPUT);
}  // of method setup()

void loop() {
  /*!
      @brief  Arduino method called after setup() which loops forever
  */
  static uint8_t secs          = 0;
  static uint8_t memoryAddress = 0;
  DateTime       now           = MCP7940.now();  // get the current time
  if (secs != now.second())                      // Output only if the seconds have changed
  {
    sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
            now.hour(), now.minute(), now.second());
    if (secs % 10 == 0) Serial.println(inputBuffer);  // Only Display every 10 seconds
    secs = now.second();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // Toggle the LED
  }                                                // of if the seconds have changed
  if (Serial.available())                          // Check for serial port data
  {
    uint8_t dataByte = Serial.read();  // read the byte from serial
    if (dataByte == 0x0A)              // If the user hit return/enter
    {
      MCP7940.writeRAM(memoryAddress++, 0);        // Terminate the string
      char    dataBuffer[MCP7940_MEMORY_SIZE];     // Memory size value
      uint8_t x = MCP7940.readRAM(0, dataBuffer);  // Read all the memory
      Serial.print("Retrieved ");
      Serial.print(x);
      Serial.print(" bytes from memory,\nstring is \"");
      Serial.print(dataBuffer);
      Serial.println("\"");
      memoryAddress = 0;
    } else {
      if (dataByte != 13)  // Write to memory and increment
      {
        MCP7940.writeRAM(memoryAddress++, dataByte);  // counter if the value is not LF
      }  // of if-then we have a valid character to write
    }    // of if-then-else we have a LF
  }      // of if-then we have something to read from the serial port
}  // of method loop()

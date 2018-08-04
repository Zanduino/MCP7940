/*******************************************************************************************************************
** Example program for using the MCP7940 library and the Square Wave output on the MFP pin in order to set the    **
** calibration bits on the MCP7940. The library as well as the current version of this program is on GitHub at    **
** the URL https://github.com/SV-Zanshin/MCP7940 and a detailed description of this program can be found at       **
** https://github.com/SV-Zanshin/MCP7940/wiki/SquareWave.ino                                                      **
**                                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also used  **
** the class definitions of the standard RTClib library from Adafruit/Jeelabs.                                    **
**                                                                                                                **
** The data sheet for the MCP7940M is located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf.   **
** The MCP7940N has extra functionality revolving around battery backup but this library's functionality covers   **
** that chip as well.                                                                                             **
**                                                                                                                **
** The MCP7940 allows the Multifunction Pin (MFP) to output a square wave based on the 32KHz crystal oscillator.  **
** The square wave signal can be set to different frequencies, but the maximum speed 32KHz does not get adjusted  **
** by the TRIM function, so the next highest setting of 8.192kHz is used for the calibration process. The TRIM    **
** adjustment is set only once a minute, so a calibration cycle must be at least 60 seconds long. Any TRIM        **
** value is reset prior to the calibration process.                                                               **
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
** Vers.  Date       Developer                     Comments                                                       **
** ====== ========== ============================= ============================================================== **
** 1.0.0  2018-07-28 https://github.com/SV-Zanshin Initial coding                                                 **
**                                                                                                                **
*******************************************************************************************************************/
#include <MCP7940.h>                                                          // Include the MCP7940 RTC library  //
/*******************************************************************************************************************
** Declare all program constants and enumerated types                                                             **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED        = 115200;                                  // Set the baud rate for Serial I/O //
const uint8_t  MFP_PIN             =      2;                                  // Pin used for the MCP7940 MFP     //
const uint8_t  LED_PIN             =     13;                                  // Arduino built-in LED pin number  //
const uint8_t  SPRINTF_BUFFER_SIZE =     32;                                  // Buffer size for sprintf()        //
enum SquareWaveTypes { Hz1, kHz4, kHz8, kHz32, Hz64 };                        // Enumerate square wave frequencies//
const int32_t  MEASUREMENT_SECONDS  =    60;                                  // Number of seconds to measure     //
const uint8_t  AVERAGING            =    10;                                  // Number of averages to take       //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MCP7940_Class     MCP7940;                                                    // Create instance of the MCP7940M  //
char              inputBuffer[SPRINTF_BUFFER_SIZE];                           // Buffer for sprintf() / sscanf()  //
volatile uint64_t ticks    = 0;                                               // Number of High-Low or Low-High   //
int32_t           SQWSpeed = 0;                                               // Stores the current SQW Hertz Freq//

/*******************************************************************************************************************
** Declare interrupt handler for pin changes to the MFP_PIN                                                       **
*******************************************************************************************************************/
ISR (PCINT_vect) {                                                            // Called when pin goes from a low  //
  ticks++;                                                                    // Increment counter                //
} // of method PCINT_vect                                                     //                                  //

/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    delay(3000);                                                              // wait 3 seconds for the serial    //
  #endif                                                                      // interface to initialize          //
  Serial.print(F("\nStarting Arduino Clock Calibrate program\n"));            // Show program information         //
  Serial.print(F("- Compiled with c++ version "));                            //                                  //
  Serial.print(F(__VERSION__));                                               // Show compiler information        //
  Serial.print(F("\n- On "));                                                 //                                  //
  Serial.print(F(__DATE__));                                                  //                                  //
  Serial.print(F(" at "));                                                    //                                  //
  Serial.print(F(__TIME__));                                                  //                                  //
  Serial.print(F("\n"));                                                      //                                  //
  while (!MCP7940.begin()) {                                                  // Initialize RTC communications    //
    Serial.println(F("Unable to find MCP7940. Checking again in 3s."));       // Show error text                  //
    delay(3000);                                                              // wait a second                    //
  } // of loop until device is located                                        //                                  //
  Serial.println(F("- MCP7940 initialized."));                                //                                  //
  while (!MCP7940.deviceStatus()) {                                           // Turn oscillator on if necessary  //
    Serial.println(F("-  Oscillator is off, turning it on."));                //                                  //
    bool deviceStatus = MCP7940.deviceStart();                                // Start oscillator and return state//
    if (!deviceStatus) {                                                      // If it didn't start               //
      Serial.println(F("-  Oscillator did not start, trying again."));        // Show error and                   //
      delay(1000);                                                            // wait for a second                //
    } // of if-then oscillator didn't start                                   //                                  //
  } // of while the oscillator is off                                         //                                  //
  Serial.println("- Setting MCP7940 to date/time of library compile");        //                                  //
  MCP7940.adjust();                                                           // Use compile date/time for clock  //

  Serial.print("- Date/Time set to ");                                        //                                  //
  DateTime now = MCP7940.now();                                               // get the current time             //
  sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),            // Use sprintf() to pretty print    //
          now.month(), now.day(), now.hour(), now.minute(), now.second());    // date/time with leading zeros     //
  Serial.println(inputBuffer);                                                // Display the current date/time    //
  Serial.print(F("- Resetting TRIM value from "));                            //                                  //
  Serial.print(MCP7940.getCalibrationTrim());                                 //                                  //
  Serial.println(F(" to 0."));                                                //                                  //
  MCP7940.calibrate((int8_t)0);                                               // Reset the TRIM value to 0        //
  pinMode(MFP_PIN,INPUT);                                                     // MCP7940 Alarm MFP digital pin    //
  attachInterrupt(digitalPinToInterrupt(MFP_PIN),PCINT_vect,FALLING);         // Call interrupt when pin changes  //
  pinMode(LED_PIN,OUTPUT);                                                    // Declare built-in LED as output   //
  Serial.print("- SQW Output (Arduino pin ");                                 //                                  //
  Serial.print(MFP_PIN);                                                      //                                  //
  Serial.println(") to 8KHz");                                                //                                  //
  MCP7940.setSQWSpeed(kHz8);                                                  // Set the square wave pin          //
  MCP7940.setSQWState(true);                                                  // Turn the SQW on                  //
  SQWSpeed = MCP7940.getSQWSpeed();                                           // Get SQW Speed code               //
  switch (SQWSpeed) {                                                         // set variable to real SQW speed   //
    case 0: SQWSpeed =     1;                                                 //                                  //
            break;                                                            //                                  //
    case 1: SQWSpeed =  4096;                                                 //                                  //
            break;                                                            //                                  //
    case 2: SQWSpeed =  8192;                                                 //                                  //
            break;                                                            //                                  //
    case 3: SQWSpeed = 32768;                                                 //                                  //
            break;                                                            //                                  //
    case 4: SQWSpeed =    64;                                                 //                                  //
  } // of switch SQWSpeed value                                               //                                  //
  digitalWrite(LED_PIN,!digitalRead(LED_PIN));                                // Toggle LED pin                   //
  Serial.print(F("- "));                                                      //                                  //
  Serial.print(MEASUREMENT_SECONDS);                                          //                                  //
  Serial.println(F(" seconds for measurements...\n\n"));                      //                                  //
  noInterrupts();                                                             // Freeze interrupts to copy data   //
  ticks = 0;                                                                  // reset number of state changes    //
  interrupts();                                                               // re-enable interrupts             //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //

} // of method loop()                                                         //----------------------------------//

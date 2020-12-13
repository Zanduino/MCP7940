/***************************************************************************************************
** Example program for using the MCP7940 library and the Square Wave output on the MFP pin. The   **
** library as well as the current version of this program is on GitHub at the URL                 **
** https://github.com/Zanduino/MCP7940 and a detailed description of this program can be found  **
** at https://github.com/Zanduino/MCP7940/wiki/SquareWave.ino                                   **
**                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip    **
** and has also used the class definitions of the standard RTClib library from Adafruit/Jeelabs.  **
**                                                                                                **
** The data sheet for the MCP7940M is located at                                                  **
** http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf. The MCP7940N has extra          **
** functionality revolving around battery backup but this library's functionality covers that     **
** chip as well.                                                                                  **
**                                                                                                **
** The MCP7940 allows the Multifunction Pin (MFP) to output a square wave based on the 32KHz      **
** crystal oscillator. The square wave signal can have the following frequencies:                 **
**                                                                                                **
**  1.0   Hz                                                                                      **
**  4.096kHz                                                                                      **
**  8.192kHz                                                                                      **
** 32.768kHz                                                                                      **
** 64.0   Hz                                                                                      **
**                                                                                                **
** If the TRIM value is set this affects the square wave output frequency as well.                **
**                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the **
** GNU General Public License as published by the Free Software Foundation, either version 3 of   **
** the License, or (at your option) any later version. This program is distributed in the hope    **
** that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of         **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for   **
** more details. You should have received a copy of the GNU General Public License along with     **
** this program.  If not, see <http://www.gnu.org/licenses/>.                                     **
**                                                                                                **
** Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin                       **
**                                                                                                **
** Vers.  Date       Developer  Comments                                                          **
** ====== ========== ========== ================================================================= **
** 1.0.4  2020-11-16 SV-Zanshin Issue #50 - clang-formatted source code                           **
** 1.0.3  2019-01-20 SV-Zanshin Corrected Interrupt vector def. for non-esp32                     **
** 1.0.2  2018-09-24 SV-Zanshin Issue #34 Support for ESP32 Type interrupts                       **
** 1.0.1  2018-07-07 SV-Zanshin Added 64Hz support , changed to use interrupts                    **
** 1.0.0  2017-07-29 SV-Zanshin Initial coding                                                    **
**                                                                                                **
***************************************************************************************************/
#include "MCP7940.h"  // Include the MCP7940 RTC library
/***************************************************************************************************
** Declare all program constants and enumerated types                                             **
***************************************************************************************************/
const uint32_t SERIAL_SPEED        = 115200;            // Set the baud rate for Serial I/O
const uint8_t  MFP_PIN             = 2;                 // Pin used for the MCP7940 MFP
const uint8_t  LED_PIN             = 13;                // Arduino built-in LED pin number
const uint8_t  SPRINTF_BUFFER_SIZE = 32;                // Buffer size for sprintf()
enum SquareWaveTypes { Hz1, kHz4, kHz8, kHz32, Hz64 };  // Enumerate square wave frequencies
/***************************************************************************************************
** Declare global variables and instantiate classes                                               **
***************************************************************************************************/
MCP7940_Class     MCP7940;                           // Create instance of the MCP7940M
char              inputBuffer[SPRINTF_BUFFER_SIZE];  // Buffer for sprintf() / sscanf()
volatile uint64_t switches = 0;                      // Number of High-Low or Low-High

/***************************************************************************************************
** Declare interrupt handler for pin changes to the MFP_PIN                                       **
***************************************************************************************************/
#ifdef ESP32
portMUX_TYPE   mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR PCINT_vect()
#else
void PCINT_vect()
#endif
{
#ifdef ESP32
  portENTER_CRITICAL_ISR(&mux);
#endif
  switches++;
#ifdef ESP32
  portENTER_CRITICAL_ISR(&mux);
#endif
}  // of method PCINT_vect

/***************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only **
** called one time and then control goes to the main loop, which loop indefinately.               **
***************************************************************************************************/
void setup() {                                        // Arduino standard setup method
  Serial.begin(SERIAL_SPEED);                         // Start serial port at Baud rate
#ifdef __AVR_ATmega32U4__                             // If this is a 32U4 processor, then
  delay(3000);                                        // wait 3 seconds for the serial
#endif                                                // interface to initialize
  Serial.print(F("\nStarting SetAlarms program\n"));  // Show program information
  Serial.print(F("- Compiled with c++ version "));
  Serial.print(F(__VERSION__));  // Show compiler information
  Serial.print(F("\n- On "));
  Serial.print(F(__DATE__));
  Serial.print(F(" at "));
  Serial.print(F(__TIME__));
  Serial.print(F("\n"));
  while (!MCP7940.begin()) {  // Initialize RTC communications
    Serial.println(F("Unable to find MCP7940. Checking again in 3s."));  // Show error text
    delay(3000);                                                         // wait a second
  }  // of loop until device is located
  Serial.println(F("MCP7940 initialized."));
  while (!MCP7940.deviceStatus()) {  // Turn oscillator on if necessary
    Serial.println(F("Oscillator is off, turning it on."));
    bool deviceStatus = MCP7940.deviceStart();  // Start oscillator and return state
    if (!deviceStatus) {                        // If it didn't start
      Serial.println(F("Oscillator did not start, trying again."));  // Show error and
      delay(1000);                                                   // wait for a second
    }  // of if-then oscillator didn't start
  }    // of while the oscillator is off
  Serial.println("Setting MCP7940M to date/time of library compile");
  MCP7940.adjust();  // Use compile date/time for clock
  Serial.print("Date/Time set to ");
  DateTime now = MCP7940.now();  // get the current time
  sprintf(inputBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(),  // Use sprintf() to pretty print
          now.month(), now.day(), now.hour(), now.minute(),
          now.second());        // date/time with leading zeros
  Serial.println(inputBuffer);  // Display the current date/time
  pinMode(MFP_PIN, INPUT);      // MCP7940 Alarm MFP digital pin
  attachInterrupt(digitalPinToInterrupt(MFP_PIN), PCINT_vect,
                  FALLING);  // Call interrupt when pin changes
  pinMode(LED_PIN, OUTPUT);  // Declare built-in LED as output
  Serial.println("Setting SQW to 64Hz and linking to LED");
  MCP7940.setSQWState(true);   // Turn the SQW on
  MCP7940.setSQWSpeed(kHz32);  // Set the square wave pin
}  // of method setup()
/***************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating    **
***************************************************************************************************/
void loop() {
  static uint32_t startMillis = millis();       // Store the starting time
  digitalWrite(LED_PIN, digitalRead(MFP_PIN));  // Make LED mirror MFP pin
  if (millis() - startMillis > 10000) {         // Show results every 10 seconds
    Serial.print("Square Wave changed ");
    Serial.print((uint32_t)(switches));
    Serial.print(" times in 10s = ~");
    Serial.print((uint32_t)(switches / 10));  // Divide by 10 seconds
    Serial.println("Hz");
    startMillis = millis();                                 // Set time to current time
    switches    = 0;                                        // reset number of state changes
  }                                                         // of if-then 10 seconds have elapsed
  if (MCP7940.getSQWState() && millis() > 60000) {          // If 60 seconds have passed and
    Serial.println("Turning off SQW pin after 1 minute.");  // SQW is still enabled then turn
    MCP7940.setSQWState(false);                             // it off. LED will stop blinking
  }                                                         // of if-then >10s and pin active
}  // of method loop()

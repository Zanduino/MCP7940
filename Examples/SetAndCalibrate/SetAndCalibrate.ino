/*******************************************************************************************************************
** Example program for using the MCP7940 library which allows access to the MCP7940N/M real-time-clock chip. The  **
** library as well as the most current version of this program is available at GitHub using the address           **
** https://github.com/SV-Zanshin/MCP7940 and a more detailed description of this program (and the library) can be **
** found at https://github.com/SV-Zanshin/MCP7940/wiki/SetAndCalibrate.ino                                        **
**                                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also used  **
** the class definitions of the standard RTClib library from Adafruit/Jeelabs. The data sheet for the MCP7940M is **
** located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf. The MCP7940N has extra functionality **
** revolving around battery backup but this library's functionality covers that chip as well.                     **
**                                                                                                                **
** On-chip digital trimming can be used to adjust for frequency variance caused by crystal tolerance and          **
** temperature. Since the chip only output full seconds, a calibration cycle won't be effective until there is at **
** least one second difference between the RTC time and real time, and this might take a day or more to do,       **
** particularly if the RTC and crystal is already close to being accurate.                                        **
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
** 1.0.1  2017-07-29 Arnd@SV-Zanshin.Com Cleaned up comments and code                                             **
** 1.0.0  2017-07-23 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include <MCP7940.h>                                                          // Include the MCP7940 RTC library  //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED        = 115200;                                  // Set the baud rate for Serial I/O //
const uint8_t  LED_PIN             =     13;                                  // Arduino built-in LED pin number  //
const uint8_t  SPRINTF_BUFFER_SIZE =     32;                                  // Buffer size for sprintf()        //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MCP7940_Class MCP7940;                                                        // Create an instance of the MCP7940//
char          inputBuffer[SPRINTF_BUFFER_SIZE];                               // Buffer for sprintf()/sscanf()    //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    delay(3000);                                                              // wait 3 seconds for the serial    //
  #endif                                                                      // interface to initialize          //
  Serial.print(F("\nStarting SetAndCalibrate program\n"));                    // Show program information         //
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
  MCP7940.adjust();                                                           // Set to library compile Date/Time //
  Serial.println(F("Enter the following serial commands:"));                  //                                  //
  Serial.println(F("SETDATE yyyy-mm-dd hh:mm:ss"));                           //                                  //
  Serial.println(F("CALDATE yyyy-mm-dd hh:mm:ss"));                           //                                  //
  pinMode(LED_PIN,OUTPUT);                                                    // Declare built-in LED as output   //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** Method readCommand(). This function checks the serial port to see if there has been any input. If there is data**
** it is read until a terminator is discovered and then the command is parsed and acted upon                      **
*******************************************************************************************************************/
void readCommand() {                                                          //                                  //
  static uint8_t inputBytes = 0;                                              // Variable for buffer position     //
  while (Serial.available()) {                                                // Loop while incoming serial data  //
    inputBuffer[inputBytes] = Serial.read();                                  // Get the next byte of data        //
    if (inputBuffer[inputBytes]!='\n' && inputBytes<SPRINTF_BUFFER_SIZE)      // keep on reading until a newline  //
      inputBytes++;                                                           // shows up or the buffer is full   //
    else {                                                                    //                                  //
      inputBuffer[inputBytes] = 0;                                            // Add the termination character    //
      for (uint8_t i=0;i<inputBytes;i++)                                      // Convert the whole input buffer   //
        inputBuffer[i] = toupper(inputBuffer[i]);                             // to uppercase characters          //
      Serial.print(F("\nCommand \""));                                        //                                  //
      Serial.write(inputBuffer);                                              //                                  //
      Serial.print(F("\" received.\n"));                                      //                                  //
      /*************************************************************************************************************
      ** Parse the single-line command and perform the appropriate action. The current list of commands           **
      ** understood are as follows:                                                                               **
      **                                                                                                          **
      ** SETDATE      - Set the device time                                                                       **
      ** CALDATE      - Calibrate device time                                                                     **
      **                                                                                                          **
      *************************************************************************************************************/
      enum commands { SetDate, CalDate, Unknown_Command };                    // of commands enumerated type      //
      commands command;                                                       // declare enumerated type          //
      char workBuffer[10];                                                    // Buffer to hold string compare    //
      sscanf(inputBuffer,"%s %*",workBuffer);                                 // Parse the string for first word  //
      if      (!strcmp(workBuffer,"SETDATE" )) command = SetDate;             // Set command number when found    //
      else if (!strcmp(workBuffer,"CALDATE" )) command = CalDate;             // Set command number when found    //
      else command = Unknown_Command;                                         // Otherwise set to not found       //
      uint16_t tokens,year,month,day,hour,minute,second;                      // Variables to hold parsed dt/tm   //
      switch (command) {                                                      // Action depending upon command    //
        /***********************************************************************************************************
        ** Set the device time and date                                                                           **
        ***********************************************************************************************************/
        case SetDate:                                                         // Set the RTC date/time            //
          tokens = sscanf(inputBuffer,"%*s %d-%d-%d %d:%d:%d;",               // Use sscanf() to parse the date/  //
                          &year,&month,&day,&hour,&minute,&second);           // time into variables              //
          if (tokens!=6)                                                      // Check to see if it was parsed    //
            Serial.print(F("Unable to parse date/time\n"));                   //                                  //
          else {                                                              //                                  //
            MCP7940.adjust(DateTime(year,month,day,hour,minute,second));      // Adjust the RTC date/time         //
            Serial.print(F("Date has been set."));                            //                                  //
          } // of if-then-else the date could be parsed                       //                                  //
          break;                                                              //                                  //
        /***********************************************************************************************************
        ** Calibrate the RTC and reset the time                                                                   **
        ***********************************************************************************************************/
        case CalDate:                                                         // Calibrate the RTC                //
          tokens = sscanf(inputBuffer,"%*s %d-%d-%d %d:%d:%d;",               // Use sscanf() to parse the date/  //
                          &year,&month,&day,&hour,&minute,&second);           // time into variables              //
          if (tokens!=6)                                                      // Check to see if it was parsed    //
            Serial.print(F("Unable to parse date/time\n"));                   //                                  //
          else {                                                              //                                  //
            int8_t trim = MCP7940.calibrate(DateTime(year,month,day,          // Calibrate the crystal and return //
                                                     hour,minute,second));    // the new trim offset value        //
            Serial.print(F("Trim value set to "));                            //                                  //
            Serial.print(trim*2);                                             // Each trim tick is 2 cycles       //
            Serial.println(F(" clock cycles every minute"));                  //                                  //
          } // of if-then-else the date could be parsed                       //                                  //
          break;                                                              //                                  //
        /***********************************************************************************************************
        ** Unknown command                                                                                        **
        ***********************************************************************************************************/
        case Unknown_Command:                                                 // Show options on bad command      //
        default:                                                              //                                  //
          Serial.println(F("Unknown command. Valid commands are:"));          //                                  //
          Serial.println(F("SETDATE yyyy-mm-dd hh:mm:ss"));                   //                                  //
          Serial.println(F("CALDATE yyyy-mm-dd hh:mm:ss"));                   //                                  //
      } // of switch statement to execute commands                            //                                  //
      inputBytes = 0; // reset the counter                                    //                                  //
    } // of if-then-else we've received full command                          //                                  //
  } // of if-then there is something in our input buffer                      //                                  //
} // of method readCommand                                                    //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint8_t secs;                                                        // store the seconds value          //
  DateTime now = MCP7940.now();                                               // get the current time             //
  if (secs != now.second()) {                                                 // Output if seconds have changed   //
    sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),          // Use sprintf() to pretty print    //
            now.month(), now.day(), now.hour(), now.minute(), now.second());  // date/time with leading zeros     //
    Serial.println(inputBuffer);                                              // Display the current date/time    //
    secs = now.second();                                                      // Set the counter variable         //
    digitalWrite(LED_PIN,!digitalRead(LED_PIN));                              // Toggle the LED                   //
  } // of if the seconds have changed                                         //                                  //
  readCommand();                                                              // See if serial port had incoming  //
} // of method loop()                                                         //----------------------------------//
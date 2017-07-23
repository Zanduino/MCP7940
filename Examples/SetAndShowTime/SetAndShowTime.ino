/*******************************************************************************************************************
** Example program for using the MCP7940 library which allows access to the MCP7940M real-time-clock chip.        **
**                                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also used  **
** the class definitions from the standard RTClib library from Adafruit/Jeelabs                                   **
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
#include <MCP7940.h>                                                          // Include the library              //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED = 115200;                                         // Set the baud rate for Serial I/O //
const uint8_t  LED_PIN      =     13;
const uint8_t  CLOCK_PIN    =      4;
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MCP7940_Class MCP7940;

/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  delay(3000);                                                                // Some Atmel Serials need time     //
  Serial.println(F("Starting SetAndShowTime program"));                       //                                  //
  while (!MCP7940.begin()) {                                                  // Start communication with RTC     //
    Serial.println(F("Unable to find MCP7940M. Checking again in 5s."));      // Show error text                  //
    delay(5000);                                                              // wait                             //
  } // of loop until device is located                                        //                                  //
  Serial.println(F("MCP7940 initialized.\n\n"));                              //                                  //
  Serial.print("Turned on, state is ");Serial.println(MCP7940.deviceStart()); // Turn on oscillator               //
  MCP7940.adjust();                                                           // Set clock to compile time        //
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,false);
  pinMode(CLOCK_PIN,INPUT);
  digitalWrite(LED_PIN,digitalRead(CLOCK_PIN));
  DateTime now = MCP7940.now();
  now = now + TimeSpan(0,0,0,15);
  MCP7940.setAlarm(0,7,now);
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint8_t secs;
  DateTime now = MCP7940.now();
  digitalWrite(LED_PIN,digitalRead(CLOCK_PIN));
  if (!digitalRead(CLOCK_PIN)) {
    delay(1000);
    MCP7940.clearAlarm(0);
  }
  if (secs != now.second()) {
    Serial.print(now.year(), DEC);
    Serial.print('-');
    Serial.print(now.month(), DEC);
    Serial.print('-');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println(")");
    secs = now.second();
  }    
   readCommand();
} // of method loop()                                                         //----------------------------------//

void readCommand() {
   const uint8_t SPRINTF_BUFFER_SIZE = 32;
   static uint8_t inputBytes = 0;
   static char    inputBuffer[SPRINTF_BUFFER_SIZE];
   
   while (Serial.available()) {
      inputBuffer[inputBytes] = Serial.read();
      if (inputBuffer[inputBytes]!='\n' && inputBytes<SPRINTF_BUFFER_SIZE) inputBytes++; // keep on reading until a newline shows up or the buffer is full
      else {
         inputBuffer[inputBytes] = 0; // set the termination character
         for (uint8_t i=0;i<inputBytes;i++) inputBuffer[i] = toupper(inputBuffer[i]); // convert to uppercase
         Serial.print("\nCommand \"");Serial.write(inputBuffer);Serial.print("\" received.\n");
         /****************************************************************************************************************************************************************
         ** Parse the single-line command and perform the appropriate action. The current list of commands understand are as follows:                                   **
         **                                                                                                                                                             **
         ** SETDATE      - Set the device time                                                                                                                          **
         ** CALDATE      - Calibrate device time                                                                                                                        **
         ****************************************************************************************************************************************************************/
         enum commands { SetDate, CalDate, Unknown_Command }; // of commands enumerated type 
         commands command; // declare the enumerated type to hold the command        
         char workBuffer[10];
         sscanf(inputBuffer,"%s %*",workBuffer);
         if      (!strcmp(workBuffer,"SETDATE" )) command = SetDate;
         else if (!strcmp(workBuffer,"CALDATE" )) command = CalDate;
         else command = Unknown_Command;
         uint16_t tokens,year,month,day,hour,minute,second;
         switch (command) {
            /*************************************************************************************************************************************************************
            ** Set the device time and date                                                                                                                             **
            *************************************************************************************************************************************************************/
            case SetDate:
               Serial.print("Date set to \"");
               tokens = sscanf(inputBuffer,"%*s %d-%d-%d %d:%d:%d;",&year,&month,&day,&hour,&minute,&second);
               if (tokens!=6) Serial.print("Unable to parse\n"); else MCP7940.adjust(DateTime(year,month,day,hour,minute,second));
               break;
            /*************************************************************************************************************************************************************
            ** Dump a file contents to the serial port                                                                                                                  **
            *************************************************************************************************************************************************************/
            case CalDate:
               tokens = sscanf(inputBuffer,"%*s %d-%d-%d %d:%d:%d;",&year,&month,&day,&hour,&minute,&second);
               if (tokens!=6) Serial.print("Unable to parse\n"); 
               else {
                 int8_t trim = MCP7940.calibrate(DateTime(year,month,day,hour,minute,second));
                 Serial.print("Trim value set to ");Serial.print(trim);Serial.println(" clock cycles every minute");
               }
               break;
            /*************************************************************************************************************************************************************
            ** Unknown command                                                                                                                                          **
            *************************************************************************************************************************************************************/
            case Unknown_Command:
            default:
               Serial.print("Unknown command. Valid commands are:\n");
               Serial.print("SETDATE yyyy-mm-dd hh:mm:ss\n");
               Serial.print("CALDATE yyyy-mm-dd hh:mm:ss\n");
         } // of switch statement to execute commands
         inputBytes = 0; // reset the counter
      } // of if-then-else we've received full command
   } // of if-then there is something in our input buffer
} // of method readCommand

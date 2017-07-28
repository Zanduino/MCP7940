/*******************************************************************************************************************
** Example program for using the MCP7940 library and the adafruit GPS Breakout V3 to calibrate the RTC.           **
**                                                                                                                **
** The MCP7940 library uses the standard SPI Wire library for communications with the RTC chip and has also used  **
** the class definitions of the standard RTClib library from Adafruit/Jeelabs.                                    **
**                                                                                                                **
** The data sheet for the MCP7940M is located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf.   **
** The description for the GPS from adafruit can be found at https://learn.adafruit.com/adafruit-ultimate-gps/.   **
**                                                                                                                **
** Since the GPS uses TTL level serial data it needs a communications port, and the standard port is already in   **
** use for our I/O. The SoftwareSerial library has trouble with the long NMEA sentences, so this sketch is written**
** for Arduino devices that have more than one hardware serial port. This sketch runs on an ATMEGA board.         **
**                                                                                                                **
** The program uses the PPS (Pulse-Per-Second) pin which pulses either every second or at the FIX_CTL speed at    **
** which the GPS is set. This is the accurate time signal and is compared to the time that the RTC returns. The   **
** difference between the two times is measured in microseconds and once a direction of divergence is determined  **
** over a number of measurements then the offset is adjusted accordingly. This is repeated until the speed of     **
** divergence is minimized. This method is a bit more complex then merely comparing the times of the two clocks   **
** and computing an offset factor when they differ by a second or more - and it is much faster as well            **
**                                                                                                                **
** Pin change interrupts are used to get accurate timing results, but the Adafruit GPS breakout library uses      **
** interrupts as well so in this sketch the GPS library isn't used and the default 9600 baud and standard message **
** settings of the GPS are used and only the time signal is read from Serial2 to set the RTC                      **
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
** 1.0.2  2017-07-28 Arnd@SV-Zanshin.Com Replaced GPS library and added interrupts for accuracy                   **
** 1.0.1  2017-07-27 Arnd@SV-Zanshin.Com Continued coding                                                         **
** 1.0.0  2017-07-23 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include <MCP7940.h>                                                          // Include the MCP7940 RTC library  //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED            = 115200;                              // Set the baud rate for Serial I/O //
const uint8_t  SPRINTF_BUFFER_SIZE     =     64;                              // Buffer size for sprintf()        //
const uint16_t GPS_BAUD_RATE           =   9600;                              // GPS Module serial port baud rate //
const uint16_t GPS_SENTENCE_SIZE       =    132;                              // Buffer size for GPS Sentences    //
const uint32_t DISPLAY_INTERVAL_MILLIS = 600000;                              // milliseconds between outputs     //
const uint8_t  LED_PIN                 =     13;                              // Built in LED                     //
const uint8_t  MFP_PIN                 =      4;                              // Pin used for the MCP7940 MFP     //
const uint8_t  PPS_PIN                 =    A15;                              // GPS Pulse-Per-Second             //
const uint8_t  AL0_INTERVAL_MINUTES    =      1;                              // RTC Alarm 0 interval in minutes  //
const int8_t   UTC_OFFSET              =      2;                              // Offset from UTC (or GMT)         //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MCP7940_Class MCP7940;                                                        // Create an instance of the MCP7940//
char          inputBuffer[SPRINTF_BUFFER_SIZE];                               // Buffer for sprintf()/sscanf()    //
int8_t        trimSetting;                                                    // Current MCP7940 trim value       //
volatile uint32_t GPSMicros,RTCMicros;                                        // Last trigger time for GPS and RTC//

/*******************************************************************************************************************
** Interrupt Service Routine handler for pin change interrupt PCINT1_vect for pins D8-D13. Sets the global var to **
** current microseconds for the RTC. It then sets the RTC alarm to the next value which also resets the interrupt **
*******************************************************************************************************************/
/*
ISR (PCINT0_vect) {                                                           // Interrupt handler for RTC clock  //
  if (micros()-RTCMicros>1500000) {
    RTCMicros = micros();                       // Only set if more then 1.5secs    //
    Serial.print("BINGO");
  }    
} // of interrupt service routine for Interrupt 1                             //                                  //

*/

/*******************************************************************************************************************
** Method to read data from the GPS port and then use the Adafruit library to parse the date and time. The GPS    **
** outputs several sentence types and we are using only the one starting with $GPRMC. We parse the time and date  **
** from the sentence in addition to the quality flag. This is either "V"oid or "A"ctive and the program keeps on  **
** looping until a valid signal is read and parsed                                                                **
*******************************************************************************************************************/
DateTime readGPS() {                                                          // Read from the GPS                //
  char gpsBuffer[GPS_SENTENCE_SIZE];                                          // Store GPS Sentences              //
  uint8_t gpsBufferIndex = 0;                                                 // Index into GPS buffer            //
  uint16_t hh,nn,ss,ms,dd,mm,yy;                                              // Store the parsed GPRMC elements  //
  char validity;                                                              // Store whether the fix is valid   //
  DateTime gpsTime;                                                           // Define the return DateTime       //
  while(true) {                                                               // Loop until we have valid fix     //
    if (Serial2.available()) {                                                // If we have something to read     //
      gpsBuffer[gpsBufferIndex] = Serial2.read();                             // Add the next character to buffer //
      if (gpsBuffer[gpsBufferIndex]==10) {                                    // Once we have a LF character      //
        gpsBuffer[gpsBufferIndex] = 0;                                        // Replace it with a 0x00 terminator//
        if (sscanf(gpsBuffer,"$GPRMC,%2d%2d%2d.%3d,%1s,%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%2d%2d%2d%*",
        &hh,&nn,&ss,&ms,&validity,&dd,&mm,&yy)==8 && validity=='A') {         // Parse $GPRMC into variables      //
          gpsTime = DateTime( yy, mm, dd, hh, nn, ss );                      // Store in return value            //
          return(gpsTime);                                                   // return our DateTime to caller    //
        } // of if-then we parsed the $GPRMC sentence                         //                                  //
        gpsBufferIndex = GPS_SENTENCE_SIZE-1;                                 // Reset for next sentence to read  //
      } // of if-then we have a LF                                            //                                  //
      if (++gpsBufferIndex==GPS_SENTENCE_SIZE) gpsBufferIndex=0;              // Start at beginning on overflow   //
    } // of if-then there is a character in the buffer                        //                                  //
  } // loop forever until we force exit                                       //                                  //
} // of method readGPS()                                                      //                                  //

/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  #if not (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__))        // This sketch needs to run on an   //
    #error This program needs to run on an Arduino Mega due to Serial2        // Arduino Mega due to serial ports //
  #endif                                                                      //                                  //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  Serial2.begin(GPS_BAUD_RATE);                                               // Start GPS serial port            //
  sprintf(inputBuffer,"Starting CalibrateFromGPS program\n- c++ compiler version %s\n- Compiled on %s at %s\n",
          __VERSION__,__DATE__,__TIME__);                                     //                                  //
  Serial.print(inputBuffer);                                                  // Display program and compile date //
  sprintf(inputBuffer,"- Arduino IDE V%d\, CPU Frequency %dmHz\n",            //                                  //
          ARDUINO, F_CPU/1000000);                                            //                                  //
  Serial.print(inputBuffer);                                                  // Display IDE and speed information//
  Serial.print(F("- Initializing MCP7940 Real-Time-Clock\n"));              //                                  //
  while (!MCP7940.begin()) {                                                  // Initialize RTC communications    //
    Serial.println(F("-  Unable to find MCP7940M. Waiting 3 seconds."));      // Show error text                  //
    delay(3000);                                                              // wait three seconds               //
  } // of loop until device is located                                        //                                  //
  Serial.print(F("- Current TRIM value is "));                                //                                  //
  trimSetting = MCP7940.getCalibrationTrim();                                 // Retrieve TRIM register setting   //
  Serial.print(trimSetting);                                                  //                                  //
  Serial.print(F(" ("));                                                      //                                  //
  Serial.print(trimSetting*2);                                                //                                  //
  Serial.println(F(" clock cycles per minute)"));                             //                                  //
  Serial.println("- Getting GPS fix with Date/Time");                         //                                  //
  DateTime GPStime = readGPS()+TimeSpan(0,UTC_OFFSET,0,0);                    // Adjust for UTC                   //
  MCP7940.adjust(GPStime);                                                    //                                  //
  MCP7940.setAlarm(0,0,GPStime);                                              // Set alarm to go off every minute //
  MCP7940.setAlarm(0,7,GPStime+TimeSpan(0,0,0,15));
  Serial.println(F("- Setting RTC alarm to every minute."));                  //                                  //
  Serial.print(F("- Setting MCP7940 using GPS time to "));                    //                                  //
  sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", GPStime.year(),        // Use sprintf() to pretty print    //
  GPStime.month(), GPStime.day(), GPStime.hour(), GPStime.minute(),   // date/time with leading zeros     //
  GPStime.second());                                                  //                                  //
  Serial.println(inputBuffer);                                                // Display the current date/time    //

uint8_t x;
GPStime=MCP7940.getAlarm(0,x);
sprintf(inputBuffer,"*%04d-%02d-%02d %02d:%02d:%02d", GPStime.year(),        // Use sprintf() to pretty print    //
GPStime.month(), GPStime.day(), GPStime.hour(), GPStime.minute(),   // date/time with leading zeros     //
GPStime.second());                                                  //                                  //
Serial.println(inputBuffer);                                                // Display the current date/time    //
Serial.println(x,BIN);

  pinMode(MFP_PIN,INPUT);                                                     // MCP7940 Alarm MFP digital pin    //
  pinMode(PPS_PIN,INPUT);                                                     // Pulses to 2.8V every cycle       //
  pinMode(LED_PIN,OUTPUT);                                                    // Green built in LED on Arduino    //
  digitalWrite(LED_PIN,LOW);                                                  // Turn off the LED light           //
  Serial.println("Rdgs  GPS Date / time    Avg ?s 5 min  ");                  // Display the output header lines  //
  Serial.println("==== =================== ====== ====== ");                  //                                  //
//  *digitalPinToPCMSK(MFP_PIN) |= bit (digitalPinToPCMSKbit(MFP_PIN));         // enable pin change int. on pin    //
//  PCIFR  |= bit (digitalPinToPCICRbit(MFP_PIN));                              // clear any outstanding interrupt  //
//  PCICR  |= bit (digitalPinToPCICRbit(MFP_PIN));                              // enable interrupt for the group   //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint32_t alarmMicros, gpsMicros;                                     // Alarm and GPS Microseconds       //
  static DateTime now,mcp;

while(1){
 if (alarmMicros!=RTCMicros) {
   alarmMicros=RTCMicros;
   digitalWrite(LED_PIN,!digitalRead(LED_PIN)); // toggle
 }
 if (!MCP7940.getMFP()) Serial.println("NOT ASSERTED");
 now = MCP7940.now();                                             // Get the current RTC time         //
 sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d\n",      // Use sprintf() to pretty print    //
 now.year(),now.month(),now.day(),now.hour(),now.minute(),// output with leading zeroes       //
 now.second()    );
 Serial.print(inputBuffer);                                                 // Output the sprintf buffer       //
delay(1000);
}  



  static uint16_t readings        =          0;                               // Number of readings taken         //
  static uint64_t totalDelta      =          0;                               // Total of all deltas read         //
  static uint16_t last5Readings[5];

  if (!digitalRead(MFP_PIN)) {                                                // If we have an alarm triggered    //
    alarmMicros = micros();                                                   // Set microseconds                 //
//    MCP7940.setAlarm(0,7,MCP7940.now()+TimeSpan(0,0,AL0_INTERVAL_MINUTES,0)); // Set alarm to go off in n-minutes //
    now = MCP7940.now();                                             // Get the current RTC time         //
    readings++;                                                               // increment the number of readings //
    totalDelta += alarmMicros-gpsMicros;                                      // Add the last delta to total      //
    uint16_t average = totalDelta / readings;
    last5Readings[readings%5]=average;
    uint16_t Avg5min = 0;
    for(uint8_t i=0;i<5;i++) Avg5min += last5Readings[i]/5;
    sprintf(inputBuffer,"%04d %04d-%02d-%02d %02d:%02d:%02d %06:2d %06:2d ",      // Use sprintf() to pretty print    //
            readings,now.year(),now.month(),now.day(),now.hour(),now.minute(),// output with leading zeroes       //
            now.second(), average, Avg5min    );
    Serial.print(inputBuffer);                                                 // Output the sprintf buffer       //
    Serial.print(" ");
    Serial.print(alarmMicros);
    Serial.print(" ");
    Serial.print(gpsMicros);
    Serial.print(" ");
    Serial.print(alarmMicros-gpsMicros);
    Serial.println(" ");
  } // of if-then the alarm has been raised
  now = readGPS();
  if (analogRead(PPS_PIN)>600) {                                              // If the GPS PPS pin is set then   //
    gpsMicros = micros();                                                     // save the microsecond time        //
    digitalWrite(LED_PIN,HIGH);                              // Toggle LED                       //
  } else digitalWrite(LED_PIN,LOW);                              // Toggle LED                       //
/*
  static uint8_t secs;                                                        // store the seconds value          //
  static DateTime RTCTime,GPSTime;                                            // Declare RTC and GPS times        //
  static uint32_t nextDisplayTime = millis()+DISPLAY_INTERVAL_MILLIS;         // 10 Minutes between output        //
  GPSTime = readGPS();                                                        // get the GPS time                 //
  RTCTime = MCP7940.now();                                                    // get the current time             //
  if(GPSTime.second()!=RTCTime.second()) {                                    // just compare the seconds portion //
    Serial.println("Discrepancy of 1 or more seconds detected!");             //                                  //
    sprintf(inputBuffer,"GPS: %04d-%02d-%02d %02d:%02d:%02d", GPSTime.year(), // Use sprintf() to pretty print    //
            GPSTime.month(), GPSTime.day(), GPSTime.hour(), GPSTime.minute(), // date/time with leading zeroes    //
            GPSTime.second());                                                //                                  //
    Serial.println(inputBuffer);                                              // Display the current date/time    //
    sprintf(inputBuffer,"RTC: %04d-%02d-%02d %02d:%02d:%02d", RTCTime.year(), // Use sprintf() to pretty print    //
            RTCTime.month(), RTCTime.day(), RTCTime.hour(), RTCTime.minute(), // date/time with leading zeroes    //
            RTCTime.second());                                                //                                  //
    Serial.println(inputBuffer);                                              // Display the current date/time    //
    Serial.print("Calibration changes trim from ");                           //                                  //
    Serial.print(MCP7940.getCalibrationTrim());                               //                                  //
    Serial.print(" to ");                                                     //                                  //
    Serial.println(MCP7940.calibrate(GPSTime));                               //                                  //
  } // of if-then we have difference                                          //                                  //
  if(millis()>nextDisplayTime) {                                              // Display time once in a while     //
    sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", GPSTime.year(),      // Use sprintf() to pretty print    //
            GPSTime.month(), GPSTime.day(), GPSTime.hour(), GPSTime.minute(), // date/time with leading zeroes    //
            GPSTime.second());                                                //                                  //
    Serial.println(inputBuffer);                                              // Display the current date/time    //
    nextDisplayTime = millis()+DISPLAY_INTERVAL_MILLIS;                       // Set next display time            //
  } // of if display time has elapsed                                         //                                  //
  */
} // of method loop()                                                         //----------------------------------//

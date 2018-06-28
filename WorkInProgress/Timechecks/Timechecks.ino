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
const uint8_t  MFP_PIN                 =      2;                              // Pin used for the MCP7940 MFP     //
const uint8_t  PPS_PIN                 =      3;                              // GPS Pulse-Per-Second             //
const uint8_t  AL0_INTERVAL_MINUTES    =      1;                              // RTC Alarm 0 interval in minutes  //
const int8_t   UTC_OFFSET              =      2;                              // Offset from UTC (or GMT)         //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MCP7940_Class     MCP7940;                                                    // Create an instance of the MCP7940//
char              inputBuffer[SPRINTF_BUFFER_SIZE];                           // Buffer for sprintf()/sscanf()    //
int8_t            trimSetting;                                                // Current MCP7940 trim value       //
volatile uint32_t GPSMicros,RTCMicros;                                        // Last trigger time for GPS and RTC//
DateTime          now;                                                        // Used to store the current time   //
/*******************************************************************************************************************
** Interrupt Service Routine handler for falling pin change interrupt on the RTC. Set the current microseconds and**
** return. Since this is set to a falling pin, it will only trigger once and then the pin is reset to high from   **
** the main loop when the alarm is reset.                                                                         **
*******************************************************************************************************************/
RTCTick() {                                                                   // Triggered on falling pin         //
  RTCMicros = micros();                                                       // Store the current microseconds   //
} // of method RTCTick()                                                      //                                  //
/*******************************************************************************************************************
** Interrupt Service Routine handler for rising pin change interrupt on the GPS. Set the current microseconds and **
** return. Since this is set to a rising pin, it will only trigger once per PPS cycle                             **
*******************************************************************************************************************/
GPSTick() {                                                                   // Triggered on rising  pin         //
  GPSMicros = micros();                                                       // Store the current microseconds   //
  digitalWrite(LED_PIN,!digitalRead(LED_PIN));                                // Toggle LED to show we are ticking//
} // of method GPSTick                                                        //                                  //
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
          &hh,&nn,&ss,&ms,&validity,&dd,&mm,&yy)==8 && validity=='A') {       // Parse $GPRMC into variables      //
          gpsTime = DateTime( yy, mm, dd, hh, nn, ss );                       // Store in return value            //
          return(gpsTime);                                                    // return our DateTime to caller    //
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
  pinMode(MFP_PIN,INPUT);                                                     // MCP7940 Alarm MFP digital pin    //
  pinMode(PPS_PIN,INPUT);                                                     // Pulses to 2.8V every cycle       //
  pinMode(LED_PIN,OUTPUT);                                                    // Green built in LED on Arduino    //
  digitalWrite(LED_PIN,LOW);                                                  // Turn off the LED light           //
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
  Serial.print(F("- Initializing MCP7940 Real-Time-Clock\n"));                //                                  //
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
  Serial.println(F("- Setting TRIM back to 0"));                              //                                  //
  MCP7940.calibrate(0);                                                       // Reset trim                       //
  Serial.println("- Getting GPS fix with Date/Time");                         //                                  //
  now = readGPS() + TimeSpan(0,UTC_OFFSET,0,0);                               // Adjust for UTC                   //
  MCP7940.adjust(now);                                                        // Set the RTC time                 //

//  MCP7940.setAlarm(0,7,now+TimeSpan(0,0,AL0_INTERVAL_MINUTES,0));             // Set the next alarm               //
  MCP7940.setSQWSpeed(0);

  Serial.println(F("- Setting RTC alarm to every minute."));                  //                                  //
  Serial.print(F("- Setting MCP7940 using GPS time to "));                    //                                  //
  sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d",now.year(),now.month(), // Use sprintf() to pretty print    //
          now.day(), now.hour(), now.minute(), now.second());                 // date/time with leading zeros     //
  Serial.println(inputBuffer);                                                // Display the current date/time    //
  Serial.println("Rdgs  GPS Date / time    Delta    ");                       // Display the output header lines  //
  Serial.println("==== =================== ======== ");                       //                                  //
  attachInterrupt(digitalPinToInterrupt(MFP_PIN),RTCTick,FALLING);            // Trigger on change of MFP         //
  attachInterrupt(digitalPinToInterrupt(PPS_PIN),GPSTick,RISING);             // Trigger on change of MFP         //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static int32_t lastGPSMicros=0;                                               // Store last RTC Micros reading    //
  static int32_t lastRTCMicros=0;                                               // Store last RTC Micros reading    //
  static int32_t lastGPSDeltaMicros=0;                                             // Store delta between RTC and GPS  //
  static int32_t lastRTCDeltaMicros=0;                                             // Store delta between RTC and GPS  //
  static uint16_t iterations = 1;                                             // Main loop iterations             //
  static int32_t lastDelta = 0;
  static int16_t rollingMinute[60] = {0};
/*
  if (lastGPSMicros!=GPSMicros) {
    uint32_t deltaMicros = GPSMicros-lastGPSMicros;
    Serial.print("GPS ");
    Serial.print(deltaMicros);
    Serial.println();
    lastGPSDeltaMicros = deltaMicros;
    lastGPSMicros = GPSMicros;
  }
  if (lastRTCMicros!=RTCMicros) {
    uint32_t deltaMicros = RTCMicros-lastRTCMicros;
    Serial.print("RTC ");
    Serial.print(deltaMicros);
    Serial.println();
    lastRTCDeltaMicros = deltaMicros;
    lastRTCMicros = RTCMicros;
  }
*/
  if(lastRTCMicros!=RTCMicros) {
    int32_t delta = RTCMicros-GPSMicros;

      rollingMinute[iterations++%60]=lastDelta -  delta;
      Serial.print(lastDelta-delta);
      Serial.print(" ");
      int32_t temp=0;
      for (uint8_t i=0;i<60;i++) temp += rollingMinute[i];
      temp = temp / 60;
      Serial.println(temp);
      lastDelta = delta;

    lastRTCMicros = RTCMicros;
  } 

} // of method loop()                                                         //----------------------------------//

/*
  if (lastRTCMicros!=RTCMicros) {
    now = MCP7940.now();                                             // Get the current RTC time         //
    MCP7940.setAlarm(0,7,now+TimeSpan(0,0,AL0_INTERVAL_MINUTES,0));             // Set the next alarm               //
    int32_t deltaMicros = (int32_t)RTCMicros-(int32_t)GPSMicros;
    if (iterations==1) lastDeltaMicros = deltaMicros;                          // set initial value               //
    int32_t deltaLast   = deltaMicros-lastDeltaMicros;
    sprintf(inputBuffer,"%04d %04d-%02d-%02d %02d:%02d:%02d %8d",      // Use sprintf() to pretty print    //
    iterations++,now.year(),now.month(),now.day(),now.hour(),now.minute(),// output with leading zeroes       //
    now.second(), deltaLast );
    Serial.print(inputBuffer);                                                 // Output the sprintf buffer       //

    Serial.print(" ");
    Serial.print(MCP7940.getCalibrationTrim());
    
    Serial.println();
    lastDeltaMicros = deltaMicros;
    lastRTCMicros   = RTCMicros;
    if (iterations%10==0) MCP7940.calibrate(MCP7940.getCalibrationTrim()+1);
  }
*/
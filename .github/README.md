[![License: GPL v3](https://zanduino.github.io/Badges/GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Build](https://github.com/Zanduino/MCP7940/workflows/Build/badge.svg)](https://github.com/Zanduino/MCP7940/actions?query=workflow%3ABuild) [![Format](https://github.com/Zanduino/MCP7940/workflows/Format/badge.svg)](https://github.com/Zanduino/MCP7940/actions?query=workflow%3AFormat) [![Wiki](https://zanduino.github.io/Badges/Documentation-Badge.svg)](https://github.com/Zanduino/MCP7940/wiki) [![Doxygen](https://github.com/Zanduino/MCP7940/workflows/Doxygen/badge.svg)](https://Zanduino.github.io/MCP7940/html/index.html) [![arduino-library-badge](https://www.ardu-badge.com/badge/MCP7940.svg?)](https://www.ardu-badge.com/MCP7940)
# MCP7940 library<br>

<img src="https://github.com/Zanduino/MCP7940/blob/master/Images/MCP7940M-PDIP-8.png" width="175" align="right"/> *Arduino* library for accessing the Microchip MCP7940 real time clock. There are two versions of the RTC, MCP7940M and MCP7940N, which differ only in that the MCP7940N has a battery backup supply pin and keeps the clock running on power failure. The MCP7940N also has additional registers which store the timepoint of power failure and point in time when power was restored. In addition there are 3 more RTC chips, the MCP79400, MCP79401 and MCP79402 which are backwards compatible but include a special EEPROM storage for EUI address and these are also supported by this library

## Overview
The [MCP7940M](http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf), [MCP7940N](http://ww1.microchip.com/downloads/en/DeviceDoc/20005010F.pdf) and [MCP7940x](http://ww1.microchip.com/downloads/en/DeviceDoc/20005009F.pdf) datasheets describe the chip in detail. While there is no breakout board available for this RTC at the present time, it is not difficult to build into a project. The MCP7940 is available as a PDIP 8 pin which allows for easy use on a breadboard and the only additional parts necessary are a 32.768kHz crystal with 2 small capacitors (~6pF, depends upon circuit capacitance) and some pull-up resistors for the I2C data lines as well as for the multifunction pin, if that is to be used.

## Features
Apart from performing as a real-time clock, the MCP4790 has 64 Bytes of SRAM storage available and features 2 separate alarms that can be set to be recurring or a single use. The alarm state can be checked using library functions or can pull down the the MFP (multi-function pin) for a hardware interrupt or other action.
The MCP7940 allows for software trimming and the library has functions which support fine-tuning the RTC to increase accuracy.

## Description
<img src="https://github.com/Zanduino/MCP7940/blob/master/Images/MCP7940_bb.png" width="175px" align="left" /> A detailed library description along with further details are available at the GitHub [MCP7940 Wiki](https://github.com/Zanduino/MCP7940/wiki). This includes wiring information and sample sketches that illustrate the library and it's functionality.
</br></br></br></br></br></br>
  
## Examples
Several example sketches exist and are documented on the [Wiki pages](https://github.com/Zanduino/MCP7940/wiki)

| Example Program                                                                       | Description                                     |
| ------------------------------------------------------------------------------------- | ----------------------------------------------- |
| [SetAndCalibrate](https://github.com/Zanduino/MCP7940/wiki/SetAndCalibrate.ino)       | Set and Calibrate the RTC using various methods |
| [SetAlarms](https://github.com/Zanduinon/MCP7940/wiki/SetAlarms.ino)                  | Set Alarm 0 and Alarm 1                         |
| [SquareWave](https://github.com/Zanduino/MCP7940/wiki/SquareWave.ino)                 | Output a Square Wave                            |
| [TestBatteryBackup](https://github.com/Zanduino/MCP7940/wiki/TestBatteryBackup.ino)   | Show how the battery backup can be used         |
| [AccessMemory](https://github.com/Zanduino/MCP7940/wiki/AccessMemory.ino)             | Read and Write to the MCP7940 memory            |
| [RegressionTests](https://github.com/Zanduino/MCP7940/wiki/RegressionTests.ino)     |   Run regression test after changing library code |

## Documentation
The library and example programs have Doxygen documentation, whose output can be found at [Doxygen Documentation](https://Zanduino.github.io/MCP7940/html/index.html)  

## Supported Hardware
The following configurations have been compiled and/or tested. [Travis-CI](https://travis-ci.org) is used for automated continuous integration and testing.

<table>
  <th>Name</th>
  <th>Tested</th>
  <th>CPU</th>
  <th>Comments</th>
  <tr>
    <td>Arduino UNO</td>
    <td>Yes</td>
    <td>ATMega328P</td>
    <td></td>
  <tr>
  <tr>
    <td>Arduino Mega</td>
    <td>Yes</td>
    <td>ATMega 2560</td>
    <td></td>
  <tr>
  <tr>
    <td>Arduino Micro</td>
    <td>Yes</td>
    <td>ATMega32U4</td>
    <td></td>
  <tr>
  <tr>
    <td>Arduino 1280</td>
    <td>Yes</td>
    <td>ATMega1280</td>
    <td></td>
  <tr>
  <tr>
    <td>Arduino 328</td>
    <td>Yes</td>
    <td>ATMega328</td>
    <td></td>
  <tr>
  <tr>
    <td>Arduino 168</td>
    <td>Yes</td>
    <td>ATMega168</td>
    <td></td>
  <tr>
</table>    
    
[![Zanshin Logo](https://zanduino.github.io/Images/zanshinkanjitiny.gif) <img src="https://zanduino.github.io/Images/zanshintext.gif" width="75"/>](https://zanduino.github.io)


# MCP7940 library [![Build Status](https://travis-ci.org/SV-Zanshin/MCP7940.svg?branch=master)](https://travis-ci.org/SV-Zanshin/MCP7940) [![DOI](https://www.zenodo.org/badge/98077572.svg)](https://www.zenodo.org/badge/latestdoi/98077572)
<img src="https://github.com/SV-Zanshin/MCP7940/blob/master/Images/MCP7940M-PDIP-8.png" width="175" align="right"/> *Arduino* library for accessing the Microchip MCP7940 real time clock. There are two versions of the RTC, MCP7940M and MCP7940N, which differ only in that the MCP7940N has a battery backup supply pin and keeps the clock running on power failure. The MCP7940N also has additional registers which store the timepoint of power failure and point in time when power was restored.

## Overview
The [MCP7940M](http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf) and [MCP7940N](http://ww1.microchip.com/downloads/en/DeviceDoc/20005010F.pdf) datasheets describe the chip in detail. While there is no breakout board available for this RTC at the present time, it is not difficult to build into a project. The MCP7940 is available as a PDIP 8 pin which allows for easy use on a breadboard and the only additional parts necessary are a 32.768kHz crystal with 2 small capacitors (~6pF, depends upon circuit capacitance) and some pull-up resistors for the I2C data lines as well as for the multifunction pin, if that is to be used.

## Features
Apart from performing as a real-time clock, the MCP4790 has 64 Bytes of SRAM storage available and features 2 separate alarms that can be set to be recurring or a single use. The alarm state can be checked using library functions or can pull down the the MFP (multi-function pin) for a hardware interrupt or other action.
The MCP7940 allows for software trimming and the library has functions which support fine-tuning the RTC to increase accuracy.

## Description
<img src="https://github.com/SV-Zanshin/MCP7940/blob/master/Images/MCP7940_bb.png" width="175px" align="left" /> A detailed library description along with further details are available at the GitHub [MCP7940 Wiki](https://github.com/SV-Zanshin/MCP7940/wiki). This includes wiring information and sample sketches that illustrate the library and it's functionality.
</br></br></br></br></br></br>
  
## Examples
Several example sketches exist and are documented on the [Wiki pages][https://github.com/SV-Zanshin/MCP7940/wiki]
<Table>
  <th>Example Program</th>
  <th>Description</th>
  <tr>
    <td> [SetAndCalibrate](https://github.com/SV-Zanshin/MCP7940/wiki/SetAndCalibrate.ino)</td>
    <td></td>
  </tr>
  <tr>
    <td>[SetAlarms](https://github.com/SV-Zanshin/MCP7940/wiki/SetAlarms.ino)</td>
    <td></td>
  </tr>
  <tr>
    <td>[SquareWave](https://github.com/SV-Zanshin/MCP7940/wiki/SquareWave.ino)</td>
    <td></td>
  </tr>
  <tr>
    <td>[TestBatteryBackup](https://github.com/SV-Zanshin/MCP7940/wiki/TestBatteryBackup.ino)</td>
    <td></td>
  </tr>
  <tr>
    <td>[AccessMemory](https://github.com/SV-Zanshin/MCP7940/wiki/AccessMemory.ino)</td>
    <td></td>
  </tr>
</Table>

## Documentation
The library and example programs have Doxygen documentation, whose output can be found at [Doxygen Documentation](https://sv-zanshin.github.io/MCP7940/html/index.html)  

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
    
![Zanshin Logo](https://www.sv-zanshin.com/r/images/site/gif/zanshinkanjitiny.gif) <img src="https://www.sv-zanshin.com/r/images/site/gif/zanshintext.gif" width="75"/>

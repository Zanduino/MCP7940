# MCP7940 library
<img src="https://github.com/SV-Zanshin/MCP7940/blob/master/Images/MCP7940M-PDIP-8.png" width="175" align="right"/> *Arduino* library for accessing the Microchip MCP7940 real time clock.

## Overview
The [MCP7940 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf) describes the chip in detail. While there is no breakout board available for this chip at the present time, it is not difficult to build into a project. The MCP7940 is available as a PDIP 8 pin which allows for easy use on a breadboard and the only additional parts necessary are a 32.768kHz crystal with 2 small capacitors (~6pF, depends upon circuit capacitance) and some pull-up resistors for the data lines and for the multifunction pin, if that is to be used.

## Features
Apart from performing as a real-time clock, the MCP4790 has 64 Bytes of SRAM available for use and features 2 separate alarms that can be set to be recurring or a single use. The alarm state can either be checked using the library or can pull down the the MFP (multi-function pin) for a hard interrupt or other hardware action.
The MCP7940 allows for software trimming and the library has functions which support fine-tuning the RTC.

## Description
A detailed library description and further details are available at the GitHub [MCP7940 Wiki](https://github.com/SV-Zanshin/MCP7940/wiki)

![Zanshin Logo](https://www.sv-zanshin.com/r/images/site/gif/zanshinkanjitiny.gif) <img src="https://www.sv-zanshin.com/r/images/site/gif/zanshintext.gif" width="75"/>

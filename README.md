# MCP7940 library
<img src="https://github.com/SV-Zanshin/MCP7940/blob/master/Images/MCP7940M-PDIP-8.png" width="175" align="right"/> *Arduino* library for accessing the Microchip MCP7940 real time clock. There are two versions of the RTC, MCP7940M and MCP7940N, which differ only in that the MCP7940N has a battery backup supply pin and keeps the clock running on power failure. The MCP7940N also has additional registers which store the timepoint of power failure and point in time when power was restored.

## Overview
The [MCP7940M](http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf) and [MCP7940N](http://ww1.microchip.com/downloads/en/DeviceDoc/20005010F.pdf) datasheets describe the chip in detail. While there is no breakout board available for this RTC at the present time, it is not difficult to build into a project. The MCP7940 is available as a PDIP 8 pin which allows for easy use on a breadboard and the only additional parts necessary are a 32.768kHz crystal with 2 small capacitors (~6pF, depends upon circuit capacitance) and some pull-up resistors for the I2C data lines as well as for the multifunction pin, if that is to be used.

## Features
Apart from performing as a real-time clock, the MCP4790 has 64 Bytes of SRAM storage available and features 2 separate alarms that can be set to be recurring or a single use. The alarm state can be checked using library functions or can pull down the the MFP (multi-function pin) for a hardware interrupt or other action.
The MCP7940 allows for software trimming and the library has functions which support fine-tuning the RTC to increase accuracy.

## Description
<img src="https://github.com/SV-Zanshin/MCP7940/blob/master/Images/MCP7940_bb.png" width="175px" align="left" /> The detailed library description along with further details are available at the GitHub [MCP7940 Wiki](https://github.com/SV-Zanshin/MCP7940/wiki). This includes wiring information and sample sketches that illustrate the library and it's functionality.

![Zanshin Logo](https://www.sv-zanshin.com/r/images/site/gif/zanshinkanjitiny.gif) <img src="https://www.sv-zanshin.com/r/images/site/gif/zanshintext.gif" width="75"/>

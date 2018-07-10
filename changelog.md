# Changelog for MCP7940 library and example programs  
File contains a list of changes made to artefacts in the MCP7940 Arduino library and example programs, most recent changes are listed first.

<table>
  <th>Date</th>
  <th>Who</th>
  <th>Version</th>
  <th>Changes</th>
  <tr>
  </tr>
    <td>2018-07-10</td>
    <td>@SV-Zanshin</td>
    <td>1.0.0</td>
    <td>Reformatted this file</td>
  </tr>
    </tr>
    <td>2018-07-10</td>
    <td>@SV-Zanshin</td>
    <td>1.0.0</td>
    <td>

in begin(): it makes more sense to set the I2C speed first, then start a transmission.
in MCP7940.h: cleaned up formatting of the readRAM() and writeRAM() templates, and added some extra explanation here.
Updated, tested/corrected setAlarm().
Updated, tested setAlarmPolarity().
Updated, tested weekdayRead() & weekdayWrite(const uint8_t dow).
Fixed bug in dayOfTheWeek() - it was a day off due to incorrect calculation.
Updated getMFP() to allow for the other options. Returns value 0-3:
    0 = pin set LOW.
    1 = pin set HIGH.
    2 = pin controlled by alarms.
    3 = pin controlled by square wave output.
Updated, tested setMFP().
Updated, tested setSQWSpeed(). Added support for 64 Hz square wave output.
Updated, tested getPowerFail() & clearPowerFail().













</td>
  </tr>
</table>






## [1.0.0] - 2017-06-20  
### Added  


# Description

:editormd-logo-5x: **2020-09-1 10:36:03**
# &copy; vic
------------


## 1.ROM memory division

  |Address                                  |Size              |Introduction|
  |-----------------------------------------|------------------|------------|
  |0x0800 0000 ~ 0x0800 1FFF                |8k                |BootLeader  |
  |0x0800 2000 ~ 0x0801 DFFF                |112k              |Application |
  |0x0801 E000 ~ 0x0801 FFFF                |8k                |Parameter   |

## 2.Upgrade function files introduction

  long range upgrade：** User/Upgrade/LongRange/ **
  short range upgrade：** User/Upgrade/ShortRange/ **
  serial port upgrade：** User/Upgrade/SerialPort/ **

## 3.Upgrade data of RF transmission format

  |Upgrade data package send 138 bytes each time, including |         |
  |---------------|---------------------------------------------------|
  |data[0]        |    hardware version                               |
  |data[1]        |    software version                               |
  |data[2:3]      |    total pages number                             |
  |data[4:5]      |    current page number                            |
  |data[6:133]    |    program data                                   |
  |data[134:137]  |    crc32 check code(from data[0] to data[133])    |

## 4.External flash data format

  |Address                                  |Size              |Introduction|
  |-----------------------------------------|------------------|------------|
  |0x0000 0000 ~ 0x0000 03FF                |1k                |Parameter   |
  |0x0000 0400 ~ 0x0007 FFFF                |511k              |Uprade data |


## Program flow chart


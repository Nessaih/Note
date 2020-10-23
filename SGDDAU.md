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

[========]
### Boot leader 流程图
```flow
st=>start: 开始
op1=>operation: MCU初始化
op2=>operation: 外设初始化
op3=>operation: 读取外部FLASH升级参数：
------------------------
升级标志：upgrade_flag；
厂商代码：Manufacturer；
硬件版本：hversion；
软件版本：sversion；
升级包总页数：page_num；
升级文件CRC32校验：file_crc；
------------------------
cond1=>condition: 检查upgrade_flag、Manufacturer是否需要升级?
cond2=>condition: 检查文件CRC校验是否正确?
op4=>operation: 将外部FLASH数据写入ROM中对应位置
op5=>operation: 跳转至APP开始执行程序
e=>end: 结束
st->op1->op2->op3->cond1->cond2->op4->op5->e
cond1(yes)->cond2
cond1(no)->e
cond2(yes)->op4
cond2(no)->e
```
[========]



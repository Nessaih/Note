## 一 、调用库函数，初始化变量未全赋值，导致SPI配置出错

#### 测试用例：

模块地址设置为：`660000011209`

1. **单抄报文1**：`55 3A 37 36 41 CD AB 0A 61 09 12 01 00 00 66 04 02 06 05 08 04 3C 09 12 01 00 00 66 04 02 06 05 08 04 C1 02 36 00 FE FE FE FE 68 09 12 01 00 00 66 68 11 04 33 33 33 33 33 16 CC 6A 99 02 17`

2. **单抄报文2**：`55 3A 37 36 41 CD AB 0A 61 09 12 01 00 00 66 04 02 06 05 08 04 3C 09 12 01 00 00 66 04 02 06 05 08 04 C1 02 35 00 FE FE FE FE 68 09 12 01 00 00 66 68 11 04 33 33 33 33 33 16 CC 6A 99 02 17`

3. **点名帧**：`55 26 00 20 43 CD 01 FF FF 09 12 01 00 00 66 AA AA AA AA AA AA 01 02 09 12 01 00 00 66 AA AA AA AA AA AA 00 00 00 04 DB 44 0F 17`



#### 测试现象：
1. 现象模块上电后，等关联地址后，点名能通，一直点名超过800次，状态正常。
2. 进行抄表，模块收到射频数据正确，抄表数据正确，抄表接收应答数据正确，射频发送应答异常(在发送但是数据不对)。
3. 进行抄表，模块收到射频数据正确，抄表数据正确，抄表接收应答数据正确，模块复位。

#### 射频发送异常原因总结：


1. SPI按照如下代码配置
```c++
SPI_InitTypeDef  SPI_InitStructure; 

SPI_Cmd(SPI1, DISABLE);
SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;   //单线
SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;                //从机模式
SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;           //八位数据
SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                  // mode0 SPI_CPOL_Low, SPI_CPHA_1Edge;
SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                // mode3 SPI_CPOL_High,SPI_CPHA_2Edge
SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                   
//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//从机不必设置此值
SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
SPI_Init(SPI1, &SPI_InitStructure);
SPI_Cmd(SPI1, ENABLE);
```

问题在于被注释掉的一行`//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//从机不必设置此值`;按照STM官方手册，作为从机确实是不用设置此值。但如果按照这么配置，`SPI1->CR1`的值应该为0xC2C0。会发现`SPI1->CR1`的值和我们预计的不同，导致出错。为什么设置的值和预计的不一致，可以看一下`SPI_Init`的具体实现，代码如下：

```c++

/**

  * @brief  Initializes the SPIx peripheral according to the specified 
  *         parameters in the SPI_InitStruct.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *         contains the configuration information for the specified SPI peripheral.
  * @retval None
  
  */
 
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
  uint16_t tmpreg = 0;
  
  /* check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));   
  
  /* Check the SPI parameters */
  assert_param(IS_SPI_DIRECTION_MODE(SPI_InitStruct->SPI_Direction));
  assert_param(IS_SPI_MODE(SPI_InitStruct->SPI_Mode));
  assert_param(IS_SPI_DATASIZE(SPI_InitStruct->SPI_DataSize));
  assert_param(IS_SPI_CPOL(SPI_InitStruct->SPI_CPOL));
  assert_param(IS_SPI_CPHA(SPI_InitStruct->SPI_CPHA));
  assert_param(IS_SPI_NSS(SPI_InitStruct->SPI_NSS));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_InitStruct->SPI_BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(SPI_InitStruct->SPI_FirstBit));
  assert_param(IS_SPI_CRC_POLYNOMIAL(SPI_InitStruct->SPI_CRCPolynomial));

/*---------------------------- SPIx CR1 Configuration ------------------------*/
  /* Get the SPIx CR1 value */
  tmpreg = SPIx->CR1;
  /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
  tmpreg &= CR1_CLEAR_Mask;
  /* Configure SPIx: direction, NSS management, first transmitted bit, BaudRate prescaler
     master/salve mode, CPOL and CPHA */
  /* Set BIDImode, BIDIOE and RxONLY bits according to SPI_Direction value */
  /* Set SSM, SSI and MSTR bits according to SPI_Mode and SPI_NSS values */
  /* Set LSBFirst bit according to SPI_FirstBit value */
  /* Set BR bits according to SPI_BaudRatePrescaler value */
  /* Set CPOL bit according to SPI_CPOL value */
  /* Set CPHA bit according to SPI_CPHA value */
  tmpreg |= (uint16_t)((uint32_t)SPI_InitStruct->SPI_Direction | SPI_InitStruct->SPI_Mode |
                  SPI_InitStruct->SPI_DataSize | SPI_InitStruct->SPI_CPOL |  
                  SPI_InitStruct->SPI_CPHA | SPI_InitStruct->SPI_NSS |  
                  SPI_InitStruct->SPI_BaudRatePrescaler | SPI_InitStruct->SPI_FirstBit);
  /* Write to SPIx CR1 */
  SPIx->CR1 = tmpreg;
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPIx->I2SCFGR &= SPI_Mode_Select;		

/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Write to SPIx CRCPOLY */
  SPIx->CRCPR = SPI_InitStruct->SPI_CRCPolynomial;
}

```

`stm32f10x_conf.h`文件中有定义`assert_param`宏,实际生效的为 `#define assert_param(expr) ((void)0)`


```C++
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32F10x_CONF_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

```

因此assert_param检测不了参数出错（宏定义没开），我们自己定义`SPI_InitStructure`是一个局部变量，数值是随机的（准确来说是栈中对应地址未清除的数据）。

1. 当进行点名时，使用的栈空间较少，未使用的部分内存值是0，`SPI_InitStructure.SPI_BaudRatePrescaler`即使是上次未清除的数据，也还是0，配置SPI不会出错。
2. 当进行抄表时，使用栈空间较多，局部变量`SPI_InitStructure.SPI_BaudRatePrescaler`的值很可能已经不是0（其对应的二进制也超过了BR[2:0]的位宽）,直接将结构体`SPI_InitStructure`的各变量直接相或时，会修改掉其他bit位。

    （1）修改了CPOL、CPHA（SPI的模式），数据虽然能传输到射频芯片，但接收数据错误，因此射频发送数据出错。

    （2）修改了MSTR位，导致MSTR = 1（SPI主机模式），而NSS管脚由软件输出低。此时产生**主模式错误**,此错误导致SPE 位被复位。这将停止一切输出，并且关闭 SPI 接口。SPI关闭后，DMA的CNDTR寄存器无法减为0，阻塞查询此寄存器会导致看门狗复位。



## 二、DMA数据传输完成，不代表外设数据传输完成

```C++
void spi_slave_send(INT8U *buff,INT16U len)
{
    SPI_I2S_DeInit(SPI1);
    DMA_DeInit(DMA1_Channel3);
    //RCC-----------------------------------------------------------------------------------------
    RCC->AHBENR |= 0x01;                         //DMA 使能
    RCC->APB2ENR |= 0x1005;                      //SPI1、IOPA、AFIO 使能
    //IO：SCK(PA5)、MISO(PA6)、MOSI(PA7)---------------------------------------------------------
    GPIOA->CRL &= 0x000FFFFF;                   //清除PA5、PA6、PA7 IO配置
    GPIOA->CRL |= 0x4B400000;                   //PA5浮空输入、PA6复用推挽输出、PA7浮空输入
    //SPI-----------------------------------------------------------------------------------------
    SPI1->CR1 = 0xC2C0;                         //单线发送、八位数据、SPI使能、从设备、模式0
    SPI1->CR2 = 0;                              //DMA使能关闭
    //DMA----------------------------------------------------------------------------------------
    DMA1_Channel2->CCR = 0;                    //关闭DMA1通道2（MOSI）
    DMA1_Channel3->CCR = 0;                    //关闭DMA1通道3（MISO）
    DMA1_Channel3->CPAR = SPI1_BASE+0x0C;      //外设地址SPI1-DR
    DMA1_Channel3->CMAR = (uint32_t)buff;      //内存地址
    DMA1_Channel3->CNDTR = len;                //传输长度
    DMA1_Channel3->CCR = 0x1093;               //MEM2MEM:       0    关闭存储器到存储器模式
                                               //PL[1:0]:      01    中优先级
                                               //MSIZE[1:0]:   00    存储器数据宽度8位
                                               //PSIZE[1:0]:   00    外设数据宽度八位
                                               //MINC:          0    存储器地址自增
                                               //PINC:          0    外设地址不自增
                                               //CIRC:          0    非循环模式
                                               //DIR:           0    从存储器读取（传输方向）
                                               //TEIE:          0    传输错误中断禁止
                                               //HTIE:          0    半传输中断禁止
                                               //TCIE:          0    传输完成中断禁止
                                               //EN:            0    DMA1通道3开启
    //SPI-----------------------------------------------------------------------------------------
    SPI1->CR2 |= 0x02;                         //SPI1的DMA传输使能
    //实际用HK32测试发现TC标志置位后CNDTR不为0，导致无法正常使用，所以改为查询CNTDR计数器
    //while(DMA1->ISR & 0x200);                //等待数据DMA数据传输完成
    //DMA1->IFCR &=  0xFFFFEFF;                //清除传输完成标志
    while(DMA1_Channel3->CNDTR);               //等待数据DMA数据传输完成
    SPI1->CR2 = 0;                             //SPI1的DMA传输使能关闭
    DMA1_Channel3->CCR = 0;                    //关闭DMA1通道3（MISO）
    while(!(SPI1->SR & 0x02));                 //等待最后一字节发送完毕▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲重点▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
    SPI1->CR1 &= 0xFFBF;                       //关闭SPI1
    GPIOA->CRL &= 0x000FFFFF;                  //清除PA5、PA6、PA7 IO配置
    GPIOA->CRL |= 0x44400000;                  //PA5浮空输入、PA6浮空输入、PA7浮空输入
}
```
如果去掉 `while(!(SPI1->SR & 0x02)); ` 后，会发现射频数据无法正确发送出（表现为侦听台接收不到）。原因是DAM数据传输完成是最后一字节写入到SPI的DR寄存器，数据没有从DR寄存器中传输出，此时执行关闭SPI，会导致最后一字节无法发送出。而射频的最后一字节为结束字节，侦听台收不到结束字节，就会丢弃此帧。

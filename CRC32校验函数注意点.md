```C++

// CRC32.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>


/*函数：crc32
 *描述：取自“http://mdfs.net/Info/Comp/Comms/CRC32.htm”
 *运算结果：错误
 */
#define poly 0xEDB88320ul
/* Some compilers need
   #define poly 0xEDB88320uL
 */
 /* On entry, addr=>start of data
              num = length of data
              crc = incoming CRC     */
int crc32(char *addr, int num, int crc)
{
    int i;

    for (; num > 0; num--)              /* Step through bytes in memory */
    {
        crc = crc ^ *addr++;            /* Fetch byte from memory, XOR into CRC */
        for (i = 0; i < 8; i++)             /* Prepare to rotate 8 bits */
        {
            if (crc & 1)                  /* b0 is set... */
                crc = (crc >> 1) ^ poly;    /* rotate and XOR with ZIP polynomic */
            else                          /* b0 is clear... */
                crc >>= 1;                  /* just rotate */
            /* Some compilers need:
              crc &= 0xFFFFFFFF;
             */
        }                             /* Loop for 8 bits */
    }                               /* Loop until num=0 */
    return(crc);                    /* Return updated CRC */
}



/*函数：crc32_le
 *描述：linux内核CRC32校验函数
 *运算结果：正确
 */
#define CRCPOLY_LE 0xedb88320ul
uint32_t crc32_le(uint32_t crc, unsigned char const *p, size_t len)
{
    int i;
    while (len--) {
        crc ^= *p++;
        for (i = 0; i < 8; i++)
            crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
    }
    crc ^= 0xFFFFFFFF;
    return crc;
}




int main()
{
    char data[100] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A };
    uint32_t crc;
    /*
    for (int i = 0; i < 4; i++) {
        crc = invert_8((poly>>i*8)&0xFF);
        std::cout << std::hex << crc;
    }*/
    crc = crc32_le(0xFFFFFFFF,(unsigned char const *)data, 10);
    std::cout << std:: hex << crc<<"\n";
    crc = crc32((char *)data, 10, 0xFFFFFFFF);
    std::cout << std::hex << crc << "\n";
}

```

**Visual Studio 2017运行结果：**
```
dadfa884
f83e449a
E:\VisualStudioWork\test\CRC32\Debug\CRC32.exe (进程 21780)已退出，返回代码为: 0。
若要在调试停止时自动关闭控制台，请启用“工具”->“选项”->“调试”->“调试停止时自动关闭控制台”。
按任意键关闭此窗口...
```


两个程序算法完全相同，运行结果居然不相同，经过多种在线验证，`crc32_le`计算结果异或**0xFFFF FFFF**后正确，而`crc32`计算结果异或**0xFFFF FFFF**后错误。
经过调试，发现原因：若函数入口参数`crc`定义为有符号型，`crc`左移运算时高位会补1，这是错误的。如果将`crc`定义为无符号型则两个函数计算结果相同。至此，问题已确定，并解决。



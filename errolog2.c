/****************************************************************************************************************************************** 
* 函数名称: Timer0_Init()
* 功能说明: TIMR0初始化，用作捕获脉冲宽度
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void Timer0_Init(void)
{
    PORT_Init(PORTA, PIN4, FUNMUX_PULSE_IN, 1);     //PA4配置为捕获输入引脚
    TIMR_Init(TIMR0, TIMR_MODE_TIMER, 0xFFFFFFFF, 0);//Timer0初始化：计数模式
    TIMRG->PCTRL &= ~(0X01 << 1);                   //测量脉冲极性： 从下降沿到上升沿长度
    TIMRG->IE |= (0X01 << 16);                      //脉宽测量中断使能
    IRQ_Connect(IRQ0_15_PULSE, IRQ7_IRQ,1);         //定时器0中断链接到IRQ6中断线，低优先级
    TIMRG->IF |= 0X01<<16;                          //清除中断标志
    TIMRG->PCTRL |=  (0X01 << 0);                   //脉宽测量使能
}

uint32_t plus_width = 0;
void IRQ7_Handler(void)
{
    if(TIMRG->IF & 0X01<<16)
    {
        plus_width = TIMRG->PCVAL;
        TIMRG->PCTRL |=  (0X01 << 0);               //脉宽测量使能    
        TIMRG->IF |= 0X01<<16;                      //清除中断标志
    }
}
void Plus_Width_Clr(void)
{
    plus_width = 0;
}
uint32_t Plus_Width_Get(void)
{
    uint32_t temp = plus_width;
    //plus_width = 0;
    return temp;
}
uint32_t Plus_Time_Get(void)
{
    uint32_t plus_time_us = 0;
    
    plus_time_us = plus_width/24;
    //plus_width = 0;//第②句
    /**不能在此处清0,（本条语句记为第②句，上一条记为第①句），当在第②句还
      *没开始执行，捕获事件发生,则会进入捕获中断，知道捕获完成，plus_width被
      *赋值退出中断，然后执行第②句，则立刻plus_width又被清零，当下一次进入此
      *函数时plus_width值为0，则plus_time_us=0，不能返回正确的值（上个函数相同）
      */
    return plus_time_us;
}


/********************************************************************以下为主函数**********************************************************************/
#include "main.h"

int main(void)
{
	uint32_t key_press_time;
  SystemInit();
	Timer0_Init();
	Led_Init();
	Uart_Init(57600);
	printf("hello world\n");
	while(1)
	{
		if(0 != (key_press_time = Plus_Time_Get()))
		{
			printf("key press time:%lf s\n",(double)key_press_time/1000000);
			Plus_Width_Clr();//修改后将清零plus_width放在串口打印之后
		}
	}
}


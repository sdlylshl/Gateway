/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：GPIOC8接超声波模块的TRIG，GPIOC9接超声模块的ECHO，
             请将串口线(直连串口线)插在板子COM1口上,并打开超级
			 终端或串口助手，配置波特率115200，8位，一个停止位，
			 无校验位。定时向PC发送测得的距离         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
*********************************************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "TIM2.h"
#include "UltrasonicWave.h"

/*
 * 函数名：DelayTime_ms
 * 描述  ：Time   	延时的时间 MS
 * 输入  ：无
 * 输出  ：无
 */
void DelayTime_ms(int Time)      //1us延时函数
{
   unsigned int i;
   for ( ; Time>0; Time--)
     for ( i = 0; i <720; i++ );
}

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{	
    /* 配置系统时钟为 72M */  
	SystemInit();
	/* USART1 config 115200 8-N-1 */
	USART1_Config();
	NVIC_Configuration();
	TIM2_Configuration();
	UltrasonicWave_Configuration();

	for(;;)
	{
	  UltrasonicWave_StartMeasure();
	  DelayTime_ms(10000);	
	}
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

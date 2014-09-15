/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：crc.c
 * 描述    ：crc（循环冗余校验）应用函数库         
 * 实验平台：野火STM32开发板
 * 硬件连接：--------------------------
 *          |                          |
 *          | 利用的是CPU内部的CRC硬件 |
 *          |                          |
 *           --------------------------
 * 库版本  ：ST3.5.0
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "crc.h"

__IO uint32_t CRCValue = 0;		 // 用于存放产生的CRC校验值


int CRC_test(void)
{
	uint8_t i = 0;
		
	/* USART1 config 115200 8-N-1 */
	USART1_Config();
	
	/* 使能CRC时钟 */
	CRC_Config();
	
	printf("\r\n 这是一个 CRC(循环冗余校验)实验 \r\n");
	
	/* Compute the CRC of "DataBuffer" */
	for(i=0; i<BUFFER_SIZE; i++ ) 
	{
		CRCValue = CRC_CalcBlockCRC((uint32_t *)DataBuffer, BUFFER_SIZE);
		printf("\r\n32-bit CRC 校验码为:0X%X\r\n", CRCValue);
	}
	printf("\r\nCRC(循环冗余校验)测试成功\r\n");

	for(;;)
	{
		
	}
}

/*
 * 函数名：CRC_Config
 * 描述  ：使能CRC时钟
 * 输入  ：无
 * 输出  ：无
 * 调用  : 外部调用
 */
void CRC_Config(void)
{
	/* Enable CRC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

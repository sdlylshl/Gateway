/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：测试温湿度传感DHT11。         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "SysTick.h"
#include "DHT11.h"
#include "usart1.h"


DHT11_Data_TypeDef DHT11_Data;


/* 
 * 函数名：main
 * 描述  : 主函数
 * 输入  ：无
 * 输出  : 无
 */
int main(void)
{


	 /*初始化SysTick定时器*/
	 SysTick_Init();

	 /*初始化串口1*/
	 USART1_Config();

	 /*初始化DTT11的引脚*/
	 DHT11_GPIO_Config();  	

	 printf("\r\n***野火DHT11温湿度传感器实验***\r\n");

	 /*调用Read_DHT11读取温湿度，若成功则输出该信息*/
	if( Read_DHT11(&DHT11_Data)==SUCCESS)										 //“\”表示转向一下行
		printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",\
		DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
	else
		printf("Read DHT11 ERROR!\r\n");

  	  while(1);
	 
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/



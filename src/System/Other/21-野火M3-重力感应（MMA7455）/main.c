/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：I2C EEPROM(AT24C02)测试，测试信息通过USART1打印在电脑的超级终端。
 *          
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.0.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/	
#include "stm32f10x.h"
#include "I2C_MMA.h"
#include "usart1.h"

/*********************************/
   MMA_Dat X_Value={MMA_XOUT8_Addr,'X'};
   MMA_Dat Y_Value={MMA_YOUT8_Addr,'Y'};
   MMA_Dat Z_Value={MMA_ZOUT8_Addr,'Z'};

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 */
int main(void)
{	
    /* 配置系统时钟为 72M */ 
 // 	SystemInit();

	/* 串口1初始化 */
	USART1_Config();	
	 
	/*重力传感器初始化*/
	I2C_MMA_Init() ;
	
	/*重力传感器校准*/
	I2C_MMA_Cal();
	  
	printf("\r\n--------这是一个重力传感器测试程序---------\r\n");  	


		 /* 检测倾角*/
  		I2C_MMA_Test(&X_Value);
	   	I2C_MMA_Test(&Y_Value);
	    I2C_MMA_Test(&Z_Value);

			printf("\r\n--------X方向的数据----------\r\n");
		I2C_MMA_Printf(&X_Value);

			printf("\r\n--------Y方向的数据----------\r\n");
		I2C_MMA_Printf(&Y_Value);

			printf("\r\n--------Z方向的数据----------\r\n");
		I2C_MMA_Printf(&Z_Value);  		

	   	 /*进入省电模式*/  
	if(I2C_MMA_Standby()== SUCCESS )
	
		printf("\r\n Acceleration enter standby mode! \r\n");
	else
		printf("\r\n Acceleration standby mode ERROR! \r\n");
 
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

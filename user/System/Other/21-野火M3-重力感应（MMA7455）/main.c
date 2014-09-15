/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ��I2C EEPROM(AT24C02)���ԣ�������Ϣͨ��USART1��ӡ�ڵ��Եĳ����նˡ�
 *          
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.0.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/	
#include "stm32f10x.h"
#include "I2C_MMA.h"
#include "usart1.h"

/*********************************/
   MMA_Dat X_Value={MMA_XOUT8_Addr,'X'};
   MMA_Dat Y_Value={MMA_YOUT8_Addr,'Y'};
   MMA_Dat Z_Value={MMA_ZOUT8_Addr,'Z'};

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 * ����  ����
 */
int main(void)
{	
    /* ����ϵͳʱ��Ϊ 72M */ 
 // 	SystemInit();

	/* ����1��ʼ�� */
	USART1_Config();	
	 
	/*������������ʼ��*/
	I2C_MMA_Init() ;
	
	/*����������У׼*/
	I2C_MMA_Cal();
	  
	printf("\r\n--------����һ���������������Գ���---------\r\n");  	


		 /* ������*/
  		I2C_MMA_Test(&X_Value);
	   	I2C_MMA_Test(&Y_Value);
	    I2C_MMA_Test(&Z_Value);

			printf("\r\n--------X���������----------\r\n");
		I2C_MMA_Printf(&X_Value);

			printf("\r\n--------Y���������----------\r\n");
		I2C_MMA_Printf(&Y_Value);

			printf("\r\n--------Z���������----------\r\n");
		I2C_MMA_Printf(&Z_Value);  		

	   	 /*����ʡ��ģʽ*/  
	if(I2C_MMA_Standby()== SUCCESS )
	
		printf("\r\n Acceleration enter standby mode! \r\n");
	else
		printf("\r\n Acceleration standby mode ERROR! \r\n");
 
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

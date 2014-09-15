/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ��������ʪ�ȴ���DHT11��         
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "SysTick.h"
#include "DHT11.h"
#include "usart1.h"


DHT11_Data_TypeDef DHT11_Data;


/* 
 * ��������main
 * ����  : ������
 * ����  ����
 * ���  : ��
 */
int main(void)
{


	 /*��ʼ��SysTick��ʱ��*/
	 SysTick_Init();

	 /*��ʼ������1*/
	 USART1_Config();

	 /*��ʼ��DTT11������*/
	 DHT11_GPIO_Config();  	

	 printf("\r\n***Ұ��DHT11��ʪ�ȴ�����ʵ��***\r\n");

	 /*����Read_DHT11��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
	if( Read_DHT11(&DHT11_Data)==SUCCESS)										 //��\����ʾת��һ����
		printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
		DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
	else
		printf("Read DHT11 ERROR!\r\n");

  	  while(1);
	 
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/



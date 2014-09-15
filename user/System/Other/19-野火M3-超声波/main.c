/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ��GPIOC8�ӳ�����ģ���TRIG��GPIOC9�ӳ���ģ���ECHO��
             �뽫������(ֱ��������)���ڰ���COM1����,���򿪳���
			 �ն˻򴮿����֣����ò�����115200��8λ��һ��ֹͣλ��
			 ��У��λ����ʱ��PC���Ͳ�õľ���         
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
*********************************************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "TIM2.h"
#include "UltrasonicWave.h"

/*
 * ��������DelayTime_ms
 * ����  ��Time   	��ʱ��ʱ�� MS
 * ����  ����
 * ���  ����
 */
void DelayTime_ms(int Time)      //1us��ʱ����
{
   unsigned int i;
   for ( ; Time>0; Time--)
     for ( i = 0; i <720; i++ );
}

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{	
    /* ����ϵͳʱ��Ϊ 72M */  
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

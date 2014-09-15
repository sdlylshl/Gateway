/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��crc.c
 * ����    ��crc��ѭ������У�飩Ӧ�ú�����         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�--------------------------
 *          |                          |
 *          | ���õ���CPU�ڲ���CRCӲ�� |
 *          |                          |
 *           --------------------------
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "crc.h"

__IO uint32_t CRCValue = 0;		 // ���ڴ�Ų�����CRCУ��ֵ


int CRC_test(void)
{
	uint8_t i = 0;
		
	/* USART1 config 115200 8-N-1 */
	USART1_Config();
	
	/* ʹ��CRCʱ�� */
	CRC_Config();
	
	printf("\r\n ����һ�� CRC(ѭ������У��)ʵ�� \r\n");
	
	/* Compute the CRC of "DataBuffer" */
	for(i=0; i<BUFFER_SIZE; i++ ) 
	{
		CRCValue = CRC_CalcBlockCRC((uint32_t *)DataBuffer, BUFFER_SIZE);
		printf("\r\n32-bit CRC У����Ϊ:0X%X\r\n", CRCValue);
	}
	printf("\r\nCRC(ѭ������У��)���Գɹ�\r\n");

	for(;;)
	{
		
	}
}

/*
 * ��������CRC_Config
 * ����  ��ʹ��CRCʱ��
 * ����  ����
 * ���  ����
 * ����  : �ⲿ����
 */
void CRC_Config(void)
{
	/* Enable CRC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

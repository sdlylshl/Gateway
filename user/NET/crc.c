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

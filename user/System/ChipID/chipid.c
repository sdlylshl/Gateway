/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��chipid.c
 * ����    ����ȡCPU��ID������ÿ��оƬ����Ψһ�� 96_bit unique ID         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�-----------------
 *          |                 |
 *          |       ��        |
 *          |                 |
 *           -----------------
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "chipid.h"

uint32_t ChipUniqueID[3];

/*
 * ��������Get_ChipID
 * ����  ����ȡоƬID
 * ����  ����
 * ���  ����
 */
void Get_ChipID(void)
{
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // ���ֽ�
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	ChipUniqueID[2] = *(__IO u32 *)(0X1FFFF7E8); // ���ֽ�
}
int ChipID_test(void)
{	
	/* USART1 config 115200 8-N-1 */
	USART1_Config();

	/* to get the chipid and put it in ChipUniqueID[3] */
	Get_ChipID();	
	
	/* printf the chipid */
	printf("\r\nоƬ��ΨһIDΪ: %X-%X-%X\r\n",
	        ChipUniqueID[0],ChipUniqueID[1],ChipUniqueID[2]);
	
	/* printf the flash memory amount */
	printf("\r\nоƬflash������Ϊ: %dK \r\n", *(__IO u16 *)(0X1FFFF7E0));

	for(;;)
	{
		
	}
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

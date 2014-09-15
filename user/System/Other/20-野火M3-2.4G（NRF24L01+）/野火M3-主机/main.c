/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ��NRF24L01����w�������ʵ��        
 * ʵ��ƽ̨��Ұ��STM32������	
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "SPI_NRF.h"
#include "usart1.h"

u8 status;	//�����жϽ���/����״̬
u8 txbuf[4]={0,1,2,3};	 //���ͻ���
u8 rxbuf[4];			 //���ջ���
int i=0;

int main(void)
{
  
   SPI_NRF_Init();
   
  /* ����1��ʼ�� */
	USART1_Config();

	printf("\r\n ����һ�� NRF24L01 ���ߴ���ʵ�� \r\n");
   	printf("\r\n �������ߴ��� ������ �ķ�����Ϣ\r\n");
	printf("\r\n   ���ڼ��NRF��MCU�Ƿ��������ӡ�����\r\n");

	 /*���NRFģ����MCU������*/
   	status = NRF_Check(); 

	/*�ж�����״̬*/  
   if(status == SUCCESS)	   
   		 printf("\r\n      NRF��MCU���ӳɹ���\r\n");  
   else	  
   	     printf("\r\n  NRF��MCU����ʧ�ܣ������¼����ߡ�\r\n");


 while(1)
		{
	   	printf("\r\n ������ ������Ӧ����ģʽ\r\n"); 
	 	NRF_TX_Mode();
		 	
			/*��ʼ��������*/	
		status = NRF_Tx_Dat(txbuf);	  
		  
		  /*�жϷ���״̬*/
		  switch(status)
		  	{
		  	  case MAX_RT:
			 	 	printf("\r\n ������ û���յ�Ӧ���źţ����ʹ��������޶�ֵ������ʧ�ܡ� \r\n");
				 break;
	
			  case ERROR:
			  	 	printf("\r\n δ֪ԭ���·���ʧ�ܡ� \r\n");
				 break;
	
			  case TX_DS:
			  		printf("\r\n ������ ���յ� �ӻ��� ��Ӧ���źţ����ͳɹ��� \r\n");	 		
				 break;  								
		  	}			   	
	
	 	printf("\r\n ������ �������ģʽ�� \r\n");	
		NRF_RX_Mode();
	
			/*�ȴ���������*/
		status = NRF_Rx_Dat(rxbuf);
	
			/*�жϽ���״̬*/
			switch(status)
			{
			 case RX_DR:
			 	for(i=0;i<4;i++)
				{					
					printf("\r\n ������ ���յ� �ӻ��� ���͵�����Ϊ��%d \r\n",rxbuf[i]);
					txbuf[i] =rxbuf[i];
				}
				  break;
	
			 case ERROR:
				  	printf("\r\n ������ ���ճ���   \r\n");
				  break;  		
			}
		} 

}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


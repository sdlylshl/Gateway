/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��i2c_ee.c
 * ����    ��i2c EEPROM(AT24C02)Ӧ�ú�����         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�-----------------
 *          |                 |
 *          |  PB6-I2C1_SCL		|
 *          |  PB7-I2C1_SDA   |
 *          |                 |
 *           -----------------
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "I2C_MMA.h"
#include "usart1.h"
#include "math.h"

#define I2C_Speed			 100000
#define MMA_ADRESS	 		 0x3A  /*MMA_Device Address*/
#define ACC_Gravity			 9.8   /*�������ٶȵ�ֵ*/

/*
 * ��������I2C_GPIO_Config
 * ����  ��I2C1 I/O����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
		/* ʹ���� I2C1 �йص�ʱ�� */
	RCC_APB2PeriphClockCmd  (RCC_APB2Periph_GPIOB, ENABLE ); 
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  

	 /* PB6-I2C1_SCL��PB7-I2C1_SDA*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


/*
 * ��������I2C_Mode_Config
 * ����  ��I2C ����ģʽ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void I2C_Mode_Config(void)
{
	 /* Initialize the I2C1 according to the I2C_InitStructure members */ 
	I2C_InitTypeDef I2C_InitStructure; 
	 
	  /* I2C ���� */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ; 
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; 
	I2C_InitStructure.I2C_OwnAddress1 = MMA_ADRESS; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed; 
	
	/* ʹ�� I2C1 */
	I2C_Cmd  (I2C1,ENABLE); 

	/* I2C1 ��ʼ�� */
	I2C_Init(I2C1, &I2C_InitStructure);	   

	/*����Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);   
}

/*
 * ��������I2C_MMA_Init
 * ����  ��I2C ����(MMA7455)��ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void I2C_MMA_Init(void)
{	   
 	I2C_GPIO_Config();
	I2C_Mode_Config();
}  

/*
 * ��������I2C_MMA_ByteWrite
 * ����  ��дһ���ֽڵ�I2C MMA�Ĵ�����
 * ����  ��-pBuffer ������ָ��
 *         -WriteAddr �������ݵ�MMA�Ĵ����ĵ�ַ 
 * ���  ����
 * ����  ����
 * ����  ���ڲ�����
 */	
static void I2C_MMA_ByteWrite(u8 pBuffer, u8 WriteAddr)
{
  /*wait until I2C bus is not busy*/
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
    
	/* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
    /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 
  
   /* Send MMA address for write */
  I2C_Send7bitAddress(I2C1, MMA_ADRESS, I2C_Direction_Transmitter);
    
	/* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
   
    /* Send the MMA's Register address to write to */    
  I2C_SendData(I2C1, WriteAddr); 
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   
    /* Send the byte to be written */
  I2C_SendData(I2C1, pBuffer);
  
   /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
   
   /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);

}


/*
 * ��������I2C_MMA_ByteRead
 * ����  ����MMA�Ĵ��������ȡһ�����ݡ� 
 * ����  �� -ReadAddr �������ݵ�MMA�Ĵ����ĵ�ַ��        
 * ���  ����
 * ����  ����ȡ���ļĴ�������
 * ����  ���ڲ�����
 */	
static u8 I2C_MMA_ByteRead(u8 ReadAddr)
{
	  	u8 RxData;
	/*wait until I2C bus is not busy*/
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	 /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
 
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MMA address for write */
  I2C_Send7bitAddress(I2C1, MMA_ADRESS, I2C_Direction_Transmitter);
   
    /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

   /* Send the MMA's Register address to write to */
  I2C_SendData(I2C1, ReadAddr);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send MMA address for read */
  I2C_Send7bitAddress(I2C1, MMA_ADRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
   /* Test on wait for EV7 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	
	/* Read a byte from the MMA Register */
	RxData = I2C_ReceiveData(I2C1);

	/* Disable Acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);

	/* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

	/*Retrun Data*/
	return RxData;
}

/*
 * ��������I2C_MMA_Cal
 * ����  ��MMA7455 У׼
 * ����  ����
 * ���  ����
 * ����  ���ⲿ���ã��ڳ�ʼ�������
 */
void I2C_MMA_Cal(void)
{   
	I2C_MMA_ByteWrite(0x00,MMA_XOFFL_Addr);	/*У��xֵ 00 */
	I2C_MMA_ByteWrite(0x30,MMA_YOFFL_Addr);	/*У��yֵ 48*/
	I2C_MMA_ByteWrite(0xE2,MMA_ZOFFL_Addr);	/*У��Zֵ -30�Ĳ���	*/
	I2C_MMA_ByteWrite(0xFF,MMA_ZOFFH_Addr);	/*У��Zֵ,У��ֵΪ������Ҫ�Ѹ�λд 1;*/		
}

/*
 * ��������I2C_MMA_Test
 * ����  ��������� �� ���ٶ� ������0-2g��
 * ����  �����ݽṹ���ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void I2C_MMA_Test(MMA_Dat* MMA_Value)
{
    u8 temp;

	/*MMA���� 2g ���̲���ģʽ*/
   	I2C_MMA_ByteWrite(0x05,MMA_MCTL_Addr);

	/*DRDY����λ,�ȴ��������*/
	while(!(I2C_MMA_ByteRead(MMA_STATUS_Addr)&0x01));

  	/*��ȡ��õ�����*/
   MMA_Value->Out = I2C_MMA_ByteRead(MMA_Value->Addr);

	if((MMA_Value->Out&0x80) ==0x00) /*������ԭʼֵΪ����	*/ 	
		{ 		
		temp =	MMA_Value->Out;	
		
		/*��ԭʼֵת��Ϊ���ٶȣ�2g����,64LSB/g;  ���� -1 Ϊ������*/ 
	   	MMA_Value->Acc = (float)(-1)*temp *ACC_Gravity/64;
	 
	 		/*��ԭʼֵת��Ϊ�Ƕ�*/
		   if(temp >=64)  
		   		/*���ٶ�ֵ���� 1g */		   
			 	MMA_Value->Angle = 90.0;
		   
		   else  
				/*���ٶ�С��1 g�� Angle = asin(Acc/9.8)*57.32; ������ת��57.32 = 180/3.14*/
		   		MMA_Value->Angle = asin((float)temp/64)*57.32; 
		}
	
	  /*������ԭʼֵΪ���� */
	 else 
	 	{
	 	 temp =	MMA_Value->Out;	 
		 
		 /*������ת��*/	
	 	 temp -= 1;
	 	 temp = ~temp;	

		 /*��ԭʼֵת��Ϊ���ٶ�*/
	 	 MMA_Value->Acc = (float) temp *ACC_Gravity/64;
		
		/*��ԭʼֵת��Ϊ�Ƕȣ����� -1 Ϊ������*/
		if(temp>=64)
		 	MMA_Value->Angle = -90.0;
		else 
			/* Angle = asin(Acc/9.8)*57.32 ; ������ת��57.32 = 180/3.14*/
	  		MMA_Value->Angle = (-1)*asin((float)temp/64)*57.32;     
	 	}  	
}

/*
 * ��������I2C_MMA_Printf
 * ����  ����������������ֵ������
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void  I2C_MMA_Printf(MMA_Dat* MMA_Value)
{
	printf("\r\n�Ĵ�����ԭʼ������: %cOUT8 = %d \r\n",MMA_Value->Name,MMA_Value->Out);	
	printf("\r\n�� ��ͷ��ע ����Ϊ �� ����%c������ٶ���: %4.2f m/s^2\r\n",MMA_Value->Name,MMA_Value->Acc);
	printf("\r\n�� ��ͷ��ע ����Ϊ �� ����, %c������ˮƽ��ļн���: %4.2f ��\r\n",MMA_Value->Name,MMA_Value->Angle);
}

/*
 * ��������I2C_MMA_Standby
 * ����  ����������������Standbyģʽ��ʡ��
 * ����  ����
 * ���  ����
 * ����	 ���Ƿ�ɹ�����Standbyģʽ
 * ����  ���ⲿ����
 */
u8 I2C_MMA_Standby(void)
{
   	u8 MMA_Test;
	
	/*MMA Standby Mode*/	 
	I2C_MMA_ByteWrite(0x04,MMA_MCTL_Addr);	
	
	/*MMA_Test*/   						 	  
	MMA_Test = I2C_MMA_ByteRead(MMA_MCTL_Addr);

	if(MMA_Test == 0x04)
		return 	  SUCCESS ;
	else
		return 	   ERROR;
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/









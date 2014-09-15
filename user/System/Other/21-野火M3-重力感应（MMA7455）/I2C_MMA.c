/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：i2c_ee.c
 * 描述    ：i2c EEPROM(AT24C02)应用函数库         
 * 实验平台：野火STM32开发板
 * 硬件连接：-----------------
 *          |                 |
 *          |  PB6-I2C1_SCL		|
 *          |  PB7-I2C1_SDA   |
 *          |                 |
 *           -----------------
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "I2C_MMA.h"
#include "usart1.h"
#include "math.h"

#define I2C_Speed			 100000
#define MMA_ADRESS	 		 0x3A  /*MMA_Device Address*/
#define ACC_Gravity			 9.8   /*重力加速度的值*/

/*
 * 函数名：I2C_GPIO_Config
 * 描述  ：I2C1 I/O配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
		/* 使能与 I2C1 有关的时钟 */
	RCC_APB2PeriphClockCmd  (RCC_APB2Periph_GPIOB, ENABLE ); 
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  

	 /* PB6-I2C1_SCL、PB7-I2C1_SDA*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


/*
 * 函数名：I2C_Mode_Config
 * 描述  ：I2C 工作模式配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void I2C_Mode_Config(void)
{
	 /* Initialize the I2C1 according to the I2C_InitStructure members */ 
	I2C_InitTypeDef I2C_InitStructure; 
	 
	  /* I2C 配置 */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ; 
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; 
	I2C_InitStructure.I2C_OwnAddress1 = MMA_ADRESS; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed; 
	
	/* 使能 I2C1 */
	I2C_Cmd  (I2C1,ENABLE); 

	/* I2C1 初始化 */
	I2C_Init(I2C1, &I2C_InitStructure);	   

	/*允许应答模式*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);   
}

/*
 * 函数名：I2C_MMA_Init
 * 描述  ：I2C 外设(MMA7455)初始化
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void I2C_MMA_Init(void)
{	   
 	I2C_GPIO_Config();
	I2C_Mode_Config();
}  

/*
 * 函数名：I2C_MMA_ByteWrite
 * 描述  ：写一个字节到I2C MMA寄存器中
 * 输入  ：-pBuffer 缓冲区指针
 *         -WriteAddr 接收数据的MMA寄存器的地址 
 * 输出  ：无
 * 返回  ：无
 * 调用  ：内部调用
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
 * 函数名：I2C_MMA_ByteRead
 * 描述  ：从MMA寄存器里面读取一块数据。 
 * 输入  ： -ReadAddr 接收数据的MMA寄存器的地址。        
 * 输出  ：无
 * 返回  ：读取出的寄存器数据
 * 调用  ：内部调用
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
 * 函数名：I2C_MMA_Cal
 * 描述  ：MMA7455 校准
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用，在初始化后调用
 */
void I2C_MMA_Cal(void)
{   
	I2C_MMA_ByteWrite(0x00,MMA_XOFFL_Addr);	/*校正x值 00 */
	I2C_MMA_ByteWrite(0x30,MMA_YOFFL_Addr);	/*校正y值 48*/
	I2C_MMA_ByteWrite(0xE2,MMA_ZOFFL_Addr);	/*校正Z值 -30的补码	*/
	I2C_MMA_ByteWrite(0xFF,MMA_ZOFFH_Addr);	/*校正Z值,校正值为负数，要把高位写 1;*/		
}

/*
 * 函数名：I2C_MMA_Test
 * 描述  ：测量倾角 和 加速度 （量程0-2g）
 * 输入  ：数据结构体的指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void I2C_MMA_Test(MMA_Dat* MMA_Value)
{
    u8 temp;

	/*MMA进入 2g 量程测试模式*/
   	I2C_MMA_ByteWrite(0x05,MMA_MCTL_Addr);

	/*DRDY标置位,等待测试完毕*/
	while(!(I2C_MMA_ByteRead(MMA_STATUS_Addr)&0x01));

  	/*读取测得的数据*/
   MMA_Value->Out = I2C_MMA_ByteRead(MMA_Value->Addr);

	if((MMA_Value->Out&0x80) ==0x00) /*读出的原始值为正数	*/ 	
		{ 		
		temp =	MMA_Value->Out;	
		
		/*将原始值转换为加速度，2g量程,64LSB/g;  乘以 -1 为方向处理*/ 
	   	MMA_Value->Acc = (float)(-1)*temp *ACC_Gravity/64;
	 
	 		/*将原始值转换为角度*/
		   if(temp >=64)  
		   		/*加速度值大于 1g */		   
			 	MMA_Value->Angle = 90.0;
		   
		   else  
				/*加速度小于1 g， Angle = asin(Acc/9.8)*57.32; 弧度制转换57.32 = 180/3.14*/
		   		MMA_Value->Angle = asin((float)temp/64)*57.32; 
		}
	
	  /*读出的原始值为负数 */
	 else 
	 	{
	 	 temp =	MMA_Value->Out;	 
		 
		 /*二补码转换*/	
	 	 temp -= 1;
	 	 temp = ~temp;	

		 /*将原始值转换为加速度*/
	 	 MMA_Value->Acc = (float) temp *ACC_Gravity/64;
		
		/*将原始值转换为角度，乘以 -1 为方向处理*/
		if(temp>=64)
		 	MMA_Value->Angle = -90.0;
		else 
			/* Angle = asin(Acc/9.8)*57.32 ; 弧度制转换57.32 = 180/3.14*/
	  		MMA_Value->Angle = (-1)*asin((float)temp/64)*57.32;     
	 	}  	
}

/*
 * 函数名：I2C_MMA_Printf
 * 描述  ：输出各方向的数据值到串口
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void  I2C_MMA_Printf(MMA_Dat* MMA_Value)
{
	printf("\r\n寄存器的原始数据是: %cOUT8 = %d \r\n",MMA_Value->Name,MMA_Value->Out);	
	printf("\r\n以 箭头标注 方向为 正 方向，%c方向加速度是: %4.2f m/s^2\r\n",MMA_Value->Name,MMA_Value->Acc);
	printf("\r\n以 箭头标注 方向为 正 方向, %c方向与水平面的夹角是: %4.2f 度\r\n",MMA_Value->Name,MMA_Value->Angle);
}

/*
 * 函数名：I2C_MMA_Standby
 * 描述  ：重力传感器进入Standby模式，省电
 * 输入  ：无
 * 输出  ：无
 * 返回	 ：是否成功进入Standby模式
 * 调用  ：外部调用
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









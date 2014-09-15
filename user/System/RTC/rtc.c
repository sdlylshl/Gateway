/*******************************************************************************
 * �ļ���  ��rtc.c
 * ����    ������STM32��RTCʵ��һ�����׵ĵ���ʱ�ӡ�
 *           ��ʾ��ʽΪ Time: XX : XX : XX(ʱ���֣���)��         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�-------------------
 *          |                   |
 *          | VBAT��������ӵ��|
 *          |                   |
 *           -----------------
 * ��汾  ��ST3.5.0
 * ����    ������ 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "rtc.h"
#include "stdio.h"

/* ���жϱ�־���������ж�ʱ��1����ʱ�䱻ˢ��֮����0 */
__IO uint32_t TimeDisplay;	


/*ʱ��ṹ��*/
struct rtc_time systmtime;


/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
int RTC_test2()
{
			
	  	/* ����RTC���ж����ȼ� */
	  NVIC_Configuration();
	  USART1_Config();
	  RTC_CheckAndConfig(&systmtime);

	
	  /* Display time in infinite loop */
	  Time_Show(&systmtime);
}



int RTC_test(void)
{	
	/* USART1 config */
	USART1_Config();
	
	/* ����RTC���ж����ȼ� */
	NVIC_Configuration();
	
	printf( "\r\n This is a RTC demo...... \r\n" );
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* Backup data register value is not correct or not yet programmed (when
		the first time the program is executed) */
		printf("\r\nThis is a RTC demo!\r\n");
		printf("\r\n\n RTC not yet configured....");
		
		/* RTC Configuration */
		RTC_Configuration();
		
		printf("\r\n RTC configured....");
		
		/* Adjust time by values entred by the user on the hyperterminal */
		Time_Adjust();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n\n Power On Reset occurred....");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n\n External Reset occurred....");
		}
		
		printf("\r\n No need to configure RTC....");
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();
		
		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
	
	#ifdef RTCClockOutput_Enable
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Disable the Tamper Pin */
	BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	             functionality must be disabled */
	
	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	#endif
	
	/* Clear reset flags */
	RCC_ClearFlag();
	
	/* Display time in infinite loop */
	Time_Show();
	
	while (1)
	{
	
	}
}
/*
 * ��������NVIC_Configuration
 * ����  ������RTC���жϵ����ж����ȼ�Ϊ1�������ȼ�Ϊ0
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������RTC_Configuration
 * ����  ������RTC
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Reset Backup Domain */
	BKP_DeInit();
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}
	
	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}


/*
 * ��������Time_Regulate
 * ����  �������û��ڳ����ն��������ʱ��ֵ������ֵ������
 *         RTC �����Ĵ����С�
 * ����  ����
 * ���  ���û��ڳ����ն��������ʱ��ֵ����λΪ s
 * ����  ���ڲ�����
 */
uint32_t Time_Regulate(void)
{
	uint32_t Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;
	
	printf("\r\n==============Time Settings=====================================");
	printf("\r\n  Please Set Hours");
	
	while (Tmp_HH == 0xFF)
	{
		Tmp_HH = USART_Scanf(23);
	}
	printf(":  %d", Tmp_HH);
	printf("\r\n  Please Set Minutes");
	while (Tmp_MM == 0xFF)
	{
		Tmp_MM = USART_Scanf(59);
	}
	printf(":  %d", Tmp_MM);
	printf("\r\n  Please Set Seconds");
	while (Tmp_SS == 0xFF)
	{
		Tmp_SS = USART_Scanf(59);
	}
	printf(":  %d", Tmp_SS);
	
	/* Return the value to store in RTC counter register */
	return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}


/*
 * ��������Time_Adjust
 * ����  ��ʱ�����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void Time_Adjust(void)
{
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Change the current time */
	RTC_SetCounter(Time_Regulate());
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}


/*
 * ��������Time_Display
 * ����  ����ʾ��ǰʱ��ֵ
 * ����  ��-TimeVar RTC����ֵ����λΪ s
 * ���  ����
 * ����  ���ڲ�����
 */	
void Time_Display(uint32_t TimeVar)
{
	uint32_t THH = 0, TMM = 0, TSS = 0;
	
	/* Compute  hours */
	THH = TimeVar / 3600;
	/* Compute minutes */
	TMM = (TimeVar % 3600) / 60;
	/* Compute seconds */
	TSS = (TimeVar % 3600) % 60;
	
	printf(" Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
}


/*
 * ��������Time_Show
 * ����  ���ڳ����ն�����ʾ��ǰʱ��ֵ
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */   
void Time_Show(void)
{
	printf("\n\r");
	
	/* Infinite loop */
	while (1)
	{
		/* If 1s has paased */
		if (TimeDisplay == 1)
		{
			/* Display current time */
			Time_Display(RTC_GetCounter());
			TimeDisplay = 0;
		}
	}
}


/*
 * ��������USART_Scanf
 * ����  �����ڴӳ����ն��л�ȡ��ֵ
 * ����  ��- value �û��ڳ����ն����������ֵ
 * ���  ����
 * ����  ���ڲ�����
 */ 
uint8_t USART_Scanf(uint32_t value)
{
	uint32_t index = 0;
	uint32_t tmp[2] = {0, 0};
	
	while (index < 2)
	{
		/* Loop until RXNE = 1 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
		{}
		tmp[index++] = (USART_ReceiveData(USART1));
		// �Ӵ����ն��������ȥ������ASCII��ֵ
		if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
		{
			printf("\n\rPlease enter valid number between 0 and 9");
			index--;
		}
	}
	/* Calculate the Corresponding value */
	index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
	/* Checks */
	if (index > value)
	{
		printf("\n\rPlease enter valid number between 0 and %d", value);
		return 0xFF;
	}
	return index;
}

/*****************************END OF FILE******************************/

/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��usart2.c
 * ����    ����printf�����ض���USART3�������Ϳ�����printf��������Ƭ��������
 *           ��ӡ��PC�ϵĳ����ն˻򴮿ڵ������֡�
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�---------------------
 *          | 									  |
 *          | PA2  - USART3(Tx)   |
 *          | PA3  - USART3(Rx)   |
 *          |                     |
 *           ---------------------
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "usart.h"
#include <stdarg.h>

//DMA Buff
uint8_t SendBuff[SENDBUFF_SIZE];

/*
 * ��������USART3_Config
 * ����  ��USART3 GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */

void USARTx_Clock(USART_TypeDef* USARTx)
{
    /* config USARTx clock */

    if (USARTx == USART1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
			

    }
    else if (USARTx == USART2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    }
    else
    {
        if (USARTx == USART3)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        }
    }
}
void USARTx_IO_Config(USART_TypeDef* USARTx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (USARTx == USART1)
    {
        /* USART1 GPIO config */
        /* Configure USART1 Tx (PA.09) as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        /* Configure USART1 Rx (PA.10) as input floating */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if (USARTx == USART2)
    {
        /* USART2 GPIO config */
        /* Configure USART2 Tx (PA.02) as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* Configure USART2 Rx (PA.03) as input floating */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else
    {
        if (USARTx == USART3)
        {
            /* USART3 GPIO config */
            /* Configure USART3 Tx (PA.10) as alternate function push-pull */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            /* Configure USART3 Rx (PA.11) as input floating */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
        }
    }
}
/*
 * ��������NVIC_Config
 * ����  ��DMA �ж�����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
static void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* ����P[A|B|C|D|E]0Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//����DMAʱ��
	NVIC_Config();	   			//����DMA�ж�

 	/*����DMAԴ���ڴ��ַ&�������ݼĴ�����ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Base;	   

	/*�ڴ��ַ(Ҫ����ı�����ָ��)*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;
	
	/*���򣺴��ڴ浽����*/		
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	
	
	/*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
	
	/*�����ַ����*/	    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
	
	/*�ڴ��ַ����*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
	
	/*�������ݵ�λ*/	
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	
	/*�ڴ����ݵ�λ 8bit*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
	
	/*DMAģʽ��һ�δ��䣬ѭ��*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;	 
	
	/*���ȼ�����*/	
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  
	
	/*��ֹ�ڴ浽�ڴ�Ĵ���	*/
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	/*����DMA1��4ͨ��*/		   
    DMA_Init(DMA1_Channel4, &DMA_InitStructure); 	   
	
	DMA_Cmd (DMA1_Channel4,ENABLE);					//ʹ��DMA
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);  //����DMA������ɺ�����ж�

}


void NVIC_Configuration(USART_TypeDef* USARTx)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	 /* Set the Vector Table base location at 0x08000000 */
   NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the USARTy Interrupt */
if (USARTx == USART1)
    {
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			
		}
    else if (USARTx == USART2)
    {
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			  
		}
    else
    {
        if (USARTx == USART3)
        {
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);				
	        }
    }				

}
typedef enum {POLLING = 0, INTERRUPT,DMA} USART_RXMode;
void USARTx_ModeConfig(USART_TypeDef* USARTx)
{
    USART_InitTypeDef USART_InitStructure;
	
    if (USARTx == USART1)
    {
        /* USART1 mode config */
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No ;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &USART_InitStructure);
				//
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        USART_Cmd(USART1, ENABLE);
    }
    else if (USARTx == USART2)
    {
        /* USART2 mode config */
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No ;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART2, &USART_InitStructure);
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
        USART_Cmd(USART2, ENABLE);
    }
    else
    {
        if (USARTx == USART3)
        {
            /* USART3 mode config */
            USART_InitStructure.USART_BaudRate = 115200;
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            USART_InitStructure.USART_StopBits = USART_StopBits_1;
            USART_InitStructure.USART_Parity = USART_Parity_No ;
            USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
            USART_Init(USART3, &USART_InitStructure);
					  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
            USART_Cmd(USART3, ENABLE);
        }
    }
		USART_ClearFlag(USART2, USART_FLAG_TC);
}
void USARTx_Init(USART_TypeDef* USARTx)
{
    USARTx_Clock(USARTx);
    USARTx_IO_Config(USARTx);
		USARTx_ModeConfig(USARTx);
	  NVIC_Configuration(USARTx);
}

/*
 * ???:USART1_Config
 * ??  :USART1 GPIO ??,???????115200 8-N-1
 * ??  :?
 * ??  : ?
 * ??  :????
 */
void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}
void USART3_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    //PB10 TX PB11 RX
    /* config USART3 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* USART3 GPIO config */
    /* Configure USART3 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure USART3 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* USART3 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);
}
/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART3
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
int fputc(int ch, FILE *f)
{
    /* ��Printf���ݷ������� */
    USART_SendData(USART1, (unsigned char) ch);
    while (!(USART1->SR & USART_FLAG_TXE));

    return (ch);
}

/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART3_printf()����
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*
 * ��������USART3_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���2����USART3
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 *         ����Ӧ��USART3_printf( USART3, "\r\n this is a demo \r\n" );
 *            		 USART3_printf( USART3, "\r\n %d \r\n", i );
 *            		 USART3_printf( USART3, "\r\n %s \r\n", j );
 */
void USARTx_printf(USART_TypeDef* USARTx, int8_t *Data,...)
{
    const char *s;
    int d;
    char buf[36];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //�س���
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;

            case 'n':							          //���з�
                USART_SendData(USARTx, 0x0a);
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {
            //
            switch ( *++Data )
            {
            case 's':										  //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
                }
                Data++;
                break;

            case 'd':										//ʮ����
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
				
    }
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

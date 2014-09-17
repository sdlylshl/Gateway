/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��Time_test.c
 * ����    ��TIM2 1ms ��ʱӦ�ú�����
 *
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�----------------------
 *          |                     |
 *          |           ��           |
 *          |                     |
 *           ----------------------
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "timer.h"
#include "config.h"

__IO u32 time; // ms ��ʱ����

void TIM2_test()
{

}


void TIM1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*
 * ��������TIM2_NVIC_Configuration
 * ����  ��TIM2�ж����ȼ�����
 * ����  ����
 * ���  ����
 */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void TIM4_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM1_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    //Step1.ʱ�����ã�����TIM1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    //TIM1 ʹ���ڲ�ʱ��
    //TIM_InternalClockConfig(TIM1);
    TIM_DeInit(TIM1);
    //Step2.�ж�NVIC���ã������жϣ��������ȼ�
    TIM1_NVIC_Configuration();
    //Step3.TIMģ������
    //����Ԥ��Ƶ����Ƶϵ��71����APB2=72M, TIM1_CLK=72/72=1MHz
    //TIM_Period��TIM1_ARR��=1000�����������ϼ�����1000����������¼�������ֵ����
    //TIM_RepetitionCounter(TIM1_RCR)=0��ÿ��������������������¼�
    TIM_BaseInitStructure.TIM_Period = 1000;
    TIM_BaseInitStructure.TIM_Prescaler = 71;
    TIM_BaseInitStructure.TIM_ClockDivision = 0;
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure);
    //���жϣ�����һ�����жϺ����������ж�
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    //ʹ��TIM1�ж�Դ
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    //TIM1�ܿ��أ�����
    TIM_Cmd(TIM1, ENABLE);
    //Step4.ע���жϷ����ӳ���
    //���жϷ�������ӵ�stm32f10x_it.c
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);   /*�ȹرյȴ�ʹ��*/
}
/*TIM_Period--1000   TIM_Prescaler--71 -->�ж�����Ϊ1ms*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM2_NVIC_Configuration();
    TIM_TimeBaseStructure.TIM_Period = 1000;
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);      /*�ȹرյȴ�ʹ��*/
}
/*TIM_Period--1000   TIM_Prescaler--71 -->�ж�����Ϊ1ms*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    TIM_DeInit(TIM3);
    TIM3_NVIC_Configuration();
    TIM_TimeBaseStructure.TIM_Period = 1000;
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE);      /*�ȹرյȴ�ʹ��*/
}
/*TIM_Period--1000   TIM_Prescaler--71 -->�ж�����Ϊ1ms*/
void TIM4_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
    TIM_DeInit(TIM4);
    TIM4_NVIC_Configuration();
    TIM_TimeBaseStructure.TIM_Period = 1000;
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , DISABLE);      /*�ȹرյȴ�ʹ��*/
}





// �жϷ����ӳ���

void TIM1_UP_IRQHandle(void)
{
    GPIOC->ODR ^= (1 << 4);                        //����
    TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); //���ж�
}
void TIM1_CC_IRQHandle(void)
{
    u16 capture;
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1 );
        capture = TIM_GetCapture1(TIM1);
        TIM_SetCompare1(TIM1, capture + 2000);
        //���������:
        //��TIM1_CCR1��ֵ����2000��ʹ����һ��TIM�¼�Ҳ��Ҫ2000�����壬
        //��һ�ַ�ʽ���������������
        //TIM_SetCounter(TIM2,0x0000);
    }
}
void TIM2_IRQHandle(void)
{
    if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
    {
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
        time++;
			
			
        if (time>500)
        {
						LED8( ON );
        }
				if(time >1000){
				
					LED8( OFF );
					time=0;
					
				}

    }
}


/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��SysTick.c
 * ����    ��SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
 *           ���õ��� 1us 10us 1ms �жϡ�
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�-----------------
 *          |                 |
 *          |      ��         |
 *          |                 |
 *           -----------------
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "SysTick.h"

static __IO u32 TimingDelay;

/**
  * @brief  SysTick_Init
  * @param
  *     @arg    SYSTICK_1US
                SYSTICK_10US
                SYSTICK_100US
                SYSTICK_1MS
                SYSTICK_10MS
                SYSTICK_100MS
                SYSTICK_1S
  * @retval None
  */
void SysTick_Init(uint32_t dely)
{
    // SystemFrequency / 1000    1ms�ж�һ��
		//SysTick_Config ---- inline core_cm3.h
    //  if (SysTick_Config(SystemFrequency / dely))   // ST3.0.0��汾
		// 1s��ִ��SystemCoreClock��ʱ������
    if (SysTick_Config(SystemCoreClock / dely)) // ST3.5.0��汾
    {
        /* Capture error */
        //TODO DEBUG
        return;
    }
    // �رյδ�ʱ��
    //SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Enable(void)
{
    // ʹ�ܵδ�ʱ��
    SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}
void SysTick_Disable(void)
{
    // �رյδ�ʱ��
    SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}
/*
 * ��������Delay_us
 * ����  ��us��ʱ����,10usΪһ����λ
 * ����  ��- dtime
 * ���  ����
 * ����  ��Delay_us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 10us = 10us
 *       ���ⲿ����
 */

void Delay_us(__IO uint32_t dtime)
{
    TimingDelay = dtime;
    //����SysTick
    SysTick_Enable();

    SysTick_Init(SYSTICK_1US);
    while (TimingDelay != 0);
    SysTick_Disable();
}

void Delay_ms(__IO uint32_t dtime)
{
    TimingDelay = dtime;
    //����SysTick
    SysTick_Enable();

    SysTick_Init(SYSTICK_1MS);
    while (TimingDelay != 0);
     SysTick_Disable();
}

/*
 * ��������SysTick_Handle
 * ����  ����ȡ���ĳ���
 * ����  ����
 * ���  ����
 * ����  ���� SysTick �жϺ��� SysTick_Handler()����
 */
void SysTick_Handle(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

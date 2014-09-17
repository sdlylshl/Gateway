/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：SysTick.c
 * 描述    ：SysTick 系统滴答时钟10us中断函数库,中断时间可自由配置，
 *           常用的有 1us 10us 1ms 中断。
 * 实验平台：野火STM32开发板
 * 硬件连接：-----------------
 *          |                 |
 *          |      无         |
 *          |                 |
 *           -----------------
 * 库版本  ：ST3.5.0
 * 作者    ：wildfire team
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
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
    // SystemFrequency / 1000    1ms中断一次
		//SysTick_Config ---- inline core_cm3.h
    //  if (SysTick_Config(SystemFrequency / dely))   // ST3.0.0库版本
		// 1s钟执行SystemCoreClock个时钟周期
    if (SysTick_Config(SystemCoreClock / dely)) // ST3.5.0库版本
    {
        /* Capture error */
        //TODO DEBUG
        return;
    }
    // 关闭滴答定时器
    //SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Enable(void)
{
    // 使能滴答定时器
    SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}
void SysTick_Disable(void)
{
    // 关闭滴答定时器
    SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}
/*
 * 函数名：Delay_us
 * 描述  ：us延时程序,10us为一个单位
 * 输入  ：- dtime
 * 输出  ：无
 * 调用  ：Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
 *       ：外部调用
 */

void Delay_us(__IO uint32_t dtime)
{
    TimingDelay = dtime;
    //启动SysTick
    SysTick_Enable();

    SysTick_Init(SYSTICK_1US);
    while (TimingDelay != 0);
    SysTick_Disable();
}

void Delay_ms(__IO uint32_t dtime)
{
    TimingDelay = dtime;
    //启动SysTick
    SysTick_Enable();

    SysTick_Init(SYSTICK_1MS);
    while (TimingDelay != 0);
     SysTick_Disable();
}

/*
 * 函数名：SysTick_Handle
 * 描述  ：获取节拍程序
 * 输入  ：无
 * 输出  ：无
 * 调用  ：在 SysTick 中断函数 SysTick_Handler()调用
 */
void SysTick_Handle(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

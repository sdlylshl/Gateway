#ifndef TIME_TEST_H
#define TIME_TEST_H

#include "stm32f10x.h"

extern __IO uint32_t time;
extern __IO uint32_t Zigbee_time;
extern __IO uint32_t Net_time;
extern __IO uint32_t time_out;

#define START_TIME()  time=0;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME()  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

extern void TIM2_Configuration(void);
extern void TIM2_IRQHandle(void);
#endif	/* TIME_TEST_H */

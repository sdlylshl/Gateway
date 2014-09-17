#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

#define  SYSTICK_1US    1000000
#define  SYSTICK_10US   100000
#define  SYSTICK_100US  10000
#define  SYSTICK_1MS    1000
#define  SYSTICK_10MS   100
#define  SYSTICK_100MS  10
#define  SYSTICK_1S     1

void Delay_us(__IO u32 nTime);	  //单位10us

#define Delay_ms(x) Delay_us(100*x)	 //单位ms

void SysTick_Handle(void);

#endif /* __SYSTICK_H */

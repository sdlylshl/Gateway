#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>

//DMA Mode
#define USART1_DR_Base  0x40013804
#define SENDBUFF_SIZE 5000
//DMA Buff
extern uint8_t SendBuff[SENDBUFF_SIZE];

extern void USARTx_Init(USART_TypeDef* USARTx);
extern void USARTx_printf(USART_TypeDef* USARTx, int8_t *Data,...);

void USART3_Config(void);
int fputc(int ch, FILE *f);
void USART3_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
extern void USART1_Config(void);
#endif /* __USART3_H */

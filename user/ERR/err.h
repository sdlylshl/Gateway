
#ifndef __ERROR_H
#define __ERROR_H

#define DATAFLOW 0x01
#define NOSPACE 0x02

#define OK 0
#define ERROR 0xFF

#define SUCCESS 0
#define FAIL 1

#define USARTn USART1
#define  DEBUG0 USARTx_printf
#define  DEBUG debg

extern void debg(USART_TypeDef* USARTx, int8_t *Data,...);


#endif

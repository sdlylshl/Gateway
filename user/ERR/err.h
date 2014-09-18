
#ifndef __ERROR_H
#define __ERROR_H


#define OK 0
#define ERROR 0xFF

//***1.**申请相关
//无可用设备
#define ERROR_NODEV 0x10
//发送缓冲区满
#define ERROR_NOSENDBUFF 0x11
//数据溢出
#define ERROR_DATAFLOW 0x12

#define ERROR_NOSPACE 0x13
//***2.**接收相关

// 未收到数据
#define ERROR_NODATA 0x02


//***3.**发送相关

//zigbee协调器发送失败
#define ERROR_ZIGBEE_SENDERR 0x30
#define ERROR_ZIGBEE_SENDOK 0x30
//**4.**系统相关
#define ERROR_TIMEOUT 0x40


#define SUCCESS 0
#define FAIL 1

#define USARTn USART1
#define  DEBUG0 USARTx_printf
#define  DEBUG debg

extern void debg(USART_TypeDef* USARTx, int8_t *Data,...);


#endif

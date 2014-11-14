#ifndef __I2C_MMA_H
#define	__I2C_MMA_H

#include "stm32f10x.h"

typedef struct
{
	uc8 Addr;		/*寄存器的地址*/
	uc8 Name;		/*数据的方向，X，Y或Z*/  
	s8 Out;			/*寄存器的值*/
	float Acc;		/*加速度值	*/
	float Angle; 	/*角度值	*/
}MMA_Dat;


/* MMA7455 Register Address ------------------------------------------------------------*/
#define		MMA_XOUTL_Addr	0x00
#define		MMA_XOUTH_Addr	0x01
#define		MMA_YOUTL_Addr	0x02
#define		MMA_YOUTH_Addr	0x03
#define		MMA_ZOUTL_Addr	0x04
#define		MMA_ZOUTH_Addr	0x05
#define		MMA_XOUT8_Addr	0x06
#define		MMA_YOUT8_Addr	0x07
#define		MMA_ZOUT8_Addr	0x08
#define		MMA_STATUS_Addr	0x09
#define		MMA_DETSRC_Addr	0x0A
#define		MMA_TOUT_Addr	0x0B
#define		MMA_I2CAD_Addr	0x0D
#define		MMA_USRINF_Addr	0x0E
#define		MMA_WHOAMI_Addr	0x0F
#define		MMA_XOFFL_Addr	0x10
#define		MMA_XOFFH_Addr	0x11
#define		MMA_YOFFL_Addr	0x12
#define		MMA_YOFFH_Addr	0x13
#define		MMA_ZOFFL_Addr	0x14
#define		MMA_ZOFFH_Addr	0x15
#define		MMA_MCTL_Addr	0x16
#define		MMA_INTRST_Addr	0x17
#define		MMA_CTL1_Addr	0x18
#define		MMA_CTL2_Addr	0x19
#define		MMA_LDTH_Addr	0x1A
#define		MMA_PDTH_Addr	0x1B
#define		MMA_PW_Addr		0x1C
#define		MMA_LT_Addr		0x1D
#define		MMA_TW_Addr		0x1E



void 	I2C_MMA_Init(void); 
u8 		I2C_MMA_Standby(void);
void 	I2C_MMA_Cal(void);
void 	I2C_MMA_Test(MMA_Dat* MMA_Value);
void  	I2C_MMA_Printf(MMA_Dat* MMA_Value);


#endif /* __I2C_MMA_H */










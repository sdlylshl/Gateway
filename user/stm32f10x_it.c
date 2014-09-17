/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <stdio.h>
#include "config.h"
void none(USART_TypeDef* USARTx, int8_t *Data,...){


}
#define Interrupt_DBG none
#define Interrupt_DBG_USARTx USART3

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n NMI_Handler \r\n");
    while (1) {}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n HardFault_Handler \r\n");
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n MemManage_Handler \r\n");
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n BusFault_Handler \r\n");
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n UsageFault_Handler \r\n");
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n SVC_Handler \r\n");
    while (1) {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n DebugMon_Handler \r\n");
    while (1) {}
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n PendSV_Handler \r\n");
    while (1) {}
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "\r\n SysTick_Handler \r\n");
    SysTick_Handle();
}

/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
void WWDG_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "WWDG_IRQHandler");
    while (1) {}
}
void PVD_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "PVD_IRQHandler");
    while (1) {}
}
void TAMPER_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "TAMPER_IRQHandler");
    while (1) {}
}
void RTC_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "RTC_IRQHandler");
    while (1) {}
}
void FLASH_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "FLASH_IRQHandler");
    while (1) {}
}
void RCC_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "RCC_IRQHandler");
    while (1) {}
}
void EXTI0_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "EXTI0_IRQHandler");
    while (1) {}
}
void EXTI1_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "EXTI1_IRQHandler");
    while (1) {}
}
void EXTI2_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "EXTI2_IRQHandler");
    while (1) {}
}
void EXTI3_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "EXTI3_IRQHandler");
    while (1) {}
}
void EXTI4_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "EXTI4_IRQHandler");
    while (1) {}
}
void DMA1_Channel1_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "DMA1_Channel1_IRQHandler");
    while (1) {}
}
void DMA1_Channel2_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "DMA1_Channel2_IRQHandler");
    while (1) {}
}
void DMA1_Channel3_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "DMA1_Channel3_IRQHandler");
    while (1) {}
}
void DMA1_Channel4_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "DMA1_Channel4_IRQHandler");
    while (1) {}
}
void DMA1_Channel5_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "DMA1_Channel5_IRQHandler");
    while (1) {}
}
void DMA1_Channel6_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "DMA1_Channel6_IRQHandler");
    while (1) {}
}
void DMA1_Channel7_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "DMA1_Channel7_IRQHandler");
    while (1) {}
}
void ADC1_2_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "ADC1_2_IRQHandler");
    while (1) {}
}
void USB_HP_CAN1_TX_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "USB_HP_CAN1_TX_IRQHandler");
    while (1) {}
}
void USB_LP_CAN1_RX0_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "USB_LP_CAN1_RX0_IRQHandler");
    while (1) {}
}
void CAN1_RX1_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "CAN1_RX1_IRQHandler");
    while (1) {}
}
void CAN1_SCE_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "CAN1_SCE_IRQHandler");
    while (1) {}
}
void EXTI9_5_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "EXTI9_5_IRQHandler");
    while (1) {}
}
void TIM1_BRK_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "TIM1_BRK_IRQHandler");
    while (1) {}
}
void TIM1_UP_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "TIM1_UP_IRQHandler");
    while (1) {}
}
void TIM1_TRG_COM_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "TIM1_TRG_COM_IRQHandler");
    while (1) {}
}
void TIM1_CC_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "TIM1_CC_IRQHandler");
    while (1) {}
}

void TIM2_IRQHandler(void)
{
    if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
    {
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
        time++;
    }
}
void TIM3_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "TIM3_IRQHandler");
    while (1) {}
}
void TIM4_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "TIM4_IRQHandler");
    while (1) {}
}
void I2C1_EV_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "I2C1_EV_IRQHandler");
    while (1) {}
}
void I2C1_ER_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "I2C1_ER_IRQHandler");
    while (1) {}
}
void I2C2_EV_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "I2C2_EV_IRQHandler");
    while (1) {}
}
void I2C2_ER_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "I2C2_ER_IRQHandler");
    while (1) {}
}
void SPI1_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "SPI1_IRQHandler");
    while (1) {}
}
void SPI2_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "SPI2_IRQHandler");
    while (1) {}
}
//在中断服务程序中，由于主机响应中断时并不知道是哪个中断源发出中断请求，
//因此必须在中断服务程序中对中断源进行判别，然后分别进行处理。
//当然，如果只涉及到一个中断请求，是不用做上述判别的。但是无论什么情况，做上述判别是个好习惯

//usart1 === net
void USART1_IRQHandler(void)
{
    uint8_t c;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        c = USART1->DR;
        NET_buf[NET_write] = c;
        if (NET_write == NET_BUFFSIZE - 1)
            NET_write = 0;
        else
            NET_write++;
        //TO TEST print usart1 == net out
        //Net_PutChar(c);
    }


}
//usart2 === zigbee
void USART2_IRQHandler(void)
{
    uint8_t c;

    Interrupt_DBG(Interrupt_DBG_USARTx, "USART2_IRQHandler");

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        c = USART2->DR;

        Zigbee_buf[Zigbee_write] = c;
        if (Zigbee_write == ZIGBEE_BUFFSIZE - 1)
            Zigbee_write = 0;
        else
            Zigbee_write++;

        //TO TEST print usart1 == net out
        Net_PutChar(c);
    }

}

void USART3_IRQHandler(void)
{
    int8_t c;
    Interrupt_DBG(Interrupt_DBG_USARTx, "USART3_IRQHandler");
    //clear IT flag
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
        {
            c = USART3->DR;

        }

}

void EXTI15_10_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "EXTI15_10_IRQHandler");
    while (1) {}
}
void RTCAlarm_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "RTCAlarm_IRQHandler");
    while (1) {}

}
void USBWakeUp_IRQHandler()
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "USBWakeUp_IRQHandler");
    while (1) {}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void PPP_IRQHandler(void)
{
    Interrupt_DBG(Interrupt_DBG_USARTx, "PPP_IRQHandler");
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

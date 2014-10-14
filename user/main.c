/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
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
#include "stm32f10x.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/

#include <stdio.h>

#include "config.h"

void Delay(__IO u32 nCount)
{
    for (; nCount != 0; nCount--);
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{


    //uint8_t buffer[10];
    LED_GPIO_Config();

    /* USART1 config 115200 8-N-1 */
    //USART1_Config();
    USARTx_Init(USART1);
    USARTx_Init(USART2);
    USARTx_Init(USART3);
    //printf("\r\n this is a printf demo \r\n");
    USARTx_printf(USART1, "\r\n This is a USART1_printf demo \r\n");
    USARTx_printf(USART2, "\r\n This is a USART2_printf demo \r\n");
    USARTx_printf(USART3, "\r\n This is a USART3_printf demo \r\n");
    //USARTx_printf(USART1, "\r\n ("__DATE__ " - " __TIME__ ") \r\n");

    //设备模拟初始化
    //devInit();

    // CRC启动
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
    /* Infinite loop */
    TIM2_Configuration();
    START_TIME();
    policy_init();
    //1.获取设备信息 AT+NWS
    Zigbee_reset_default(1);
    while (1)
    {
        //取命令 并解析
        Zigbee_fetchParseInstruction();
        NET_fetchParseInstruction();

        //策略实现
        strategy_implementation();

        // print_CMDS();
        // print_DEVS();

        //定时执行 对执行来说 不关心是否执行成功 所以 要定时执行
        //2S 执行一次
        // zigbee_operate_default(1000*2);

        //
        //定时查询默认状态设备状态
				//Zigbee_getActstate_timer( 1000*50);
				//定时更新设备电量
				//Zigbee_getBattery(1000*60*60);
        //定时获取设备信息

        //重发机制 1S重发一次 清理一次

        Net_send_Timer(100);
        // 获取控制器状态信息
				Zigbee_getActstate_timer(1000*30);
        // 5s 复位控制器状态
        Zigbee_reset_default(1000*5);
				// Zigbee 发送缓冲区 发送规则
        Zigbee_send_Timer(100);
       // 隔段时间清除 查询状态标志，防止未接收到信息。
       Zigbee_ClearRequest(1000*60*5);

    }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

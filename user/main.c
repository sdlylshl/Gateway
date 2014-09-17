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
    //uint8_t i;
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
		devInit();

		// CRC启动
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
    /* Infinite loop */
    TIM2_Configuration();
    START_TIME();

    //1.获取设备信息 AT+NWS

    while (1)
    {
        //取命令 并解析
        Zigbee_fetchParseInstruction();
        NET_fetchParseInstruction();

        //策略解析，修改对应 设备表
        policydecisions();

        // print_CMDS();
        // print_DEVS();

        //定时执行 对执行来说 不关心是否执行成功 所以 要定时执行
        //2.5S 执行一次
        if (Zigbee_time>2500)
        {
          zigbee_operate_ALL();
          Zigbee_time=0;
        }
        //
        //定时查询状态

        //重发机制 1S重发一次 清理一次
        if (Net_time>100)
        {
          Net_sendTimer();
          Net_time=0;
        }



        //zigbee_remote_set_net_io(3600, IO_D2, IO_MODE_GPIO_OUTPUT_0,  0);


        //zigbee_remote_set_net_io(3600, IO_D2, IO_MODE_GPIO_OUTPUT_1,  0);
        // strategy_implementation();

        // zigbee_send_data(1, 3600, buffer);
        // zigbee_send_data(1, 3601, buffer);
        // zigbee_remote_set_net_io(3600, IO_D2 , IO_MODE_GPIO_OUTPUT_1, 0);
        // zigbee_remote_set_net_io(3601, IO_D2 , IO_MODE_GPIO_OUTPUT_1, 0);


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

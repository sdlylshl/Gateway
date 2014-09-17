
#include "config.h"


//全局设备表
extern struct devTable  devTbs[MAX_DEVTABLE_NUM];

/**
  * @brief  控制策略执行 循环检测 设备状态表
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
    * @writer lishoulei
    *   @modify
  */
void strategy_implementation(void)
{
    uint8_t i;
    uint16_t status;
    uint8_t IOmode;

    for (i = 0; i < MAX_DEVTABLE_NUM; i++)
    {
        status = (devTbs[i].ActSt ^ devTbs[i].ActSt);
        //查询状态指令
        if (status & 0x8000)
        {

        }
        //检测是否有需要执行的设备
        if (status & 0x7FFF)
        {


					/*/检测要控制的IO
            for (j = 0; j < 5; j++)
            {

                if (status & (1 << j))
                {

                    if (devTbs[i].ActSt & (1 << j))
                    {
                        IOmode = IO_MODE_GPIO_OUTPUT_1;
                    }
                    else
                    {
                        IOmode = IO_MODE_GPIO_OUTPUT_0;
                    }

                    zigbee_remote_set_net_io(devTbs[i].netId, j, IOmode , 0);

                }
            }
						*/
						if (devTbs[i].ActSt & 1 )
                    {
                        IOmode = IO_MODE_GPIO_OUTPUT_1;
                    }
                    else
                    {
                        IOmode = IO_MODE_GPIO_OUTPUT_0;
                    }

						 zigbee_remote_set_net_io(devTbs[i].netId, IO_D2, IOmode , 0);


        }
    }




}

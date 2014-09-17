
#include "config.h"

//最大全局策略表大小
struct strgytable strategytable[MAX_DESTABLE_NUM];

/**********************************policy**************************************/
/**
  * @brief  从策略表中获取可填充策略位置
  * @param  None
  * @retval None
    * @writer liuzhishan
    *   @modify
  */
struct strgytable *getANewDES(void)
{
    uint32_t i = 0;
    while (i < MAX_DESTABLE_NUM)
    {
        if (strategytable[i].usable == 0)
        {
            return &strategytable[i];
        }
        i++;
    }
    return NULL;
}

//策略解析
void  policydecisions(void)
{
    uint32_t i = 0;
    DEBUG(USARTn, "\r\n policydecisions \r\n");
    while (i < MAX_DESTABLE_NUM)
    {
        if (strategytable[i].usable)
        {
            if (strategytable[i].num == 0) //只有一个传感设备、一个执行设备
            {
                struct devTable *staDev = getDevTbsByMac(strategytable[i].sensor[0].sensorId);
                if (staDev->curSt == strategytable[i].sensor[0].sensorTrigger & 0xffffffff)
                {
                    struct devTable *actDev = getDevTbsByMac(strategytable[i].actuator[0].actuatorId);
                    actDev->ActSt = strategytable[i].actuator[0].actuatorTrigger && 0xffffffff;
                }
            }
            else
            {
                uint8_t numSta = (strategytable[i].num >> 6) & 0x03;
                uint8_t type;
                uint8_t flagTrue = 0;
                uint8_t j = 0;
                uint32_t statusValue;
                //传感器解析
                while (j < numSta)
                {

                    //获取对应的设备
                    struct devTable *staDev = getDevTbsByMac(strategytable[i].sensor[j].sensorId);
                    if (staDev == NULL)
                    {
                        //TODO ERROR
                        return;
                    }

                    flagTrue = 1;
                    //触发类型type
                    type = strategytable[i].type >> (j << 1);

                    statusValue = ((staDev->ActSt & 0x7FFF) << 16) | staDev->curSt;
                    //开关型触发
                    if (1 == type)
                    {
                        if (statusValue ^ strategytable[i].sensor[j].sensorTrigger)
                        {
                            //不相等
                            flagTrue = 0;
                            break;
                        }
                    }
                    //小触发   0b10
                    else if (2 == type)
                    {
                        if (statusValue > strategytable[i].sensor[j].sensorTrigger)
                        {
                            flagTrue = 0;
                            break;
                        }
                    }
                    // 大触发 0b11
                    else if (3 == type)
                    {
                        if (statusValue < strategytable[i].sensor[j].sensorTrigger)
                        {
                            flagTrue = 0;
                            break;
                        }
                    }

                    j++;
                }
                // 执行器解析
                if (flagTrue == 1)
                {
                    uint8_t numAct = strategytable[i].num & 0x3f;
                    uint32_t j = 0;
                    while (j < numAct)
                    {
                        struct devTable *actDev = getDevTbsByMac(strategytable[i].actuator[j].actuatorId);
                        if (actDev == NULL)
                        {
                            //TODO ERROR
                            return;
                        }
                        actDev->ActSt = strategytable[i].actuator[j].actuatorTrigger  & 0x1f;//最多设置5个IO
                        j++;
                    }
                }

            }
        }

        i++;
    }
}

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

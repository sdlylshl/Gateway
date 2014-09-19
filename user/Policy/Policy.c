
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

//策略执行
void  strategy_implementation(void)
{
    uint8_t i = 0;
    DEBUG(USARTn, "\r\n strategy_implementation \r\n");
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
                uint16_t statusValue;
                //传感器解析,必须所有的条件全部满足，flagTrue才置1
                for (j = 0; j < numSta; i++)
                 {
                    //获取对应的设备
                    struct devTable *staDev = getDevTbsByMac(strategytable[i].sensor[j].sensorId);
                    if (staDev == NULL)
                    {
                        //TODO ERROR
                        return;
                    }

                    flagTrue = 1;
                    //触发类型type 两位表示一个设备 共表示4个设备

                    type = strategytable[i].type >> (j << 1);

                    statusValue = staDev->curSt;
                    // 翻转触发  0b00
                    if (0 == type)
                    {
                        //上次执行的状态，
                    }
                    //开关型触发 0b01
                    else if (1 == type)
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
                            //TODO: ERROR
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


#include "config.h"

//最大全局策略表大小
struct strgytable strategytable[MAX_DESTABLE_NUM];
//开关策略
struct strgy_swtable strategy_swtable[MAX_DESTABLE_NUM];
/**********************************policy**************************************/
/**
  * @brief  从策略表中获取可填充策略位置
  * @param  None
  * @retval None
    * @writer liuzhishan
    *   @modify
  */
struct strgytable *getNewStrategy(void)
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
// 开关策略执行

// 普通策略执行
void  strategy_implementation(void)
{
 /*   uint8_t i = 0;
    DEBUG(USARTn, "\r\n strategy_implementation \r\n");
    for (i = 0; i < MAX_DESTABLE_NUM; i++)
    {
        if (strategytable[i].usable)
        {
            // 获取传感器触发数量
            uint8_t numSta = (strategytable[i].num >> 6) & 0x03;
            uint8_t type;
            uint8_t flagTrue = 1;
            uint8_t j = 0;
            uint16_t statusValue;
            //传感器解析,必须所有的条件全部满足，flagTrue才置1
            for (j = 0; j < numSta; i++)
            {
                //获取对应的设备
                struct devTable *staDev = getDevTbsByMac(strategytable[i].sensor[j].deviceId);
                if (staDev == NULL)
                {
                    //TODO ERROR
                    break;
                }


                statusValue = staDev->curSt;
                //触发类型type 两位表示一个设备 共表示4个设备 每次移2位
                type = (strategytable[i].type >> (j << 1)) & 0x03;

                // 翻转触发  0b00
                if (0 == type)
                {
                    //上次执行的状态
                     flagTrue = 0;
                }
                //开关型触发 0b01
                else if (1 == type)
                {
                    if (statusValue ^ strategytable[i].sensor[j].deviceSta)
                    {
                        //不相等
                        flagTrue = 0;
                        // 只要有一个条件不满足，直接退出
                        break;
                    }
                }
                //小触发   0b10
                else if (2 == type)
                {
                    if (statusValue > strategytable[i].sensor[j].deviceSta)
                    {
                        flagTrue = 0;
                        break;
                    }
                }
                // 大触发 0b11
                else if (3 == type)
                {
                    if (statusValue < strategytable[i].sensor[j].deviceSta)
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
                uint8_t j = 0;
                for (j = 0; i < numAct; i++)
                {
                    struct devTable *actDev = getDevTbsByMac(strategytable[i].actuators[j].deviceId);
                    if (actDev != NULL)
                    {
                        actDev->ActSt = strategytable[i].actuators[j].deviceSta  & 0x1fFF;//最多设置5个IO
                    }

                }
            }


        }
    }// end for*/
}


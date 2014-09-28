
#include "config.h"

//最大全局策略表大小
struct strgytable strategy_tables[MAX_DESTABLE_NUM];
//开关策略
struct strgy_swtable strategy_swtables[MAX_DESTABLE_NUM];


extern void zigbee_operate(struct devTable *pdevTbs, uint8_t force, uint8_t immediate);
extern void Zigebee_setIOBynetId(struct devTable *pdevTbs);


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
        if (strategy_tables[i].usable == 0)
        {
            return &strategy_tables[i];
        }
        i++;
    }
    return NULL;
}
struct strgytable *getHighPriorityStrategy(void)
{

    return NULL;
}

void strategy_RedefinePriority(void)
{
    uint8_t i;
    for (i = 0; i < MAX_DEVTABLE_NUM; i++)
    {
        if (devTbs[i].devstate )
        {
            if (devTbs[i].priority < PRIORITY_HIGHEST )
            {
                devTbs[i].priority  = PRIORITY_LOWEST;
            }
        }
    }
}
// 开关策略执行

// 普通策略执行
void  strategy_implementation(void)
{
    uint8_t i = 0;
    struct devTable *pdevTbs=NULL;
    //DEBUG(USARTn, "\r\n strategy_implementation \r\n");
    for (i = 0; i < MAX_DESTABLE_NUM; i++)
    {
        if (strategy_tables[i].usable)
        {
            // 获取传感器触发数量
            uint8_t numSta = (strategy_tables[i].num >> 4) & 0x0F;
            uint8_t type;
            uint8_t flagTrue = 1;
            uint8_t j = 0;;
            //传感器解析,必须所有的条件全部满足，flagTrue才置1
            for (j = 0; j < numSta; j++)
            {
                //获取对应的设备
                pdevTbs = getDevTbsByNetId(strategy_tables[i].sensors[j].sensorId);
                if (pdevTbs == NULL)
                {
                    flagTrue = 0;
                    //TODO ERROR
                    break;
                }

                
                //触发类型type 两位表示一个设备 共表示4个设备 每次移2位
                type = (strategy_tables[i].type >> (j << 1)) & 0x03;

                // 翻转触发  0b00
                if (0 == type)
                {
                    //上次执行的状态
                    flagTrue = 0;
                    break;
                }
                //开关型触发 0b01
                else if (1 == type)
                {
                    if (pdevTbs->statetables[pdevTbs->ion].curstat ^ strategy_tables[i].sensors[j].sensorState)
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
                    if (pdevTbs->statetables[pdevTbs->ion].curstat > strategy_tables[i].sensors[j].sensorState)
                    {
                        flagTrue = 0;
                        break;
                    }
                }
                // 大触发 0b11
                else if (3 == type)
                {
                    if (pdevTbs->statetables[pdevTbs->ion].curstat < strategy_tables[i].sensors[j].sensorState)
                    {
                        flagTrue = 0;
                        break;
                    }
                }

            }
            // 执行器解析
            if (flagTrue == 1)
            {
                uint8_t j = 0;
                uint8_t numAct = strategy_tables[i].num & 0x0f;
                if (numAct < MAX_DEVTABLE_NUM)
                {
                    for (j = 0; j < numAct; j++)
                    {
                        pdevTbs = getDevTbsByNetId(strategy_tables[i].actuators[j].actuatorId);
                        if (pdevTbs != NULL)
                        {
                            // 1.先检测优先级
                            if (pdevTbs->priority < strategy_tables[i].priority)
                            {
                                Device_operateFlag(pdevTbs);
                                Zigebee_setIOBynetId(pdevTbs);
                                pdevTbs->statetables[pdevTbs->ion].iomode = (uint8_t)strategy_tables[i].actuators[j].actuatorIO;

                                //zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, pdevTbs->statetables[pdevTbs->ion].iomode, 0, immediate);
                                pdevTbs->priority = strategy_tables[i].priority;
                            }
                        }

                    }

                }

            }//end act


        }
    }// end for
    zigbee_operate_ALL();
    // 重新应用优先级
    strategy_RedefinePriority();
}

void strategy_init(void)
{
		strategy_tables[0].priority = 1;
        strategy_tables[0].num =0x11;
        strategy_tables[0].type =0x01;
        strategy_tables[0].usable =0x01;
        strategy_tables[0].sensors[0].sensorId =0x8102;
        strategy_tables[0].sensors[0].sensorState =0x1;

        strategy_tables[0].actuators[0].actuatorId =0x0102;
        strategy_tables[0].actuators[0].actuatorIO =IO_MODE_GPIO_OUTPUT_1;

        strategy_tables[1].priority = 2;
        strategy_tables[1].num =0x11;
        strategy_tables[1].type =0x01;
        strategy_tables[1].usable =0x01;
        strategy_tables[1].sensors[0].sensorId =0x8102;
        strategy_tables[1].sensors[0].sensorState =0x0;

        strategy_tables[1].actuators[0].actuatorId =0x0102;
        strategy_tables[1].actuators[0].actuatorIO =IO_MODE_GPIO_OUTPUT_0;


}

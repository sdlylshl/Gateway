
#include "config.h"
//最大全局策略表大小
struct strgytable strategy_tables[MAX_DESTABLE_NUM];
//开关策略
struct strgy_swtable strategy_swtables[MAX_DESTABLE_NUM];
extern void zigbee_operate(struct devTable *pdevTbs, uint8_t priority, uint8_t force, uint8_t immediate);
extern void Zigebee_setIOBynetId(struct devTable *pdevTbs);
uint8_t MODE = MODE_DEFAULT;
struct devTable *pdevTbs_curtain = NULL;
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
void Zigbee_ClearPriority( void)
{
    uint8_t i;
    for (i = 0; i < MAX_DEVTABLE_NUM; i++)
    {
        if (devTbs[i].devstate)
        {
            devTbs[i].priority = PRIORITY_LOWEST;
        }
    }
}
// 开关策略执行
// 普通策略执行
void  strategy_implementation(void)
{
    uint8_t i = 0;
    struct devTable *pdevTbs = NULL;
    //DEBUG(USARTn, "\r\n strategy_implementation \r\n");
    for (i = 0; i < MAX_DESTABLE_NUM; i++)
    {
        // 当前策略是否有效 usable = MODE_HOME|
        if (strategy_tables[i].usable)
        {

            uint8_t type;
            uint8_t flagTrue = 1; //执行器触发
            uint8_t j = 0;
            // 获取传感器触发数量
            uint8_t numSta = (strategy_tables[i].num >> 4) & 0x0F;
            // 获取执行器触发数量
            uint8_t numAct = strategy_tables[i].num & 0x0f;
            //传感器解析,必须所有的条件全部满足，flagTrue才置1
            for (j = 0; j < numSta; j++)
            {
                //获取对应的设备
                pdevTbs = getDevTbsByNetId(strategy_tables[i].sensors[j].sensorId);
                if (pdevTbs == NULL)
                {
                    flagTrue = 0xFF;
                    //TODO ERROR
                    break;
                }
                //触发类型type 两位表示一个设备 共表示4个设备 每次移2位
                type = (strategy_tables[i].type >> (j << 1)) & 0x03;
                // 翻转触发  0b00
                if (TRIGGER_FLIP == type)
                {
                    //TODO: 翻转触发 上次执行的状态
                    // 1.获取上次状态
                    // 2.比较当前状态的上次状态是否一致
                    flagTrue = 0;
                    break;
                }
                /*
                //开关型触发 0b01
                else if (TRIGGER_EQUAL == type)
                {
                    if (pdevTbs->statetables[pdevTbs->ion].curstat ^ strategy_tables[i].sensors[j].sensorState)
                    {
                        //不相等
                        flagTrue = 0;
                        // 只要有一个条件不满足，直接退出
                        break;
                    }
                }*/
                //小于触发   0b10
                else if ( TRIGGER_SMALL == type)
                {
                    if (pdevTbs->statetables[pdevTbs->ion].curstat >= strategy_tables[i].sensors[j].sensorState)
                    {
                        flagTrue = 0;
                        break;
                    }
                }
                // 大于等于 0b11
                else if (TRIGGER_BIG_EQUAL == type)
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
                if (numAct < MAX_DEVTABLE_NUM)
                {
                    for (j = 0; j < numAct; j++)
                    {
                        pdevTbs = getDevTbsByNetId(strategy_tables[i].actuators[j].actuatorId);
                        if (pdevTbs != NULL)
                        {
                            // 1.先检测优先级 优先级高的 或同级的可执行
                            if (strategy_tables[i].priority > pdevTbs->priority)
                            {
                                Device_operateFlag(pdevTbs);
                                Zigebee_setIOBynetId(pdevTbs);

                                pdevTbs->statetables[pdevTbs->ion].iomode = (uint8_t)strategy_tables[i].actuators[j].actuatorIO;

                                /*
                                else
                                {

                                    switch ((uint8_t)strategy_tables[i].actuators[j].actuatorIO)
                                    {
                                    case IO_MODE_GPIO_OUTPUT_0 :
                                        pdevTbs->statetables[pdevTbs->ion].ioMODE = IO_MODE_GPIO_OUTPUT_1;
                                        break;
                                    case IO_MODE_GPIO_OUTPUT_1 :
                                        pdevTbs->statetables[pdevTbs->ion].ioMODE = IO_MODE_GPIO_OUTPUT_0;
                                        break;
                                    case IO_MODE_CURTAIN_FORTH :
                                        pdevTbs->statetables[pdevTbs->ion].ioMODE = IO_MODE_CURTAIN_BACK;
                                        pdevTbs_curtain = pdevTbs;
                                        break;
                                    case IO_MODE_CURTAIN_BACK :
                                        pdevTbs->statetables[pdevTbs->ion].ioMODE = IO_MODE_CURTAIN_FORTH;
                                        pdevTbs_curtain = pdevTbs;
                                        break;
                                    }

                                }*/
                                //zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, pdevTbs->statetables[pdevTbs->ion].ioMODE, 0, immediate);
                                pdevTbs->priority = strategy_tables[i].priority;
                            }
                        }
                    }
                }
            }
        }
    }// end for
    // 立即执行
    zigbee_operate_default(0);
    // 重新应用优先级
    strategy_RedefinePriority();
}
void strategy_init(void)
{
    // 0x0~0xF 公共策略
    // 燃气报警
    strategy_tables[0x0F].num = 0x12;
    strategy_tables[0x0F].priority = 0x0F;
    strategy_tables[0x0F].usable = 0xFF;
    strategy_tables[0x0F].type = TRIGGER_BIG_EQUAL;
    //燃气传感器IO1
    strategy_tables[0x0F].sensors[0].sensorId = (DEV_SENSOR_SMOKE + 1);
    strategy_tables[0x0F].sensors[0].sensorState = 0x0300;
    // 1.灯1
    strategy_tables[0x0F].actuators[0].actuatorId = (DEV_ACT_LIGHT + 1);
    strategy_tables[0x0F].actuators[0].actuatorIO = IO_MODE_GPIO_OUTPUT_1;
    // 2.报警灯2
    strategy_tables[0x0F].actuators[1].actuatorId = (DEV_ACT_ALARM + 1);
    strategy_tables[0x0F].actuators[1].actuatorIO = IO_MODE_GPIO_OUTPUT_1;

    strategy_tables[0x0E].num = 0x12;
    strategy_tables[0x0E].priority = 0x0E;
    strategy_tables[0x0E].usable = 0xFF;
    strategy_tables[0x0E].type = TRIGGER_BIG_EQUAL;
    //烟雾传感器IO1
    strategy_tables[0x0E].sensors[0].sensorId = (DEV_SENSOR_GAS + 1);
    strategy_tables[0x0E].sensors[0].sensorState = 0x0300;
    // 1.灯1
    strategy_tables[0x0E].actuators[0].actuatorId = (DEV_ACT_LIGHT + 1);
    strategy_tables[0x0E].actuators[0].actuatorIO = IO_MODE_GPIO_OUTPUT_1;
    // 2.报警灯2
    strategy_tables[0x0E].actuators[1].actuatorId = (DEV_ACT_ALARM + 1);
    strategy_tables[0x0E].actuators[1].actuatorIO = IO_MODE_GPIO_OUTPUT_1;


    // 0x10~0x1F 回家模式 0x10
		
		
    //0x20~0x2F 布防 、离家模式策略0x20
    strategy_tables[0x2F].num = 0x13;
    strategy_tables[0x2F].priority = 0x2F;
    strategy_tables[0x2F].usable = MODE_LEAVE;
    strategy_tables[0x2F].type = TRIGGER_BIG_EQUAL;
    // 门口人体传感器触发
    strategy_tables[0x2F].sensors[0].sensorId = (DEV_SENSOR_IR + 1);
    strategy_tables[0x2F].sensors[0].sensorState = 1;
    // 1.开窗帘1
    strategy_tables[0x2F].actuators[0].actuatorId = (DEV_ACT_CURTAIN + 1);
    strategy_tables[0x2F].actuators[0].actuatorIO = IO_MODE_CURTAIN_BACK;
    // 2.灯1
    strategy_tables[0x2F].actuators[1].actuatorId = (DEV_ACT_LIGHT + 1);
    strategy_tables[0x2F].actuators[1].actuatorIO = IO_MODE_GPIO_OUTPUT_1;
    // 3.报警灯2
    strategy_tables[0x2F].actuators[2].actuatorId = (DEV_ACT_ALARM + 1);
    strategy_tables[0x2F].actuators[2].actuatorIO = IO_MODE_GPIO_OUTPUT_1;

    strategy_tables[0x2E].num = 0x13;
    strategy_tables[0x2E].priority = 0x2E;
    strategy_tables[0x2E].usable = MODE_LEAVE;
    strategy_tables[0x2E].type = TRIGGER_BIG_EQUAL;
    // 卧室人体传感器触发
    strategy_tables[0x2E].sensors[0].sensorId = (DEV_SENSOR_IR + 2);
    strategy_tables[0x2E].sensors[0].sensorState = 1;
    // 1.开窗帘1
    strategy_tables[0x2E].actuators[0].actuatorId = (DEV_ACT_CURTAIN + 1);
    strategy_tables[0x2E].actuators[0].actuatorIO = IO_MODE_CURTAIN_BACK;
    // 2.灯1
    strategy_tables[0x2E].actuators[1].actuatorId = (DEV_ACT_LIGHT + 1);
    strategy_tables[0x2E].actuators[1].actuatorIO = IO_MODE_GPIO_OUTPUT_1;
    // 3.报警灯2
    strategy_tables[0x2E].actuators[2].actuatorId = (DEV_ACT_ALARM + 1);
    strategy_tables[0x2E].actuators[2].actuatorIO = IO_MODE_GPIO_OUTPUT_1;

    // 0x30~0x3F 休息模式策略 0x30
    // 1. 检测到 门口人体感应 -->报警器响 开灯 开窗帘
    strategy_tables[0x3F].num = 0x13;
    strategy_tables[0x3F].priority = 0x3F;
    strategy_tables[0x3F].usable = MODE_LEAVE;
    strategy_tables[0x3F].type = TRIGGER_BIG_EQUAL;
    // 门口人体传感器触发
    strategy_tables[0x3F].sensors[0].sensorId = (DEV_SENSOR_IR + 1);
    strategy_tables[0x3F].sensors[0].sensorState = 1;
    // 1.开窗帘1
    strategy_tables[0x3F].actuators[0].actuatorId = (DEV_ACT_CURTAIN + 1);
    strategy_tables[0x3F].actuators[0].actuatorIO = IO_MODE_CURTAIN_BACK;
    // 2.灯1
    strategy_tables[0x3F].actuators[1].actuatorId = (DEV_ACT_LIGHT + 1);
    strategy_tables[0x3F].actuators[1].actuatorIO = IO_MODE_GPIO_OUTPUT_1;
    // 3.报警灯2
    strategy_tables[0x3F].actuators[2].actuatorId = (DEV_ACT_ALARM + 1);
    strategy_tables[0x3F].actuators[2].actuatorIO = IO_MODE_GPIO_OUTPUT_1;

    // 2. 检测到 卧室人体感应 -->开灯
    strategy_tables[0x3E].num = 0x11;
    strategy_tables[0x3E].priority = 0x3E;
    strategy_tables[0x3E].usable = MODE_LEAVE;
    strategy_tables[0x3E].type = TRIGGER_BIG_EQUAL;
    // 门口人体传感器触发
    strategy_tables[0x3E].sensors[0].sensorId = (DEV_SENSOR_IR + 2);
    strategy_tables[0x3E].sensors[0].sensorState = 1;
    // 1.开灯1
    strategy_tables[0x3E].actuators[0].actuatorId = (DEV_ACT_LIGHT + 1);
    strategy_tables[0x3E].actuators[0].actuatorIO = IO_MODE_GPIO_OUTPUT_1;
    // 0x40
}
void policy_mode_clean()
{
    uint8_t i;
    for ( i = 0x10; i < MAX_DESTABLE_NUM; i++)
    {
        strategy_tables[i].usable = 0;
    }
}
void policy_mode_usable()
{
    uint8_t i;
    if (MODE)
    {
        MODE = MODE << 4;
        for ( i = 0; i < 0x10; i++)
        {
            if (strategy_tables[MODE + i].priority)
            {
                strategy_tables[MODE + i].usable = MODE + i;
            }
        }

    }
}
void policy_mode_switch()
{
    struct devTable *pdevTbs = NULL;

    // 1. 使能禁用不同的策略
    // 2. 修改控制器优先级
    switch (MODE)
    {
    // 1. 回家模式切换
    case MODE_HOME:
        // 2.启用回家模式
        policy_mode_clean();
        policy_mode_usable();
        Zigbee_ClearPriority();
        // 1.开灯 开门 开窗帘
        pdevTbs = getDevTbsByNetId(DEV_ACT_LOCKER + 1);
        if (pdevTbs)
        {
            Zigebee_setIOBynetId(pdevTbs);
            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
            zigbee_operate(pdevTbs, PRIORITY_DEFAULT, ZIGEBE_NOFORCE, ZIGEBE_NONIMMEDIATE);
        }
        pdevTbs = getDevTbsByNetId(DEV_ACT_LIGHT + 1);
        if (pdevTbs)
        {
            Zigebee_setIOBynetId(pdevTbs);
            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
            zigbee_operate(pdevTbs, PRIORITY_HIGHEST, ZIGEBE_NOFORCE, ZIGEBE_NONIMMEDIATE);
        }
        pdevTbs = getDevTbsByNetId(DEV_ACT_CURTAIN + 1);
        if (pdevTbs)
        {
            Zigebee_setIOBynetId(pdevTbs);
            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_CURTAIN_BACK;
            zigbee_operate(pdevTbs, PRIORITY_DEFAULT, ZIGEBE_NOFORCE, ZIGEBE_NONIMMEDIATE);
        }

        break;
    // 2. 离家布防模式
    case MODE_LEAVE:
        // 1. 关灯、关窗帘
        pdevTbs = getDevTbsByNetId(DEV_ACT_LIGHT + 1);
        if (pdevTbs)
        {
            Zigebee_setIOBynetId(pdevTbs);
            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
            zigbee_operate(pdevTbs, PRIORITY_DEFAULT, ZIGEBE_NOFORCE, ZIGEBE_NONIMMEDIATE);
        }
        pdevTbs = getDevTbsByNetId(DEV_ACT_CURTAIN + 1);
        if (pdevTbs)
        {
            Zigebee_setIOBynetId(pdevTbs);
            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_CURTAIN_FORTH;
            zigbee_operate(pdevTbs, PRIORITY_DEFAULT, ZIGEBE_NOFORCE, ZIGEBE_NONIMMEDIATE);
        }
        pdevTbs = getDevTbsByNetId(DEV_ACT_CURTAIN + 1);
        if (pdevTbs)
        {
            Zigebee_setIOBynetId(pdevTbs);
            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_CURTAIN_BACK;
            zigbee_operate(pdevTbs, PRIORITY_DEFAULT, ZIGEBE_NOFORCE, ZIGEBE_NONIMMEDIATE);
        }
        //

        // 2. 5S 后执行   关门 启用离家、布防模式

        // leave_mode = 1;
        policy_mode_clean();
        policy_mode_usable();
        Zigbee_ClearPriority();
        break;
    // 3. 休息模式切换
    case MODE_RELAX:
        // 启用休息模式
        policy_mode_clean();
        policy_mode_usable();
        Zigbee_ClearPriority();
        break;

    case MODE_DEFAULT:
        // MODE_DEF 自定义模式
        policy_mode_clean();
        policy_mode_usable();
        break;

    }

}
void policy_init()
{
    strategy_init();
    policy_mode_switch();
}

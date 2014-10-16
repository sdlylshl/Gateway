
#include "stm32f10x.h"
#include "Zigbee.h"
#include <stdio.h>
#include "config.h"
// 1.为方便操作MAC为小端存储方式
//#define BIGENDIAN 1
#define DEFAULT_IO 2
//#define OLDPROTOCAL 1
#define Zigbee_DBG USARTx_printf
#define Zigbee_DBG_USARTx USART3
uint8_t Zigbee_buf[ZIGBEE_BUFFSIZE] ;
//Zigebee 接收缓存
struct Zigbee_msgStu Zigbee_RecvBuff[ZIGEBE_RECV_CMD_NUM];
struct Zigbee_msgStu Zigbee_SendBuff[ZIGEBE_SEND_CMD_NUM];
__IO uint16_t Zigbee_read = 0;
__IO uint16_t Zigbee_write = 0;
__IO uint16_t Zigbee_send_add = 0;
__IO uint16_t Zigbee_send_rm = 0;
extern uint8_t Net_send_device(struct devTable *pdevTbs);
//extern void Device_operateFlag(struct devTable *pdevTbs);
extern uint8_t zigbee_operate(struct devTable *pdevTbs, uint8_t priority, uint8_t force, uint8_t immediate);
extern uint8_t zigbee_inquire(struct devTable *pdevTbs, uint8_t force, uint8_t immediate);

void SendCMD(uint8_t data)
{
    while ( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET );
    USART_SendData(USART2, data);
}
void Zigbee_send_add_forward()
{
    if (Zigbee_send_add == (ZIGEBE_SEND_CMD_NUM - 1))
        Zigbee_send_add = 0;
    else
    {
        Zigbee_send_add++;
    }
}
void Zigbee_send_rm_forward()
{
    if (Zigbee_send_rm == (ZIGEBE_SEND_CMD_NUM - 1))
        Zigbee_send_rm = 0;
    else
    {
        Zigbee_send_rm++;
    }
}
void Zigbee_read_backward(uint8_t dec)
{
    while (dec--)
    {
        if (Zigbee_read == 0)
            Zigbee_read = (ZIGBEE_BUFFSIZE - 1);
        else
            Zigbee_read--;
    }
}
// BUG修复 一次只能移动一个字节
void Zigbee_read_forward()
{
    if (Zigbee_read == (ZIGBEE_BUFFSIZE - 1))
        Zigbee_read = 0;
    else
    {
        Zigbee_read++;
    }
}
// 异或和校验
unsigned char calcfcs(unsigned char *pmsg, unsigned char len)
{
    unsigned char result = 0;
    while (len--)
    {
        result ^= *pmsg++;
    }
    return result;
}
struct Zigbee_msgStu *get_ZigbeeSendBuf(void)
{
    /*  uint8_t i = 0;
        while (i < ZIGEBE_SEND_CMD_NUM)
       {
           if (Zigbee_SendBuff[i].usable == 0)
           {
               return &Zigbee_SendBuff[i];
           }
           i++;
       }
       return NULL;*/
    // Zigbee_send_add++;
    Zigbee_send_add_forward();
    return (&Zigbee_SendBuff[Zigbee_send_add]);
}
uint8_t Zigbee_send(struct Zigbee_msgStu *pZmsgSnd)
{
    // 立即发送 则跳过
    if (pZmsgSnd->usable > 1)
    {
        uint8_t i;
        SendCMD(pZmsgSnd->head);
        SendCMD(pZmsgSnd->len);
        SendCMD(pZmsgSnd->cmd[0]);
        SendCMD(pZmsgSnd->cmd[1]);
        for (i = 0; i < pZmsgSnd->len; i++)
        {
            SendCMD(pZmsgSnd->data[i]);
        }
        SendCMD(pZmsgSnd->chk);
        SendCMD(pZmsgSnd->endl);
        pZmsgSnd->usable = 0;
        timer_Zigbee_sendBuff = 0;
    }
    return OK;
}
void Zigbee_send_Timer(uint32_t timeout)
{
    if (timer_Zigbee_sendBuff > timeout)
    {
        //Net_PutChar(Zigbee_send_add);
        //Net_PutChar(Zigbee_send_rm);
        //TODO: 定时执行Zigbee发送缓冲区指令
        if (((ZIGEBE_SEND_CMD_NUM + Zigbee_send_add - Zigbee_send_rm) % ZIGEBE_SEND_CMD_NUM))
        {
            Zigbee_send(&Zigbee_SendBuff[Zigbee_send_rm]);
            Zigbee_send_rm_forward();
            //timer_Zigbee_sendBuff = 0;
        }
    }
}
// TAG: Zigbee 将指令送入发送缓冲区
uint8_t zigbee_push(uint8_t len, uint16_t cmd, uint8_t buf[], uint8_t immediate)
{
    uint8_t i;
    struct Zigbee_msgStu *pZmsgSndbuf;
    pZmsgSndbuf = &Zigbee_SendBuff[Zigbee_send_add];
    if (!immediate)
    {
        Zigbee_send_add_forward();
    }
    if (pZmsgSndbuf == NULL)
    {
        return ERROR_NOSENDBUFF;
    }
    //开始填充数据
    pZmsgSndbuf->usable = 1;
    pZmsgSndbuf->head = ZIGBEE_API_FRAME_HEAD;
    pZmsgSndbuf->len = len;
    //NOTE: 小端模式 低地址放在低字节
    pZmsgSndbuf->cmd[1] = (uint8_t)(cmd >> 8) & 0xFF;
    pZmsgSndbuf->cmd[0] = (uint8_t)cmd & 0xFF;
    for (i = 0; i < len; i++)
        pZmsgSndbuf->data[i] = buf[i];
    //异或和校验
    pZmsgSndbuf->chk = calcfcs(&pZmsgSndbuf->len, len + 3);
    pZmsgSndbuf->endl = 0;
    //填充完成
    pZmsgSndbuf->usable = 2;
    //立即发送
    if (immediate)
    {
        //NOTE: 立即发送 清定时器时钟 最少延迟20ms 让下条指令延时发送
        while (timer_Zigbee_sendBuff < 50);
        Zigbee_send(pZmsgSndbuf);
    }
    return OK;
}
// Zigbee 指令发送zigbee_send_data
int8_t zigbee_send_data(uint8_t len, uint16_t netid, uint8_t buf[], uint8_t immediate)
{
    uint8_t i;
    uint8_t data[ZIGEBEE_DATA_LEN];
    if (len > ZIGEBEE_DATA_LEN )
        return ERROR;
    //低位先发送
    data[0] = (uint8_t) netid & 0xff;
    data[1] = (uint8_t) (netid >> 8) & 0xff;
    for (i = 0; i < len; i++)
        data[2 + i] = buf[i];
    zigbee_push(len + 2, ZIGBEE_SND_DATA, data, immediate);
    return OK;
}
/**
  * @brief  从头获取可用指令列表
  * @param  None
  * @retval struct Zigbee_msgStu *
  * @writer lishoulei
  * @modify
  */
struct Zigbee_msgStu *getZigbeeRecvBuff()
{
    uint32_t i = 0;
    while (i < ZIGEBE_RECV_CMD_NUM)
    {
        if (Zigbee_RecvBuff[i].usable == 0)
        {
            return &Zigbee_RecvBuff[i];
        }
        i++;
    }
    return NULL;
}
void Zigebee_ResetIOBynetId(struct devTable *pdevTbs)
{
    if (pdevTbs)
    {
        if (pdevTbs->devstate)
        {
            //增加传感器检测 网络ID最高位为1
            //IO0 电池电量检测
            //IO1 默认模拟IO采集
            //IO2 默认开关IO采集 /备用开关控制
            //IO3 默认开关控制  /备用开关IO采集
            //IO4 指示灯
            //pdevTbs->priority = PRIORITY_DEFAULT;
            switch (pdevTbs->netId & 0xFF00)
            {
            //模拟采集IO1
            case DEV_SENSOR_SMOKE:

            case DEV_SENSOR_GAS  :
                pdevTbs->ion = IO_D1;
                pdevTbs->operate = 0;
                pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_ANALOG_INPUT;
                break;

            //开关采集IO2
            case DEV_SENSOR_SW   :
            // break;
            case DEV_SENSOR_IR   :
                pdevTbs->ion = IO_D2;
                pdevTbs->operate = 0;
                pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_INPUT;
                break;

            // 开关输出 IO3
            // 灯
            case DEV_ACT_LIGHT   :
            case DEV_ACT_ALARM   :
            case DEV_ACT_LOCKER  :
            case DEV_ACT_JACK    :
                pdevTbs->ion = IO_D3;
                pdevTbs->operate = 1;
                pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                break;
            // 窗帘执行器
            case DEV_ACT_CURTAIN :
                pdevTbs->ion = IO_D3;
                pdevTbs->operate = 1;
                pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_CURTAIN_PAUSE;
                break;

            }
        }
    }
}
void Zigebee_setIOBynetId(struct devTable *pdevTbs)
{
    if (pdevTbs)
    {
        if (pdevTbs->devstate)
        {
            //增加传感器检测 网络ID最高位为1
            //IO0 电池电量检测
            //IO1 默认模拟IO采集
            //IO2 默认开关IO采集 /备用开关控制
            //IO3 默认开关控制  /备用开关IO采集
            //IO4 指示灯
            //pdevTbs->operate = 0;
            //pdevTbs->priority = PRIORITY_DEFAULT;
            switch (pdevTbs->netId & 0xFF00)
            {
            //模拟采集IO1
            case DEV_SENSOR_SMOKE:

            case DEV_SENSOR_GAS  :
                pdevTbs->ion = IO_D1;
                pdevTbs->operate = 0;
                // pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_ANALOG_INPUT;
                break;

            //开关采集IO2
            case DEV_SENSOR_SW   :
            // break;
            case DEV_SENSOR_IR   :
                pdevTbs->ion = IO_D2;
                pdevTbs->operate = 0;
                // pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_INPUT;
                break;

            // 开关输出 IO3
            // 灯

            case DEV_ACT_LIGHT   :
            case DEV_ACT_ALARM   :
            case DEV_ACT_LOCKER  :
            case DEV_ACT_JACK    :
            // break;
            // 窗帘执行器
            case DEV_ACT_CURTAIN :
                pdevTbs->ion = IO_D3;
                // pdevTbs->operate = 1;
                // pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                break;

            }
        }
    }
}
// TAG: Zigbee 指令解析Zigbee_parseInstruction
uint8_t Zigbee_parseInstruction(struct Zigbee_msgStu *pZmsgRcv)
{
    struct devTable *pdevTbs = NULL;
    uint16_t *pcmd;
    uint16_t *pnetId;
    uint8_t  *pion;
    //uint8_t  *piomode;
    uint16_t *pvalue;
    uint8_t  *pstate;//设置成功标志
    uint8_t  *pmac;
    if (pZmsgRcv == NULL)
    {
        //TODO ERROR
        return ERROR_NOSENDBUFF;
    }
    //释放当前指令
    pZmsgRcv->usable = 0;
    //获取命令
    pcmd = (uint16_t *)(pZmsgRcv->cmd);
    // printf("ins: 0x%x\n", *ins);
    switch (*pcmd)
    {
    //指令响应
    case ZIGBEE_CMD_RSP:
        //00 = 成功
        pstate = (uint8_t *)(&pZmsgRcv->data[0]);
        if (*pstate)
        {
            //指令发送失败
            return ERROR_ZIGBEE_SENDERR;
        }
        else
        {
            //指令发送成功
            return ERROR_ZIGBEE_SENDOK;
        }
    //远程设置某个网络设备 IO值
    case ZIGBEE_REMOTE_SET_RECV_DATA:
        pnetId = (uint16_t *)(&pZmsgRcv->data[0]);
        pion  = (uint8_t *)(&pZmsgRcv->data[2]);
        pstate = (uint8_t *)(&pZmsgRcv->data[3]);
        pdevTbs = getDevTbsByNetId(*pnetId);
        if (pdevTbs != NULL)
        {
            pdevTbs->ion = *pion;
            // 不管是否设置成功，都更新设备状态
            // 将全局设备表维护到正确的状态上
            zigbee_inquire(pdevTbs, ZIGEBE_FORCE, ZIGEBE_NONIMMEDIATE);
            //zigbee_remote_req_net_io(pdevTbs->netId, pdevTbs->ion, ZIGEBE_NONIMMEDIATE);
            return OK;
        }
        else
        {
            //无此网络号 更新设备
            zigbee_updateMacByNetId(*pnetId);
        }
        break;
    //远程查询某个网络设备 IO值
    case ZIGBEE_REMOTE_REQ_RECV_DATA:
        pnetId = (uint16_t *)(&pZmsgRcv->data[0]);
        pion  = (uint8_t *)(&pZmsgRcv->data[2]);
        //piomode = (uint8_t *)(&pZmsgRcv->data[3]);
        pvalue = (uint16_t *)(&pZmsgRcv->data[4]);
        pdevTbs = getDevTbsByNetId(*pnetId);
        if (pdevTbs != NULL)
        {
            // 1.传感器检测
            // 2.IO检测
            // 3.状态是否改变
            // 4.如果改变 更新到服务器
            // 更新curSt
            // 转为小端存储
            pdevTbs->ion = *pion;
            //pdevTbs->statetables[pdevTbs->ion].iomode = *piomode;
            pdevTbs->statetables[pdevTbs->ion].curstat = *pvalue;
            pdevTbs->request = pdevTbs->request & (~(1 << pdevTbs->ion));
            //此时服务器有可能还没有更新设备，所以将整个设备信息更新上去
            Net_send_device(pdevTbs);
            // 更新完成后 切换到默认IO
            Zigebee_setIOBynetId(pdevTbs);
            Net_send_device(pdevTbs);
            return OK;
        }
        else
        {
            //无此网络号 发送 通过网络号查询mac
            zigbee_updateMacByNetId(*pnetId);
        }
        break;
    //查询网络设备 通过mac地址查询 通过网络号查询
    //网络设备状态数据接收 CMD= 44 5D
    case ZIGBEE_NET_RECV_DEVID:
        // NOTE: Zigbee MAC地址 由于采用大端传输，所以存储时也延续大端存储方式
        if (pZmsgRcv->len == 0x0C)
        {
            pnetId = (uint16_t *)(&pZmsgRcv->data[1]);
            pmac = &pZmsgRcv->data[3];
        }
        else
        {
            pnetId = (uint16_t *)(&pZmsgRcv->data[0]);
            pmac = &pZmsgRcv->data[2];
        }
        //FE 0B 44 5D 10 0E AACF2802004B1200 01 1B
        // MAC 地址：0x00124B000228CFAA
        //查找设备
        // pdevTbs = getDevTbsByNetId(*pnetId);
        pdevTbs = getDevTbsByMac(pmac);
        if (pdevTbs != NULL)
        {
            //移除同网络号的设备
            struct devTable *pdevTbs1 = getDevTbsByNetId(*pnetId);
            if (pdevTbs1 != pdevTbs)
            {
                pdevTbs->devstate = 0;
                pdevTbs->protocol = 0;
                pdevTbs->netId = 0 ;
            }
            //找到设备 网络号发生变更 则更新设备
            //1.判断网络号是否一致 如果不一致 1.更新此设备 3.通知服务器
            if ( *pnetId != pdevTbs->netId )
            {
                // 更新设备表
                pdevTbs->netId = *pnetId;
                pdevTbs->devstate = 1;
                pdevTbs->protocol = 1;
                //更新当前设备到服务器
                Net_send_device(pdevTbs);
            }
        }
        else
        {
            //添加新设备
            //1.申请空设备
            pdevTbs = getNewDevTbs();
            if (pdevTbs != NULL)
            {
                uint8_t i;
                pdevTbs->devstate = 1;
                pdevTbs->protocol = 1;
                pdevTbs->netId = *pnetId;
                //为方便操作采用大端存储
                for (i = 0; i < 8; i++)
                    pdevTbs->mac[i] = *pmac++;
                //TODO: 根据网络ID设置设备IO
                Zigebee_setIOBynetId(pdevTbs);
                //更新当前设备到服务器
                Net_send_device(pdevTbs);
            }
            else
            {
                return ERROR_NODEV;
            }
        }
        break;
    }
    return ERROR;
}
// TAG: Zigbee 取出指令Zigbee_fetchParseInstruction
//从数据缓冲池中将指令提取出来 放入Zigbee_msgStu[]
uint8_t Zigbee_fetchParseInstruction (void)
{
    uint8_t len_tmp;
    uint8_t i;
    uint32_t chk;
    struct Zigbee_msgStu *pZmsgRcv;
    // SendCMD(Zigbee_write);
    // SendCMD(Zigbee_read);
    //包头检测 若接收到的数据长度小于最小心跳包长度 则不再检测包头
    // DEBUG: Zigbee_fetchParseInstruction
    // Zigbee_DBG(Zigbee_DBG_USARTx, "\r\n Zigbee_fetchInstruction \r\n");
    while ((Zigbee_write + ZIGBEE_BUFFSIZE - Zigbee_read) % ZIGBEE_BUFFSIZE > 2)
    {
        //检测到包头0xFE
        if (Zigbee_buf[Zigbee_read] == ZIGBEE_API_FRAME_HEAD)
        {
            //指向
            Zigbee_read_forward();
            //取得接收数据长度Zigbee_read+1 代表len 位置
            len_tmp = Zigbee_buf[Zigbee_read];
            //当前无效帧
            if (len_tmp > ZIGEBEE_DATA_LEN)
            {
                continue;
            }
            else if ((Zigbee_write + ZIGBEE_BUFFSIZE - Zigbee_read) % ZIGBEE_BUFFSIZE > len_tmp + 3)
            {
                //接收到完整指令   len1+cmd2+chk1 =4字节
                //由于接收到的数据不是连续存储的，所以必须先转存 后校验
                //获取可用指令接收表
                pZmsgRcv = getZigbeeRecvBuff();
                if (pZmsgRcv == NULL)
                {
                    return ERROR_NODEV;
                }
                pZmsgRcv->usable = 1;
                pZmsgRcv->head = ZIGBEE_API_FRAME_HEAD;
                pZmsgRcv->len = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();
                //命令cmd[0]存放低字节  低字节先发送
                pZmsgRcv->cmd[0] = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();
                pZmsgRcv->cmd[1] = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();
                //指向data
                // SendCMD((uint8_t)pZmsgRcv);
                // SendCMD((uint8_t)dat);
                for (i = 0; i < (len_tmp); i++)
                {
                    pZmsgRcv->data[i] = Zigbee_buf[Zigbee_read];
                    Zigbee_read_forward();
                }
                pZmsgRcv->chk = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();
                //自动填充 不属于接收的范畴
                pZmsgRcv->endl = 0x00;
                //异或和校验 len + cmd + data
                chk = calcfcs(&pZmsgRcv->len, len_tmp + 3);
                if (chk == pZmsgRcv->chk)
                {
                    //校验成功
                    pZmsgRcv->usable = 2;
                    Zigbee_parseInstruction(pZmsgRcv);
                    //正确解析
                    return OK;
                }
                else
                {
                    //校验失败 此处可能存在丢包
                    pZmsgRcv->usable = 0;
                    return ERROR;
                }
            }
            else
            {
                //未接收到完整的数据帧 退回
                Zigbee_read_backward(1);
            }
        }
        else
        {
            Zigbee_read_forward();
        }
    }//end while
    //未收到数据
    return ERROR_NODATA;
}
void Zigbee_ClearRequest(uint32_t timeout)
{
    if (timer_Zigbee_ClearRequest > timeout)
    {
        uint8_t i;
        for (i = 0; i < MAX_DEVTABLE_NUM; i++)
        {
            if (devTbs[i].devstate)
            {
                devTbs[i].request = 0;
            }
        }
        timer_Zigbee_ClearRequest = 0;
    }
}

// 定时查询IO状态
//IO0 电池电量检测
//IO1 默认模拟IO采集
//IO2 开关控制1 / 开关IO采集
//IO3 默认开关控制2 / 开关IO采集
//IO4 指示灯
void Zigbee_getActstate_timer(uint32_t timeout)
{
    if (timer_Zigbee_getStatus > timeout)
    {
        uint8_t i;
        for (i = 0; i < MAX_DEVTABLE_NUM; i++)
        {
            if (devTbs[i].devstate)
            {
                switch (devTbs[i].netId & 0xFF00)
                {

                // 开关输出 IO3
                // 灯
                case DEV_ACT_LIGHT   :
                case DEV_ACT_ALARM   :
                case DEV_ACT_LOCKER  :
                case DEV_ACT_JACK    :
                    devTbs[i].ion = IO_D3;
                    // devTbs[i].operate = 1;
                    // devTbs[i].statetables[devTbs[i].ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                    zigbee_inquire(&devTbs[i], ZIGEBE_FORCE, ZIGEBE_NONIMMEDIATE);
                    break;
                // 窗帘执行器
                case DEV_ACT_CURTAIN :
                    devTbs[i].ion = IO_D3;
                    // devTbs[i].operate = 1;
                    // devTbs[i].statetables[devTbs[i].ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                    zigbee_inquire(&devTbs[i], ZIGEBE_FORCE, ZIGEBE_NONIMMEDIATE);
                    devTbs[i].ion = IO_D2;
                    zigbee_inquire(&devTbs[i], ZIGEBE_FORCE, ZIGEBE_NONIMMEDIATE);

                    break;

                }
                /*
                //获取控制器的默认IO状态
                if ((devTbs[i].netId & 0x8000 ) == 0x0000)
                {
                    Zigebee_setIOBynetId(devTbs);
                    zigbee_inquire(&devTbs[i], ZIGEBE_FORCE, ZIGEBE_NONIMMEDIATE);
                    //zigbee_remote_req_net_io(devTbs[i].netId, devTbs[i].ion, ZIGEBE_NONIMMEDIATE);
                }
                */
            }
        }
        timer_Zigbee_getStatus = 0;
    }
}
void Zigbee_getBattery(uint32_t timeout)
{
    if (timer_Zigbee_getBattery > timeout)
    {
        uint8_t io;
        uint8_t i;
        for (i = 0; i < MAX_DEVTABLE_NUM; i++)
        {
            if (devTbs[i].devstate)
            {
                // 切换到默认IO
                io = devTbs[i].ion;
                devTbs[i].ion = IO_D0;
                zigbee_inquire(&devTbs[i], ZIGEBE_FORCE, ZIGEBE_NONIMMEDIATE);
                // 切换到默认IO
                // Zigebee_setIOBynetId(devTbs[i].netId);
                devTbs[i].ion = io;
                //zigbee_remote_req_net_io(devTbs[i].netId, IO_D0, ZIGEBE_NONIMMEDIATE);
            }
        }
        timer_Zigbee_getBattery = 0;
    }
}
// 复位所有设备默认状态
void Zigbee_reset_default(uint32_t timeout)
{
    if (timer_Zigbee_reset_default > timeout)
    {
        uint8_t i;
        for (i = 0; i < MAX_DEVTABLE_NUM; i++)
        {
            if (devTbs[i].devstate)
            {
                // 判断是否是执行器
                if ((devTbs[i].netId & 0x8000) == 0)
                {

                    Zigebee_ResetIOBynetId(&devTbs[i]);
                    zigbee_operate(&devTbs[i], PRIORITY_DEFAULT, ZIGEBE_FORCE, ZIGEBE_IMMEDIATE);

                }
            }
        }
        timer_Zigbee_reset_default = 0;
    }

}
// 定时更新 设备状态
void zigbee_operate_default(uint32_t timeout)
{
    if (timer_Zigbee_operate_default > timeout)
    {
        uint8_t i = 0;
        for (i = 0; i < MAX_DEVTABLE_NUM; i++)
        {
            if (devTbs[i].devstate)
            {
                Zigebee_setIOBynetId(&devTbs[i]);
                zigbee_operate(&devTbs[i], PRIORITY_DEFAULT, ZIGEBE_NOFORCE, ZIGEBE_NONIMMEDIATE);
            }
        }
        timer_Zigbee_operate_default = 0;
    }
}
uint8_t zigbee_inquire(struct devTable *pdevTbs, uint8_t force, uint8_t immediate)
{
    if (force)
    {
        pdevTbs->request = pdevTbs->request | (1 << pdevTbs->ion);
        zigbee_remote_req_net_io(pdevTbs->netId, pdevTbs->ion, immediate);
    }
    else if (pdevTbs->request & (1 << pdevTbs->ion))
    {
        pdevTbs->request = pdevTbs->request | (1 << pdevTbs->ion);
        zigbee_remote_req_net_io(pdevTbs->netId, pdevTbs->ion, immediate);
    }
    return OK;
}
/**
  * @brief  设备执行    循环检测标志位
                若状态标志为1 1.发送NET当前状态 2.发送 Zigbee查询指令  3. 将网路查询标志位清零
                若控制和状态不匹配  发送设置命令
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
    * @writer lishoulei
    *   @modify
  */
//执行策略使用 延迟执行
uint8_t zigbee_operate(struct devTable *pdevTbs, uint8_t priority, uint8_t force, uint8_t immediate)
{
    // Zigebee_setIOBynetId(pdevTbs);
    //TODO: 1.状态操作允许 设置函数，主要功能是是设置允许操作标志
    //  2. 判断Iomode 设置是否有效
    Device_operateFlag(pdevTbs);
    if ( pdevTbs->operate)
    {
        if (priority >= pdevTbs->priority)
        {
            pdevTbs->priority = priority;
            switch (pdevTbs->statetables[pdevTbs->ion].iomode)
            {
            case IO_MODE_NOUSE:
                break;
            case IO_MODE_ANALOG_INPUT:
                break;
            case IO_MODE_GPIO_INPUT:
                break;
            case IO_MODE_GPIO_OUTPUT_0:
                if (force)
                {
                    zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                    pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                    zigbee_remote_set_net_io(pdevTbs->netId, IO_D4, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                    pdevTbs->statetables[IO_D4].curstat = 0;
                }
                else if (pdevTbs->statetables[pdevTbs->ion].curstat)
                {
                    zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, pdevTbs->statetables[pdevTbs->ion].iomode, 0, immediate);
                    pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                    zigbee_remote_set_net_io(pdevTbs->netId, IO_D4, pdevTbs->statetables[pdevTbs->ion].iomode, 0, immediate);
                    pdevTbs->statetables[IO_D4].curstat = 0;
                }
                break;
            case IO_MODE_GPIO_OUTPUT_1:
                if (force)
                {
                    zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                    pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                    zigbee_remote_set_net_io(pdevTbs->netId, IO_D4, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                    pdevTbs->statetables[IO_D4].curstat = 1;

                }
                else if (!pdevTbs->statetables[pdevTbs->ion].curstat)
                {
                    zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, pdevTbs->statetables[pdevTbs->ion].iomode, 0, immediate);
                    pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                    zigbee_remote_set_net_io(pdevTbs->netId, IO_D4, pdevTbs->statetables[pdevTbs->ion].iomode, 0, immediate);
                    pdevTbs->statetables[IO_D4].curstat = 1;
                }
                break;
            case IO_MODE_PULSE_COUNT:

                break;
            // 窗帘
            case IO_MODE_CURTAIN_STOP:
                if ((pdevTbs->netId & 0xFF00) == DEV_ACT_CURTAIN)
                {
                    // 正转IO3 0  反转IO2 0
                    if (force)
                    {
                        pdevTbs->ion  = IO_D2;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                        pdevTbs->ion  = IO_D3;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                    }
                    else
                    {
                        pdevTbs->ion = IO_D2;
                        if (1 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                        }

                        pdevTbs->ion = IO_D3;
                        if (1 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                        }

                    }
                }
                break;
            case IO_MODE_CURTAIN_FORTH:
                if ((pdevTbs->netId & 0xFF00) == DEV_ACT_CURTAIN)
                {
                    // 正转IO3 1  反转IO2 0
                    if (force)
                    {
                        pdevTbs->ion  = IO_D2;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                        pdevTbs->ion  = IO_D3;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                    }
                    else
                    {
                        pdevTbs->ion = IO_D2;
                        if (1 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                        }

                        pdevTbs->ion = IO_D3;
                        if (0 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                        }

                    }
                }
                break;

            case IO_MODE_CURTAIN_BACK:
                if ((pdevTbs->netId & 0xFF00) == DEV_ACT_CURTAIN)
                {
                    // 反转IO2 1 正转IO3 0
                    if (force)
                    {

                        pdevTbs->ion  = IO_D2;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                        pdevTbs->ion  = IO_D3;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                    }
                    else
                    {
                        pdevTbs->ion = IO_D2;
                        if (0 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                        }

                        pdevTbs->ion = IO_D3;
                        if (1 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_0;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_0, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 0;
                        }

                    }
                }
                break;
            case IO_MODE_CURTAIN_PAUSE:
                if ((pdevTbs->netId & 0xFF00) == DEV_ACT_CURTAIN)
                {
                    //   反转IO2 1 正转IO3 1
                    if (force)
                    {
                        pdevTbs->ion  = IO_D3;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                        pdevTbs->ion  = IO_D2;
                        pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                        zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                        pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                    }
                    else
                    {
                        pdevTbs->ion = IO_D2;
                        if (0 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                        }

                        pdevTbs->ion = IO_D3;
                        if (0 == pdevTbs->statetables[pdevTbs->ion].curstat)
                        {
                            pdevTbs->statetables[pdevTbs->ion].iomode = IO_MODE_GPIO_OUTPUT_1;
                            zigbee_remote_set_net_io(pdevTbs->netId, pdevTbs->ion, IO_MODE_GPIO_OUTPUT_1, 0, immediate);
                            pdevTbs->statetables[pdevTbs->ion].curstat = 1;
                        }

                    }
                }
                break;
            }
        }
    }
    else
    {
        //操作禁止
        return ERR_OPERATE;
    }
    return OK;
}
/**
  * @brief  通过网络号远程查询对应IO状态
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval None
  * @writer lishoulei
  * @modify
  */
void zigbee_remote_req_net_io(uint16_t netid, uint8_t IOn, uint8_t immediate )
{
    uint8_t data[4];
    //低位先发送
    data[0] = (uint8_t) netid & 0xff;
    data[1] = (uint8_t) (netid >> 8) & 0xff;
    //读命令
    data[2] = 0;
    data[3] = IOn;
    zigbee_push(4, ZIGBEE_REMOTE_REQ_IO, data, immediate);
}
/**
  * @brief  通过网络号远程查询对应IO状态
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @param  IOmode:
                                    IO_MODE_NOUSE
                                    IO_MODE_ANALOG_INPUT
                                    IO_MODE_GPIO_INPUT
                                    IO_MODE_GPIO_OUTPUT_1
                                    IO_MODE_GPIO_OUTPUT_0
                                    IO_MODE_PULSE_COUNT
  * @param  IOvalue:
  * @retval None
  * @writer lishoulei
  * @modify
  */
uint8_t zigbee_remote_set_net_io(uint16_t netid, uint8_t IOn, uint8_t IOmode, uint8_t IOvalue, uint8_t immediate)
{
    uint8_t data[7];
    //低位先发送
    data[0] = (uint8_t) netid & 0xff;
    data[1] = (uint8_t) (netid >> 8) & 0xff;
    //写命令
    data[2] = 1;
    data[3] = IOn;
    data[4] = IOmode;
    data[5] = (uint8_t) IOvalue & 0xff;
    data[6] = (uint8_t) (IOvalue >> 8) & 0xff;
    zigbee_push(7, ZIGBEE_REMOTE_SET_IO, data, immediate);
    return OK;
}
void zigbee_updateAllDevice(void)
{
    uint8_t data[1];
    //写查询网络内所有设备网络状态 命令
    data[0] = 0x01;
    zigbee_push(1, ZIGBEE_GET_ALL, data, 0);
}
void zigbee_updateMacByNetId(uint16_t netid)
{
    uint8_t data[3];
    //低位先发送
    data[0] = (uint8_t) netid & 0xff;
    data[1] = (uint8_t) (netid >> 8) & 0xff;
    //写命令
    data[2] = 0x02;
    zigbee_push(3, ZIGBEE_GET_MAC_ADDR, data, 0);
}
void zigbee_updateNetIdByMac(uint8_t *mac)
{
    //这里不能传递数组
    uint8_t i;
    uint8_t data[9];
    //写命令
    data[0] = 0x02;
    //写MAC地址 大端传递
    //FE 0B 44 5D 10 0E AACF2802004B1200 01 1B
    // MAC 地址：0x00124B000228CFAA
    for ( i = 1; i < 9; i++)
    {
        //存储格式 MAC: AACF2802004B1200
        data[i] = *mac++;
    }
    zigbee_push(9, ZIGBEE_GET_NETID_ADDR, data, 0);
}

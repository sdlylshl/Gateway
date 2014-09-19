
#include "stm32f10x.h"
#include "Zigbee.h"
#include <stdio.h>

#include "config.h"


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

//extern void zigbee_operate(struct devTable *pdevTbs);
extern uint8_t Net_send_device(struct devTable *pdevTbs, uint8_t CMD, uint8_t control);
void SendCMD(uint8_t data)
{

    while ( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET );
    USART_SendData(USART2, data);

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
unsigned char calcfcs(unsigned char *pmsg, unsigned char len)
{
    unsigned char result = 0;
    while (len--)
    {
        result ^= *pmsg++;
    }
    return result;
}
uint8_t Zigbee_send(struct Zigbee_msgStu *pZmsgS)
{
    // 立即发送 则跳过
    if (pZmsgS->usable == 3)
        Zigbee_send_rm_forward();
    else if (pZmsgS->usable > 1)
    {
        uint8_t i;
        SendCMD(pZmsgS->head);
        SendCMD(pZmsgS->len);
        SendCMD(pZmsgS->cmd[0]);
        SendCMD(pZmsgS->cmd[1]);

        for (i = 0; i < pZmsgS->len; i++)
        {
            SendCMD(pZmsgS->data[i]);
        }
        SendCMD(pZmsgS->chk);
        SendCMD(pZmsgS->endl);
        Zigbee_send_rm_forward();
        pZmsgS->usable = 0;
    }
    return OK;
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
void zigbee_operate(struct devTable *pdevTbs)
{

    uint8_t j = 0;
    uint8_t IOmod;
    if (pdevTbs == NULL)
    {
        return;
    }

    //判断当前设备是否可用
    if (pdevTbs->devstate)
    {

        //状态查询检测
        if (pdevTbs->ActSt & 0xC000)
        {
            //00 x x
            //01 发送NET
            //10 发送指令 发送net
            //11 发送指令 发送net
            if (pdevTbs->ActSt & 0x8000)
            {

                //判断要查询的IO
                if (pdevTbs->ActSt & 0x1F)
                {
                    for (j = 0; j < 5; j++)
                    {
                        if ((pdevTbs->ActSt >> j) & 0x01)
                        {
                            zigbee_remote_req_net_io(pdevTbs->netId, j );
                        }
                    }
                }
                else
                {
                    //执行默认IO查询
                    zigbee_remote_req_net_io(pdevTbs->netId, IO_D2 );

                }
            }

            //发送当前状态给APP
            Net_send_device(pdevTbs, 11, DEV_MAC | DEV_CURST);
            //发送完成清状态标志
            pdevTbs->ActSt = pdevTbs->ActSt & 0x3FFF;
        }//end (pdevTbs->ActSt & 0x8000)

        //控制执行
        //增加传感器检测 网络ID最高位为1
        if (!pdevTbs->netId & 0x8000)
        {

            if ((pdevTbs->ActSt & 0x100) ^ (pdevTbs->curSt & 0x100) )
            {
                if (pdevTbs->ActSt & 0x100)
                {
                    IOmod = IO_MODE_GPIO_OUTPUT_1;
                }
                else
                {
                    IOmod = IO_MODE_GPIO_OUTPUT_0;
                }

                //判断要设置的IO
                if (pdevTbs->ActSt & 0x1F)
                {
                    for (j = 0; j < 5; j++)
                    {
                        if ((pdevTbs->ActSt >> j) & 0x01)
                        {
                            zigbee_remote_set_net_io(pdevTbs->netId, j, IOmod, 0, 0);
                            pdevTbs->curSt = pdevTbs->ActSt & 0x100;
                        }
                    }
                }
                else
                {
                    //为0执行默认IO查询
                    zigbee_remote_set_net_io(pdevTbs->netId, IO_D2, IOmod, 0 , 0);
                    pdevTbs->curSt = pdevTbs->ActSt & 0x100;

                }
            }
        }
    }//end devstate
}


void zigbee_operate_ALL(void)
{
    uint8_t i = 0;
    Zigbee_DBG(Zigbee_DBG_USARTx, "\r\n zigbee_operate \r\n");
    while (i < MAX_DEVTABLE_NUM) //MAX_DEVTABLE_NUM defined in net.h
    {
        zigbee_operate(&devTbs[i]);
        i++;
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


/**
  * @brief  从头获取可用指令列表
  * @param  None
  * @retval struct Zigbee_msgStu *
  * @writer lishoulei
  * @modify
  */
struct Zigbee_msgStu *getRecvBuff()
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

//从数据缓冲池中将指令提取出来 放入Zigbee_msgStu[]
uint8_t Zigbee_fetchParseInstruction (void)
{
    uint8_t len_tmp;
    uint8_t i;
    uint32_t chk;
    struct Zigbee_msgStu *pZmsgS;
    // SendCMD(Zigbee_write);
    // SendCMD(Zigbee_read);
    //包头检测 若接收到的数据长度小于最小心跳包长度 则不再检测包头
    Zigbee_DBG(Zigbee_DBG_USARTx, "\r\n Zigbee_fetchInstruction \r\n");
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
            else if ((Zigbee_write + ZIGBEE_BUFFSIZE - Zigbee_read) % ZIGBEE_BUFFSIZE >= len_tmp + 4)
            {
                //接收到完整指令   len1+cmd2+chk1 =4字节

                //由于接收到的数据不是连续存储的，所以必须先转存 后校验
                //获取可用指令接收表
                pZmsgS = getRecvBuff();
                if (pZmsgS == NULL)
                {
                    return ERROR_NODEV;
                }
                pZmsgS->usable = 1;

                pZmsgS->head = ZIGBEE_API_FRAME_HEAD;
                pZmsgS->len = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();
                //命令cmd[0]存放低字节  低字节先发送
                pZmsgS->cmd[0] = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();
                pZmsgS->cmd[1] = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();

                //指向data
                // SendCMD((uint8_t)pZmsgS);
                // SendCMD((uint8_t)dat);
                for (i = 0; i < (len_tmp); i++)
                {
                    pZmsgS->data[i] = Zigbee_buf[Zigbee_read];
                    Zigbee_read_forward();
                }

                pZmsgS->chk = Zigbee_buf[Zigbee_read];
                Zigbee_read_forward();
                //自动填充 不属于接收的范畴
                pZmsgS->endl = 0x00;

                //异或和校验 len + cmd + data
                chk = calcfcs(&pZmsgS->len, len_tmp + 3);

                if (chk == pZmsgS->chk)
                {
                    //校验成功
                    pZmsgS->usable = 2;

                    Zigbee_parseInstruction(pZmsgS);
                    //正确解析
                    return OK;
                }
                else
                {
                    //校验失败 此处可能存在丢包
                    pZmsgS->usable = 0;

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

uint8_t Zigbee_parseInstruction(struct Zigbee_msgStu *pZmsgS)
{
    struct devTable *pdevTbs = NULL;
    uint16_t *pcmd;
    uint16_t *pnetId;
    uint8_t *mac;
    if (pZmsgS == NULL)
    {
        //TODO ERROR
        return ERROR_NOSENDBUFF;
    }

    //释放当前指令
    pZmsgS->usable = 0;
    //获取命令
    pcmd = (uint16_t *)(pZmsgS->cmd);

    // printf("ins: 0x%x\n", *ins);
    switch (*pcmd)
    {

    //指令响应
    case ZIGBEE_CMD_RSP:
        //00 = 成功
        if (pZmsgS->data[0])
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

        if (pZmsgS->data[3])
        {
            //设置成功
            pnetId = (uint16_t *)(&pZmsgS->data[0]);
            pdevTbs = getDevTbsByNetId(*pnetId);
            if (pdevTbs != NULL)
            {
                // 此处有BUG 因为不知道反馈的是否和设置一致
                //更新设备表的状态位
                //pdevTbs->curSt = pdevTbs->ActSt & 0x0100;
                //更新标志 通过NET发送到服务器
                //pdevTbs->ActSt = pdevTbs->ActSt | 0x4000;

                //NET立即发送更新设备状态 MAC ActSt curSt
                //Net_send_device(pdevTbs, DEVTAB_UPDATE, 0x38);
                //获取状态
                zigbee_remote_req_net_io(*pnetId, pZmsgS->data[2] );

            }
            else
            {
                //无此网络号 更新设备
                zigbee_updateMacByNetId(*pnetId);
            }
        }
        else
        {
            //设置失败

        }
        break;
    //远程查询某个网络设备 IO值
    case ZIGBEE_REMOTE_REQ_RECV_DATA:

        pnetId = (uint16_t *)(&pZmsgS->data[0]);
        pdevTbs = getDevTbsByNetId(*pnetId);
        if (pdevTbs != NULL)
        {
            //更新curSt
            pdevTbs->curSt = (uint16_t)(pZmsgS->data[4] << 8) | pZmsgS->data[5];
            //更新 zigbee IOn
            pdevTbs->ActSt = (pdevTbs->ActSt & 0xFFE0) |   (1 << pZmsgS->data[2]);
            //更新标志 通过NET发送到服务器
            //pdevTbs->ActSt = pdevTbs->ActSt | 0x4000;
            //NET立即发送更新设备状态 MAC ActSt curSt 0x38
            Net_send_device(pdevTbs, DEVTAB_UPDATE, 0xFF);

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
        if (pZmsgS->len == 0x0C)
        {
            pnetId = (uint16_t *)(&pZmsgS->data[1]);
            mac = &pZmsgS->data[3];
        }
        else
        {
            pnetId = (uint16_t *)(&pZmsgS->data[0]);
            mac = &pZmsgS->data[2];
        }
        //FE 0B 44 5D 10 0E AACF2802004B1200 01 1B
        // MAC 地址：0x00124B000228CFAA
        //查找设备
        // pdevTbs = getDevTbsByNetId(*pnetId);
        pdevTbs = getDevTbsByMac(mac);
        //移除同网络号的设备

        if (pdevTbs != NULL)
        {
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
                Net_send_device(pdevTbs, DEVTAB_UPDATE, 0xFF);

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
                pdevTbs->netId = *pnetId;
                pdevTbs->devstate = 1;
                pdevTbs->protocol = 1;
                for (i = 0; i < 8; i++)
                    pdevTbs->mac[i] = *mac++;
                //更新当前设备到服务器
                Net_send_device(pdevTbs, DEVTAB_UPDATE, 0xFF);
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

//
uint8_t zigbee_push(uint8_t len, uint16_t cmd, uint8_t buf[], uint8_t immediate)
{

    uint8_t i;
    struct Zigbee_msgStu *pZmsgSbuf;

    Net_PutChar(0xFF);
    Net_PutChar(timer_Zigbee_sendBuff);
    pZmsgSbuf = &Zigbee_SendBuff[Zigbee_send_add];
    Zigbee_send_add_forward();

    if (pZmsgSbuf == NULL)
    {
        return ERROR_NOSENDBUFF;
    }
    //开始填充数据
    pZmsgSbuf->usable = 1;
    pZmsgSbuf->head = ZIGBEE_API_FRAME_HEAD;
    pZmsgSbuf->len = len;

    //NOTE: 小端模式 低地址放在低字节
    pZmsgSbuf->cmd[1] = (uint8_t)(cmd >> 8) & 0xFF;
    pZmsgSbuf->cmd[0] = (uint8_t)cmd & 0xFF;
    for (i = 0; i < len; i++)
        pZmsgSbuf->data[i] = buf[i];
    //异或和校验
    pZmsgSbuf->chk = calcfcs(&pZmsgSbuf->len, len + 3);
    pZmsgSbuf->endl = 0;
    //填充完成
    pZmsgSbuf->usable = 2;
    //立即发送
    if (immediate)
    {
        pZmsgSbuf->usable = 3;
        //NOTE: 立即发送 清定时器时钟 最少延迟20ms 让下条指令延时发送
        while (timer_Zigbee_sendBuff < 9);
        timer_Zigbee_sendBuff = 0;
        Zigbee_send(pZmsgSbuf);
    }

    return OK;

}


/**
  * @brief  发送数据，最大25字符
  * @param  NETID:
  * @param  immediate: 0:放入发送池 1:立即发送
  * @retval 发送成功返回 OK
  * @writer lishoulei
  * @modify
  */
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
  * @brief  发送数据，并等待响应，
  * @param  timeout:  超时时间
  * @param
  * @retval 发送成功返回 OK
  * @writer lishoulei
  * @modify
  */
uint8_t Zigebee_sendData_Ans(uint8_t len, uint16_t netid, uint8_t buf[], uint8_t timeoutms)
{
    uint8_t erflag;
    zigbee_send_data(len, netid, buf, 0);
    //
    erflag = ERROR_TIMEOUT;
    //超时响应
    time_out = 0;
    while (time_out > timeoutms)
    {
        erflag = Zigbee_fetchParseInstruction();
        if (erflag != ERROR_NODATA )break;
    }
    return erflag;
}

/**
  * @brief  通过网络号远程查询对应IO状态
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval None
    * @writer lishoulei
    *   @modify
  */
void zigbee_remote_req_net_io(uint16_t netid, uint8_t IOn )
{
    uint8_t data[4];

    //低位先发送
    data[0] = (uint8_t) netid & 0xff;
    data[1] = (uint8_t) (netid >> 8) & 0xff;
    //读命令
    data[2] = 0;
    data[3] = IOn;

    zigbee_push(4, ZIGBEE_REMOTE_REQ_IO, data, 0);

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
    *   @modify
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




#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net.h"
#include "config.h"

#define BIGENDIAN 1
//采用老通信协议V8版本
//#define OLDPROTOCAL 1

//接收缓冲器 512
uint8_t NET_buf[NET_BUFFSIZE];
//读写指针
__IO uint16_t NET_write ;
__IO uint16_t NET_read ;

//最大 指令表 发送到服务器指令
struct msgStu netSendDataCMDS[SEND_CMDS_NUM];
//最大 指令表 接收到服务器指令
struct msgStu netRecvDataCMDS[RECV_CMDS_NUM];
//全局设备表


//extern uint32_t Net_cmd[20];
//uint8_t data[200];
//uint32_t cmdRcv = 0;
//uint32_t cmdDeal = -1;

//发送数据包随机号 ：指令包为奇数 应答包为用偶数，  0 表示心跳
uint16_t  msgSN = 1;

extern void zigbee_operate(struct devTable *pdevTbs,uint8_t force, uint8_t immediate);

//extern uint8_t Net_send_device(struct devTable *pdevTbs);
void Net_PutChar(uint8_t ch)
{
    USART_SendData(USART1, (uint8_t) ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void uart_print( uint8_t len, uint8_t *data)
{
    uint32_t i = 1;
    while (i < len)
    {
        Net_PutChar(data[i++]);
    }
    data[0] = 2;

}

//完成指令的未相应重发
void Net_send_Timer(void)
{
    uint8_t i;

    for (i = 0; i < SEND_CMDS_NUM; i++)
    {
        if (netSendDataCMDS[i].usable)
        {

            netSendDataCMDS[i].usable++;

            // 重发条件
            switch (netSendDataCMDS[i].usable)
            {
            case 10:
            case 20:
            case 30:
            case 40:
            case 50:
                Net_send_data(&netSendDataCMDS[i]);
                break;
            }
            // 死指令 丢弃
            if ( netSendDataCMDS[i].usable > 50)
            {

                Net_send_data(&netSendDataCMDS[i]);
                netSendDataCMDS[i].usable = 0;
            }

        }

    }

}


/**
  * @brief  获取可用的发送缓冲
  * @retval 返回发送结构体指针
  * @writer lishoulei
  * @modify
  */
struct msgStu *get_NetSendBuf(void)
{
    uint8_t i = 0;
    while (i < SEND_CMDS_NUM)
    {
        if (netSendDataCMDS[i].usable == 0)
        {
            return &netSendDataCMDS[i];
        }

        i++;
    }
    return NULL;
}


/**
  * @brief  发送数据，最大25字符
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
  * @writer lishoulei
  *   @modify
  */
uint8_t Net_send_data(struct msgStu *pNmsgS)
{
    // 填充完成
    if (pNmsgS->usable > 1)
    {
        uint8_t i;
        Net_PutChar(pNmsgS->head);
        Net_PutChar(pNmsgS->len);
        Net_PutChar(pNmsgS->sn[0]);
        Net_PutChar(pNmsgS->sn[1]);

        for (i = 0; i < pNmsgS->len; i++)
        {
            Net_PutChar(pNmsgS->data[i]);
        }
        Net_PutChar(pNmsgS->crc[0]);
        Net_PutChar(pNmsgS->crc[1]);
        Net_PutChar(pNmsgS->crc[2]);
        Net_PutChar(pNmsgS->crc[3]);
        Net_PutChar(pNmsgS->endl);
    }
    pNmsgS->usable++;
    return OK;
}

/**
  * @brief  获取msgSN
  * @retval 返回msgSN
  * @writer lishoulei
  * @modify
  */
uint16_t get_NetSendSN()
{

    if (msgSN % 2)
    {
        // 奇数
        msgSN += 2;
    }
    else
    {
        msgSN++;
    }
    return msgSN;
}

/**
  * @brief  发送数据，最大25字符
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
  * @writer lishoulei
  *   @modify
  */
//TAG: NET 发送数据Net_push_data
int8_t Net_push_data(uint8_t len, uint8_t data[])
{
    uint8_t i;
    uint32_t crc = 0;
    uint8_t *psndat;
    struct msgStu *pNmsgS;
    if (len > CMD_DATA_LEN)
    {
        return ERROR_DATAFLOW;
    }
    //从缓冲区中获取获取
    pNmsgS = get_NetSendBuf();
    if (NULL == pNmsgS)
    {
        return ERROR_NOSENDBUFF;
    }

    pNmsgS->usable = 1; //正在填充数据
    pNmsgS->head = NET_CMD_HEAD;
    pNmsgS->len = len;
    //获取新的发送Sn
    get_NetSendSN();

#if BIGENDIAN
    pNmsgS->sn[0] = (uint8_t)((msgSN >> 8) & 0xff);
    pNmsgS->sn[1] = (uint8_t) msgSN & 0xff;
#else
    pNmsgS->sn[1] = (uint8_t)((msgSN >> 8) & 0xff);
    pNmsgS->sn[0] = (uint8_t) msgSN & 0xff;
#endif
    for (i = 0; i < len; i++)
    {
        pNmsgS->data[i] = data[i];
    }
    //TODO
    //生成CRC校验码
    psndat = &pNmsgS->len;
    CRC_ResetDR();
    for (i = 0; i < len + 3; i++)
    {
        CRC_CalcCRC((uint32_t)(*psndat++));
    }
    crc = CRC_GetCRC();
#if BIGENDIAN
    pNmsgS->crc[0] = (uint8_t)((crc >> 24) & 0xff);
    pNmsgS->crc[1] = (uint8_t)((crc >> 16) & 0xff);
    pNmsgS->crc[2] = (uint8_t)((crc >> 8) & 0xff);
    pNmsgS->crc[3] = (uint8_t)((crc >> 0) & 0xff);
#else
    pNmsgS->crc[3] = (uint8_t)((crc >> 24) & 0xff);
    pNmsgS->crc[2] = (uint8_t)((crc >> 16) & 0xff);
    pNmsgS->crc[1] = (uint8_t)((crc >> 8) & 0xff);
    pNmsgS->crc[0] = (uint8_t)((crc >> 0) & 0xff);
#endif
    pNmsgS->endl = NET_CMD_END;
    pNmsgS->usable = 2;//填充完成

    //数据发送
    Net_send_data(pNmsgS);
    return OK;

}

/**
  * @brief  发送设备表信息
  * @param  CMD:
  * @param  control:
  * @retval 发送成功返回 OK
  * @writer lishoulei
  * @modify
  */
uint8_t Net_send_device(struct devTable *pdevTbs)
{
    uint8_t len = 0;
    uint8_t data[CMD_DATA_LEN];
    // 1.指令
    data[len++] = NET_CMD_DEV_UPDATE;
    // 2.设备状态
    data[len++] = pdevTbs->devstate;
    // 3.接口类型
    data[len++] = pdevTbs->protocol;
    // 4.网络号
#if BIGENDIAN
    data[len++] = (uint8_t)((pdevTbs->netId >> 8) & 0xff);
    data[len++] = (uint8_t)((pdevTbs->netId >> 0) & 0xff);
#else
    data[len++] = (uint8_t)((pdevTbs->netId >> 0) & 0xff);
    data[len++] = (uint8_t)((pdevTbs->netId >> 8) & 0xff);
#endif
    // 5.MAC地址
#if BIGENDIAN
    data[len++] = pdevTbs->mac[0];
    data[len++] = pdevTbs->mac[1];
    data[len++] = pdevTbs->mac[2];
    data[len++] = pdevTbs->mac[3];
    data[len++] = pdevTbs->mac[4];
    data[len++] = pdevTbs->mac[5];
    data[len++] = pdevTbs->mac[6];
    data[len++] = pdevTbs->mac[7];
#else
    data[len++] = pdevTbs->mac[7];
    data[len++] = pdevTbs->mac[6];
    data[len++] = pdevTbs->mac[5];
    data[len++] = pdevTbs->mac[4];
    data[len++] = pdevTbs->mac[3];
    data[len++] = pdevTbs->mac[2];
    data[len++] = pdevTbs->mac[1];
    data[len++] = pdevTbs->mac[0];
#endif
    // 6.IOn
    data[len++] = pdevTbs->ion;
    // 7.IOmode
    data[len++] = pdevTbs->statetables[pdevTbs->ion].iomode;
    // 8.当前状态
#if BIGENDIAN
    data[len++] = (uint8_t)((pdevTbs->statetables[pdevTbs->ion].curstat >> 8) & 0xff);
    data[len++] = (uint8_t)((pdevTbs->statetables[pdevTbs->ion].curstat >> 0) & 0xff);
#else
    data[len++] = (uint8_t)((pdevTbs->statetables[pdevTbs->ion].curstat >> 0) & 0xff);
    data[len++] = (uint8_t)((pdevTbs->statetables[pdevTbs->ion].curstat >> 8) & 0xff);
#endif
    // 9.设备名称
#if BIGENDIAN
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[0];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[1];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[2];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[3];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[4];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[5];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[6];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[7];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[8];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[9];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[10];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[11];
#else
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[11];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[10];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[9];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[8];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[7];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[6];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[5];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[4];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[3];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[2];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[1];
    data[len++] = pdevTbs->statetables[pdevTbs->ion].name[0];
#endif

    Net_push_data(len, data);
    return OK;
}


void Net_Ans(struct msgStu *pansmsgS)
{
    uint16_t sn;
    uint32_t crc = 0;
#if BIGENDIAN
    sn = (uint16_t)(((pansmsgS->sn[0] << 8 | pansmsgS->sn[1])) );
#else
    sn = (uint16_t)(((pansmsgS->sn[1] << 8 | pansmsgS->sn[0])) );
#endif
    //TOTE: 无效判断
    if (0 == sn)
    {
        sn = 2;
    }
    Net_PutChar(pansmsgS->head);
    Net_PutChar(0);
    Net_PutChar((uint8_t)(sn >> 8) & 0xff);
    Net_PutChar((uint8_t)sn & 0xff);
    //生成CRC校验码

    CRC_ResetDR();

    CRC_CalcCRC((uint32_t)0);
    CRC_CalcCRC((uint32_t)((sn >> 8) & 0xff));
    CRC_CalcCRC((uint32_t)(sn & 0xff));
    crc = CRC_GetCRC();
#if BIGENDIAN
    pansmsgS->crc[0] = (uint8_t)((crc >> 24) & 0xff);
    pansmsgS->crc[1] = (uint8_t)((crc >> 16) & 0xff);
    pansmsgS->crc[2] = (uint8_t)((crc >> 8) & 0xff);
    pansmsgS->crc[3] = (uint8_t)((crc >> 0) & 0xff);
#else
    pansmsgS->crc[3] = (uint8_t)((crc >> 24) & 0xff);
    pansmsgS->crc[2] = (uint8_t)((crc >> 16) & 0xff);
    pansmsgS->crc[1] = (uint8_t)((crc >> 8) & 0xff);
    pansmsgS->crc[0] = (uint8_t)((crc >> 0) & 0xff);
#endif
    Net_PutChar(pansmsgS->endl);

}


/**
  * @brief  从头获取可用指令列表
  * @param  None
  * @retval struct msgStu *
  * @writer liuzhishan
  *   @modify
  */
struct msgStu *get_NetRecvDataCMDS()
{
    uint32_t i = 0;
    while (i < RECV_CMDS_NUM)
    {
        if (netRecvDataCMDS[i].usable == 0)
        {
            return &netRecvDataCMDS[i];
        }
        i++;
    }
    return NULL;

}
//DEBUG
void print_CMDS()
{
    uint16_t i = 0;
    uint16_t j = 0;
    //uint8_t *pcmd;
    SendCMD(0x5A);
    SendCMD(NET_write);
    SendCMD(NET_read);

    while (i < RECV_CMDS_NUM)
    {

        if (netRecvDataCMDS[i].usable > 1)
        {
            SendCMD(netRecvDataCMDS[i].usable);
            SendCMD(netRecvDataCMDS[i].head);
            SendCMD(netRecvDataCMDS[i].sn[0]);
            SendCMD(netRecvDataCMDS[i].sn[1]);
            SendCMD(netRecvDataCMDS[i].len);

            for (j = 0; j < netRecvDataCMDS[i].len; j++)
            {
                SendCMD(netRecvDataCMDS[i].data[i]);
            }
            SendCMD(netRecvDataCMDS[i].crc[0]);
            SendCMD(netRecvDataCMDS[i].crc[1]);
            SendCMD(netRecvDataCMDS[i].crc[2]);
            SendCMD(netRecvDataCMDS[i].crc[3]);
            SendCMD(netRecvDataCMDS[i].endl);
        }
        i++;
    }

}


void NET_read_backward(uint8_t dec)
{
    while (dec--)
    {
        if (NET_read == 0)
            NET_read = (NET_BUFFSIZE - 1);
        else
            NET_read--;
    }
}


void NET_read_forward()
{


    if (NET_read == (NET_BUFFSIZE - 1))
        NET_read = 0;
    else
    {
        NET_read++;
    }

}
//将接收缓冲器中数据解析成单条指令 存放到cmdS[MAX_CMDS]
void NET_fetchParseInstruction()
{
    uint8_t len_tmp;
    uint8_t i;
    // SendCMD(NET_write);
    // SendCMD(NET_read);
    //包头检测 若接收到的数据长度小于最小心跳包长度 则不再检测包头
    // DEBUG: NET_fetchParseInstruction
    // DEBUG(USARTn, "\r\n net parseRcvBufToLst2 \r\n");
    while (((NET_BUFFSIZE + NET_write - NET_read) % NET_BUFFSIZE) > 2)
    {


        //检测到包头
        if (NET_buf[NET_read] == NET_CMD_HEAD)
        {
            NET_read_forward();
            //取得接收数据长度
            len_tmp = NET_buf[NET_read];
            //当前无效帧
            if (len_tmp < CMD_DATA_LEN)
            {
							if (((NET_BUFFSIZE + NET_write - NET_read) % NET_BUFFSIZE) >= len_tmp + 7)
								{
                //接收到完整指令 NET_read+3 代表len 位置  len1+sn2+crc4 =7字节 endl 可忽略

                struct msgStu *pNmsgR;
                uint8_t *psndat;
                uint32_t crc;
                //获取可用指令表
                pNmsgR = get_NetRecvDataCMDS();
                if (pNmsgR == NULL)
                {
                    return;
                }
                pNmsgR->usable = 1;

                pNmsgR->head = NET_CMD_HEAD;
                pNmsgR->len = NET_buf[NET_read ];
                NET_read_forward();
                pNmsgR->sn[0] = NET_buf[NET_read];
                NET_read_forward();
                pNmsgR->sn[1] = NET_buf[NET_read ];
                NET_read_forward();

                //指向data
                // SendCMD((uint8_t)pNmsgR);
                // SendCMD((uint8_t)dat);
                for (i = 0; i < (len_tmp); i++)
                {
                    pNmsgR->data[i] = NET_buf[NET_read];
                    NET_read_forward();
                }

                pNmsgR->crc[0] = NET_buf[NET_read];
                NET_read_forward();
                pNmsgR->crc[1] = NET_buf[NET_read ];
                NET_read_forward();
                pNmsgR->crc[2] = NET_buf[NET_read ];
                NET_read_forward();
                pNmsgR->crc[3] = NET_buf[NET_read ];
                NET_read_forward();
                //
                pNmsgR->endl = NET_CMD_END;
                NET_read_forward();

                //CRC校验
                psndat = &pNmsgR->len;
                CRC_ResetDR();
                for (i = 0; i < len_tmp + 3; i++)
                {
                    CRC_CalcCRC((uint32_t)(*psndat++));
                }

                crc = (pNmsgR->crc[0] << 24) | (pNmsgR->crc[1] << 16) | (pNmsgR->crc[2] << 8) | (pNmsgR->crc[3]);

                if (crc == CRC_GetCRC())
                {
                    pNmsgR->usable = 2;
                    NET_parseInstruction(pNmsgR);

                }
                else
                {
                    //校验失败
                    pNmsgR->usable = 0;

                }

            }
            else
            {
                //未接收到完整的数据帧 退回
                NET_read_backward(1);
								break;
            }

					}
				}else{
				NET_read_forward();
				}


    }//end while


}


//应答包解析
void Ans_parse(struct msgStu *pNmsgR)
{

    uint16_t i = 0;
    //修改对应重发列表的 usable
    for (i = 0; i < SEND_CMDS_NUM; i++)
    {
        if ( (netSendDataCMDS[i].sn[0] == pNmsgR->sn[0]) && (netSendDataCMDS[i].sn[1] == pNmsgR->sn[1]))
        {
            //如果出现两个sn相同的怎么办？
            netSendDataCMDS[i].usable = 0;
            break;
        }
    }

}


//NOTE: NET_parseData指令包解析
uint8_t NET_parseData(struct msgStu *pNmsgR)
{
    uint8_t i = 0;
    uint8_t instr;
    uint8_t *pmac;
    uint8_t *pion;
    uint8_t *piomode;
    uint8_t *pname;
    struct devTable *pdevTbs = NULL;
    //uint8_t ctr;
    // uint8_t *msgStu = (uint8_t *) & (pNmsgR->data[0]);
    //释放当前指令
    pNmsgR->usable = 0;
    // 获取指令类型
    instr = pNmsgR->data[0];
    switch (instr)
    {
    //查询所有设备
    case NET_CMD_REQ_ALL_DEV:

        for (i = 0; i < MAX_DEVTABLE_NUM; i++)
        {
            if (devTbs[i].devstate)
            {
                Net_send_device(&devTbs[i]);
            }
        }

        zigbee_updateAllDevice();

        break;

    // 查询单个设备信息
    case NET_CMD_REQ_ONE_DEV:
        // (02 [MAC] [Ion])
        //通过MAC获取设备 大端存储

        //查找MAC所在的字节
        pmac = &pNmsgR->data[1];
        pion = &pNmsgR->data[9];

        pdevTbs = getDevTbsByMac(pmac);

        if (pdevTbs != NULL)
        {
            pdevTbs->ion = *pion;
            Net_send_device(pdevTbs);
            // 发送Zigbee查询
            zigbee_remote_req_net_io(pdevTbs->netId, pdevTbs->ion , ZIGEBE_IMMEDIATE);
        }
        else
        {
            //TODO: 设备不存在，添加设备移除操作
        }
        break;
    // 设置设备名称
    case NET_CMD_SET_DEV_NAME:
        // ([04] [MAC] [Ion] [name])
        pmac = &pNmsgR->data[1];
        pion = &pNmsgR->data[9];
        pname = &pNmsgR->data[10];

        pdevTbs = getDevTbsByMac(pmac);

        if (pdevTbs != NULL)
        {
            pdevTbs->ion = *pion;
            for (i = 0; i < DEV_NAME_SIZE; i++)
                pdevTbs->statetables[pdevTbs->ion].name[i] = *pname++;
        }
        else
        {
            //TODO: 设备不存在，添加设备移除操作
        }
        break;

    // 控制设备开关
    case NET_CMD_SET_ON_OFF:
        // ([08] [MAC] [Ion] [iomode])
        pmac = &pNmsgR->data[1];
        pion = &pNmsgR->data[9];
        //poperate = &pNmsgR->data[10];
        piomode = &pNmsgR->data[10];

        pdevTbs = getDevTbsByMac(pmac);

        if (pdevTbs != NULL)
        {
            pdevTbs->ion = *pion;
            pdevTbs->statetables[pdevTbs->ion].iomode = *piomode;
			pdevTbs->priority = PRIORITY_HIGHEST; //将优先级设置为最高，禁用策略控制
					// 立即强制执行
            zigbee_operate(pdevTbs, PRIORITY_HIGHEST,ZIGEBE_FORCE,ZIGEBE_IMMEDIATE);

        }
        else
        {
            //TODO: 设备不存在，添加设备移除操作
        }
        break;
    // 策略操作
    case NET_CMD_PROCY_UPDATE:
        break;
    }


    return OK;
}


// 解析指令cmdS[MAX_CMDS]
/**
  * @brief  解析指令cmdS[MAX_CMDS]
  * @param  None
  * @retval None
  * @writer liuzhishan
  *  @modify
  */
void NET_parseInstruction(struct msgStu *pNmsgR)
{
    uint16_t sn;
    if (pNmsgR->usable != 2)
        return;

#if BIGENDIAN
    sn = (uint16_t)(((pNmsgR->sn[0] << 8 | pNmsgR->sn[1])) );
#else
    sn = (uint16_t)(((pNmsgR->sn[1] << 8 | pNmsgR->sn[0])) );
#endif

    if (sn == 0) //心跳sn=0
    {

    }
    else if (sn % 2 == 0)
    {
        //返回应答信息
        Net_Ans(pNmsgR);
        //偶数表示服务器下发指令信息
        NET_parseData(pNmsgR);

    }
    else if (sn % 2)
    {

        //奇数为网关发送后服务器返回应答信息

        Ans_parse(pNmsgR);
    }

    pNmsgR->usable = 0;
}
/**
  * @brief  解析所有命令
  * @param  None
  * @retval None
  * @writer liuzhishan
  *   @modify
  */
void parseAllCmd()
{
    uint32_t i = 0;
    DEBUG(USARTn, "\r\n NET parseAllCmd \r\n");
    while (i < RECV_CMDS_NUM)
    {
        NET_parseInstruction(&netRecvDataCMDS[i++]);
    }
}


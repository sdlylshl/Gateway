#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net.h"
#include "config.h"

#define BIGENDIAN 1
//发送缓冲器 最大条数30x单条指令长度50
uint8_t sendBuf[SEND_CMDS_NUM][CMD_DATA_LEN];

//接收缓冲器 512
uint8_t NET_buf[NET_DATA_LEN];
//读写指针
volatile uint16_t NET_write ;
volatile uint16_t NET_read ;

extern void zigbee_operate(struct devTable *pdevTbs);

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


void sendTimer()
{
    uint32_t i = 0;
    while (i < SEND_CMDS_NUM)
    {
        if (sendBuf[i][0] > 1)
        {
            sendBuf[i][0]++;
        }
        if (sendBuf[i][0] > 3)
        {
            uart_print(0, sendBuf[i]);
        }
        i++;
    }
}




//全局设备表
struct devTable  devTbs[MAX_DEVTABLE_NUM];

//最大全局策略表大小
struct strgytable strategytable[MAX_DESTABLE_NUM];

//最大 指令表 发送到服务器指令
struct msgStu netSendDataCMDS[SEND_CMDS_NUM];
//最大 指令表 接收到服务器指令
struct msgStu netRecvDataCMDS[RECV_CMDS_NUM];


//extern uint32_t Net_cmd[20];
//uint8_t data[200];
//uint32_t cmdRcv = 0;
//uint32_t cmdDeal = -1;

//发送数据包随机号 ：指令包为奇数 应答包为用偶数，  0 表示心跳
uint16_t  msgSN = 1;


void loadDevMsg()
{


}
/**
  * @brief  获取可用的发送缓冲
  * @retval 返回发送结构体指针
  * @writer lishoulei
  * @modify
  */
struct msgStu *getSendBuf(void)
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

uint8_t *getOneSendBuf()
{
    uint32_t i = 1;
    while (i < SEND_CMDS_NUM)
    {

        if (sendBuf[i][0] == 0)
        {
            sendBuf[i][0] = 1;
            sendBuf[i][1] = 0xE0;
            msgSN += 2;
            if (msgSN >= 65533)
                msgSN = 1;
            sendBuf[i][2] = (msgSN >> 8) & 0xFF;
            sendBuf[i][3] = msgSN & 0xFF;
            return &sendBuf[i][0];
        }
        i++;
    }
    return NULL;
}

/**
  * @brief  获取msgSN
  * @retval 返回msgSN
  * @writer lishoulei
  * @modify
  */
uint16_t getmsgSN()
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
int8_t Net_send(struct msgStu *pNmsgS)
{
    if (pNmsgS->usable == 2)
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
    pNmsgS->usable = 3;
    return OK;
}
void Net_Ans(struct msgStu *ansdat)
{
		
    uint16_t sn;
		uint32_t crc = 0;
#if BIGENDIAN
    sn = (uint16_t)(((ansdat->sn[0] << 8 | ansdat->sn[1])) + 1);
#else
    sn = (uint16_t)(((ansdat->sn[1] << 8 | ansdat->sn[0])) + 1);
#endif
    if (0 == sn)
    {
        sn = 2;
    }
    Net_PutChar(ansdat->head);
		Net_PutChar(0);
    Net_PutChar((uint8_t)(sn >> 8) & 0xff);
    Net_PutChar((uint8_t)sn & 0xff);
    //TODO
    //生成CRC校验码

    CRC_ResetDR();
  
    CRC_CalcCRC((uint32_t)0);
    CRC_CalcCRC((uint32_t)((sn >> 8) & 0xff));
		CRC_CalcCRC((uint32_t)(sn & 0xff));
    crc = CRC_GetCRC();
    Net_PutChar((uint8_t)(crc>>24)&0xFF);
		Net_PutChar((uint8_t)(crc>>16)&0xFF);
		Net_PutChar((uint8_t)(crc>>8)&0xff);
		Net_PutChar((uint8_t)(crc&0xff));
    Net_PutChar(ansdat->endl);

}



/**
  * @brief  发送数据，最大25字符
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
  * @writer lishoulei
  *   @modify
  */
int8_t Net_send_data(uint8_t len, uint8_t data[])
{
    uint8_t i;
    uint32_t crc = 0;
    uint8_t *psndat;
    struct msgStu *pNmsgS;
    if (len > NET_DATA_LEN)
    {
        return DATAFLOW;
    }
    //从缓冲区中获取获取
    pNmsgS = getSendBuf();
    if (NULL == pNmsgS)
    {
        return NOSPACE;
    }

    pNmsgS->usable = 1; //正在填充数据
    pNmsgS->head = NET_CMD_HEAD;
		pNmsgS->len = len;
    //获取新的发送Sn
    getmsgSN();
    
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
    psndat = &pNmsgS->sn[0];
    CRC_ResetDR();
    for (i = 0; i < len + 3; i++)
    {
        CRC_CalcCRC((uint32_t)*psndat++);
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
    Net_send(pNmsgS);
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
uint8_t Net_send_device(struct  devTable *pdevTbs, uint8_t CMD, uint8_t control)
{
    uint8_t len = 0;
    uint8_t data[CMD_DATA_LEN];
    data[len++] = CMD;
    data[len++] = control;

    //control为0全部更新
    if (control & DEV_STATE)
    {
        data[len++] = pdevTbs->devstate;

    }
    if (control & DEV_PROTOCOL)
    {
        data[len++] = pdevTbs->protocol;

    }
    if (control & DEV_NETID)
    {
#if BIGENDIAN
        data[len++] = (uint8_t)((pdevTbs->netId >> 8) & 0xff);
        data[len++] = (uint8_t)((pdevTbs->netId >> 0) & 0xff);
#else
        data[len++] = (uint8_t)((pdevTbs->netId >> 0) & 0xff);
        data[len++] = (uint8_t)((pdevTbs->netId >> 8) & 0xff);
#endif

    }
    if (control & DEV_MAC)
    {
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

    }
    if (control & DEV_ACTST)
    {
#if BIGENDIAN
        data[len++] = (uint8_t)((pdevTbs->ActSt >> 8) & 0xff);
        data[len++] = (uint8_t)((pdevTbs->ActSt >> 0) & 0xff);
#else
        data[len++] = (uint8_t)((pdevTbs->ActSt >> 0) & 0xff);
        data[len++] = (uint8_t)((pdevTbs->ActSt >> 8) & 0xff);
#endif
    }
    if (control & DEV_CURST)
    {
#if BIGENDIAN
        data[len++] = (uint8_t)((pdevTbs->curSt >> 8) & 0xff);
        data[len++] = (uint8_t)((pdevTbs->curSt >> 0) & 0xff);
#else
        data[len++] = (uint8_t)((pdevTbs->curSt >> 0) & 0xff);
        data[len++] = (uint8_t)((pdevTbs->curSt >> 8) & 0xff);
#endif
    }
    if (control & DEV_TYPE)
    {
        data[len++] = pdevTbs->type;
    }
    if (control & DEV_NAME)
    {
#if BIGENDIAN
        data[len++] = pdevTbs->name[0];
        data[len++] = pdevTbs->name[1];
        data[len++] = pdevTbs->name[2];
        data[len++] = pdevTbs->name[3];
        data[len++] = pdevTbs->name[4];
        data[len++] = pdevTbs->name[5];
        data[len++] = pdevTbs->name[6];
        data[len++] = pdevTbs->name[7];
        data[len++] = pdevTbs->name[8];
        data[len++] = pdevTbs->name[9];
        data[len++] = pdevTbs->name[10];
#else
        data[len++] = pdevTbs->name[10];
        data[len++] = pdevTbs->name[9];
        data[len++] = pdevTbs->name[8];
        data[len++] = pdevTbs->name[7];
        data[len++] = pdevTbs->name[6];
        data[len++] = pdevTbs->name[5];
        data[len++] = pdevTbs->name[4];
        data[len++] = pdevTbs->name[3];
        data[len++] = pdevTbs->name[2];
        data[len++] = pdevTbs->name[1];
        data[len++] = pdevTbs->name[0];
#endif
    }

    Net_send_data(len, data);
    return OK;
}

// Net_send_device(11,MAC|CURST);
/**
  * @brief  发送celue
  * @param  CMD:
  * @param  control:
  * @retval 发送成功返回 OK
  * @writer
  * @modify
  */

/*******************************parse******************************************/

/**
  * @brief  通过MAC地址表获取设备表
  * @param  None
  * @retval None
    * @writer liuzhishan
    *   @modify
  */

struct devTable *getDevTbsByMac(uint8_t *mac)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint8_t flag = 0;

    while (i < MAX_DEVTABLE_NUM)
    {

        flag = 1;

        for (j = 0; j < 8; j++)
        {
            if (devTbs[i].mac[i] != mac[i])
                flag = 0;
        }

        if ( flag)
        {
            return &devTbs[i];
        }
        i++;
    }
    return NULL;
}
/**
  * @brief  通过NetId地址表获取设备表
  * @param  None
  * @retval struct devTable *
  * @writer liuzhishan
  *   @modify
  */
struct devTable *getDevTbsByNetId(uint16_t id)
{
    uint32_t i = 0;
    while (i < MAX_DEVTABLE_NUM)
    {
        if (devTbs[i].netId == id)
        {
            return &devTbs[i];
        }
        i++;
    }
    return NULL;
}

/**
  * @brief  从头获取可用指令列表
  * @param  None
  * @retval struct msgStu *
  * @writer liuzhishan
  *   @modify
  */
struct msgStu *getACmdS()
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

void print_DEVS()
{
    uint16_t i = 0;
    uint16_t j = 0;
    //uint8_t *pcmd;

    while (i < MAX_DEVTABLE_NUM)
    {

        if (devTbs[i].devstate )
        {

            //pcmd = (uint8_t *)&devTbs[i];

            j++;

        }
        i++;
    }

    SendCMD(j);
    SendCMD(0x5B);


}
//DEBUG END
void NET_read_backward(uint8_t dec)
{
    while (dec--)
    {
        if (NET_read == 0)
            NET_read = (NET_DATA_LEN - 1);
        else
            NET_read--;
    }
}


void NET_read_forward()
{


    if (NET_read == (NET_DATA_LEN - 1))
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
    DEBUG(USARTn, "\r\n net parseRcvBufToLst2 \r\n");
    while ((NET_write + NET_DATA_LEN - NET_read) % NET_DATA_LEN > 2)
    {


        //检测到包头
        if (NET_buf[NET_read] == NET_CMD_HEAD)
        {
            NET_read_forward();
            //取得接收数据长度
            len_tmp = NET_buf[NET_read];
            //当前无效帧
            if (len_tmp > CMD_DATA_LEN)
            {
                break;
            }
            else if ((NET_write + NET_DATA_LEN - NET_read) % NET_DATA_LEN > len_tmp + 7)
            {
                //接收到完整指令 NET_read+3 代表len 位置  len1+sn2+crc4 =7字节 endl 可忽略

                struct msgStu *pNmsgR;
                uint8_t *psndat;
                uint32_t crc;
                //获取可用指令表
                pNmsgR = getACmdS();
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
                psndat = &pNmsgR->sn[0];
                CRC_ResetDR();
                for (i = 0; i < len_tmp + 3; i++)
                {
                    CRC_CalcCRC((uint32_t)*psndat++);
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

            }else
						{
							//未接收到完整的数据帧 退回
							NET_read_backward(1);
						}

        }
        else
        {
            NET_read_forward();
        }


    }//end while


}


//应答包解析
void Ans_parse(struct msgStu *pNmsgR)
{
    uint16_t sn = ((pNmsgR->sn[1] << 8) | pNmsgR->sn[0]) - 1 ;
    uint8_t sn1 = (sn >> 8) & 0xff;
    uint8_t sn2 = sn & 0xff;


    uint16_t i = 0;
    //修改对应重发列表的 usable
    while (i < SEND_CMDS_NUM)
    {
        if (sendBuf[i][0] > 1 && sn1 == sendBuf[i][2] && sn2 == sendBuf[i][3])
        {
            sendBuf[i][0] = 0;
            break;
        }
        i++;
    }
}


//指令包解析
void NET_parseData(struct msgStu *pNmsgR)
{
    uint8_t i = 0;
    uint8_t index;
    uint8_t *mac;
    uint16_t netId;
    struct devTable *pdevTbs = NULL;
    //uint8_t ctr;
    // uint8_t *msgStu = (uint8_t *) & (pNmsgR->data[0]);
    switch (pNmsgR->data[0])
    {
    case 0x10://设备查询
        //检测控制字
        if (pNmsgR->data[1])
        {
            index = 2;
            if (pNmsgR->data[1] & 0x01)
            {
                index++;
            }
            if (pNmsgR->data[1] & 0x02)
            {
                index++;
            }

            if (pNmsgR->data[1] & 0x04)
            {
                //通过网络号获取设备netId
                netId = (uint16_t)((pNmsgR->data[index] << 8) | pNmsgR->data[index + 1]);
                //查找netId所在的字节
                pdevTbs = getDevTbsByNetId(netId);
                if (pdevTbs != NULL)
                {
                    Net_send_device(pdevTbs, DEVTAB_UPDATE, pNmsgR->data[1]);
                }
                index += 2;
            }
            if (pNmsgR->data[1] & 0x08)
            {
                //通过MAC获取设备

                //查找MAC所在的字节
                mac = &pNmsgR->data[index];

                pdevTbs = getDevTbsByMac(mac);
                if (pdevTbs != NULL)
                {
                    Net_send_device(pdevTbs, DEVTAB_UPDATE, pNmsgR->data[1]);
                }
                index += 4;
            }

        }
        else
        {
            i = 0;
            //更新全部设备表
            while (i < MAX_DEVTABLE_NUM)
            {
                if (devTbs[i].devstate)
                {
                    Net_send_device(&devTbs[i], DEVTAB_UPDATE, 0xFF);
                }
                i++;
            }

        }

        break;

    case 0x11://设备更新


        index = 2;
        if (pNmsgR->data[1] & 0x01)
        {
            index++;
        }
        if (pNmsgR->data[1] & 0x02)
        {
            index++;
        }

        if (pNmsgR->data[1] & 0x04)
        {
            //通过网络号获取设备netId
            netId = (uint16_t)((pNmsgR->data[index] << 8) | pNmsgR->data[index + 1]);
            //查找netId所在的字节
            pdevTbs = getDevTbsByNetId(netId);

            index += 2;
        }
        if (pNmsgR->data[1] & 0x08)
        {
            //只能通过MAC获取设备其他都是不可靠的

            //查找MAC所在的字节
            mac = &pNmsgR->data[index];
            pdevTbs = getDevTbsByMac(mac);

            index += 4;
        }
        //状态设置
        if (pNmsgR->data[1] & 0x10)
        {
            if (pdevTbs != NULL)
            {
                //设置对应的状态
                pdevTbs->ActSt = (pNmsgR->data[index] << 8 | pNmsgR->data[index + 1]);

                //下发Zigbee设置指令
								zigbee_operate(pdevTbs);

            }

            index += 2;
        }
        //当前状态
        if (pNmsgR->data[1] & 0x20)
        {

            index += 2;
        }
        //设备类型
        if (pNmsgR->data[1] & 0x40)
        {

            if (pdevTbs != NULL)
            {
                //设置对应的状态
                pdevTbs->type = pNmsgR->data[index];
            }
            index++;
        }
        //设备名称
        if (pNmsgR->data[1] & 0x80)
        {

            if (pdevTbs != NULL)
            {

                //设置对应的状态
                for (i = 0; i < 11; i++)
                {
                    pdevTbs->name[i] = pNmsgR->data[index++];
                }

            }


        }

        break;
    }
    //
    pNmsgR->usable = 0;

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
    if (pNmsgR->usable != 2)
        return;


    if ((pNmsgR->sn[0] == 0 && pNmsgR->sn[1] == 0)) //心跳sn=0
    {

    }
    else if (pNmsgR->sn[1] % 2 == 0)
    {
        //偶数为应答信息
        Ans_parse(pNmsgR);

    }
    else if (pNmsgR->sn[1] % 2)
    {
        //返回应答信息
        Net_Ans(pNmsgR);
        //奇数表示指令信息
        NET_parseData(pNmsgR);

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

/**********************************policy**************************************/
/**
  * @brief  从策略表中获取可填充策略位置
  * @param  None
  * @retval None
    * @writer liuzhishan
    *   @modify
  */
struct strgytable *getANewDES()
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
void  policydecisions()
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

void devInit(void)
{
    //pwNr = 0;

    devTbs[1].mac[0] = 10;
    devTbs[1].mac[1] = 10;
    devTbs[1].netId = 0x3600;
    devTbs[1].protocol = 3;
    devTbs[1].devstate = 1;

    devTbs[2].mac[0] = 25;
    devTbs[2].netId = 3600;
    devTbs[2].protocol = 4;
    devTbs[2].devstate = 2;

}
uint32_t writeFlag = 0;
uint8_t numTick = 0;
uint32_t net(void)
{
    //  uint32_t curRdNr = 0;
    //RCC_Configuration();
    //NVIC_Configuration();
    //USART1_Configuration();
    USARTx_Init(USART1);
    loadDevMsg();


    while (1)
    {

        //将指令从缓冲池中解析出来，放入指令表中

        //从指令表中取出所有指令 解析 完成 心跳、应答、指令处理
        parseAllCmd();
        //策略解析，修改对应 设备表
        policydecisions();
        //重发机制
        sendTimer();
        Delay(60);
        Net_PutChar(0xaa);

    }

}

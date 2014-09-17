
#include "stm32f10x.h"
#include "Zigbee.h"
#include <stdio.h>

#include "config.h"


#define Zigbee_DBG USARTx_printf
#define Zigbee_DBG_USARTx USART3

uint8_t Zigbee_buf[BUFFSIZE] ;
uint8_t Zigbee_cmd[CMDSIZE];
//Zigebee 接收缓存
struct Zigbee_msgStu Zigbee_RecvBuff[ZIGEBE_RECV_CMD_NUM];
struct Zigbee_msgStu Zigbee_SendBuff[ZIGEBE_SEND_CMD_NUM];

__IO uint16_t Zigbee_read = 0;
__IO uint16_t Zigbee_write = 0;

extern void zigbee_operate(struct devTable *pdevTbs);
uint8_t Net_send_device(struct devTable *pdevTbs, uint8_t CMD, uint8_t control);
void SendCMD(uint8_t data)
{

    while ( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET );
    USART_SendData(USART2, data);

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

    uint8_t i = 0;
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
                        zigbee_remote_set_net_io(pdevTbs->netId, j, IOmod, 0);
                    }
                }
            }
            else
            {
                //为0执行默认IO查询
                zigbee_remote_req_net_io(pdevTbs->netId, IO_D2 );

            }
        }

    }//end devstate
}


void zigbee_operate_ALL(void)
{
		uint8_t i=0;
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
void Zigbee_fetchParseInstruction (void)
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
            else if ((Zigbee_write + ZIGBEE_BUFFSIZE - Zigbee_read) % ZIGBEE_BUFFSIZE > len_tmp + 4)
            {
                //接收到完整指令   len1+cmd2+chk1 =4字节

                //由于接收到的数据不是连续存储的，所以必须先转存 后校验
                //获取可用指令接收表
                pZmsgS = getRecvBuff();
                if (pZmsgS == NULL)
                {
                    return;
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
                }
                else
                {
                    //校验失败 此处可能存在丢包
                    pZmsgS->usable = 0;

                }

            }

        }
        else
        {
            Zigbee_read_forward();
        }


    }//end while


}
void Zigbee_parseInstruction(struct Zigbee_msgStu *pZmsgS)
{
    struct devTable *pdevTbs = NULL;
    uint16_t *pcmd;
    uint16_t *pnetId;
    uint16_t *pvalue;
    if (pZmsgS == NULL)
    {
        //TODO ERROR
        return;
    }
    //命令字
    pcmd = (uint16_t *)(pZmsgS->cmd);

    // printf("ins: 0x%x\n", *ins);
    switch (*pcmd)
    {
    //远程设置某个网络设备 IO值
    case ZIGBEE_REMOTE_SET_RECV_DATA:

        if (pZmsgS->data[3])
        {
            //设置成功
            pnetId = (uint16_t *)(&pZmsgS->data[0]);
            pdevTbs = getDevTbsByNetId(*pnetId);
            if (pdevTbs != NULL)
            {
                //更新设备表的状态位
                pdevTbs->curSt = pdevTbs->ActSt & 0x0100;
                //更新标志 通过NET发送到服务器
                //pdevTbs->ActSt = pdevTbs->ActSt | 0x4000;

								//NET立即发送更新设备状态 MAC ActSt curSt
								 Net_send_device(pdevTbs, DEVTAB_UPDATE, 0x38);


            }
            else
            {
                //无此网络号 更新设备
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
        pvalue = (uint16_t *)(&pZmsgS->data[4]);
        pdevTbs = getDevTbsByNetId(*pnetId);
        if (pdevTbs != NULL)
        {
            //更新curSt
            pdevTbs->curSt = *pvalue;
            //更新 zigbee IOn
            pdevTbs->ActSt =(1 << pZmsgS->data[2]);
						//更新标志 通过NET发送到服务器
						//pdevTbs->ActSt = pdevTbs->ActSt | 0x4000;
					//NET立即发送更新设备状态 MAC ActSt curSt
					  Net_send_device(pdevTbs, DEVTAB_UPDATE, 0x38);

        }
        else
        {
            //无此网络号 更新设备
        }

        break;



    }
    //释放当前指令
    pZmsgS->usable = 0;
}


void zigbee_cmd(uint8_t len, uint16_t cmd, uint8_t buf[])
{

    uint8_t snd_buf[CMDSIZE];
    uint8_t ret;
    uint8_t i;

    snd_buf[0] = ZIGBEE_API_FRAME_HEAD;
    snd_buf[1] = len;
    //小端模式
    snd_buf[2] = (uint8_t)cmd & 0xFF;
    snd_buf[3] = (uint8_t)(cmd >> 8) & 0xFF;
    //*((uint16_t*)(snd_buf+4)) = dst;

    for (i = 0; i < len; i++)
        snd_buf[4 + i] = buf[i];

    ret = calcfcs(snd_buf + 1, len + 3);
    snd_buf[len + 4] = ret;
		snd_buf[len + 5] = 0x00;
    //命令输出 head1+len1+cmd2+chk1=5字节
    for (i = 0; i < len + 6; i++)
        SendCMD(snd_buf[i]);

}


/**
  * @brief  发送数据，最大25字符
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
    * @writer lishoulei
    *   @modify
  */
int8_t zigbee_send_data(uint8_t len, uint16_t netid, uint8_t buf[])
{
    uint8_t snd_buf[CMDSIZE];
    uint8_t i;
    if (len > CMDSIZE - 7)
        return ERROR;
    //低位先发送
    snd_buf[0] = (uint8_t) netid & 0xff;
    snd_buf[1] = (uint8_t) (netid >> 8) & 0xff;
    for (i = 0; i < len; i++)
        snd_buf[2 + i] = buf[i];
    zigbee_cmd(len + 2, ZIGBEE_SND_DATA, snd_buf);

    return OK;

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

    zigbee_cmd(4, ZIGBEE_REMOTE_REQ_IO, data);

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
void zigbee_remote_set_net_io(uint16_t netid, uint8_t IOn, uint8_t IOmode, uint8_t IOvalue)
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
    zigbee_cmd(7, ZIGBEE_REMOTE_SET_IO, data);

}


int zigbee_req_net_mac(uint8_t len, uint16_t dst)
{
    uint8_t snd_buf[CMDSIZE];
    uint8_t dst_buf[CMDSIZE];
    uint8_t ret;
    uint8_t i;

    snd_buf[0] = 0xfe;
    snd_buf[1] = len;
    snd_buf[2] = 0x24;
    snd_buf[3] = 0x5d;
    *((uint16_t *)(snd_buf + 4)) = dst;
    snd_buf[6] = 2;


    ret = calcfcs(snd_buf + 1, len + 3);
    snd_buf[len + 4] = ret;
    //for(i=0; i<len+5; i++)
    //  printf("%02x", snd_buf[i]);
    for (i = 0; i < len + 5; i++)
        sprintf((char *)dst_buf + i * 2, "%02x", snd_buf[i]);

    printf("%s", dst_buf);

    return OK;
}

int zigbee_req_net_addr(uint8_t len, unsigned __int64 mac)
{
    uint8_t snd_buf[CMDSIZE];
    uint8_t dst_buf[CMDSIZE];
    uint8_t ret;
    uint8_t i;

    snd_buf[0] = 0xfe;
    snd_buf[1] = len;
    snd_buf[2] = 0x24;
    snd_buf[3] = 0x5c;
    snd_buf[4] = 3;
    *((unsigned __int64 *)(snd_buf + 5)) = mac;



    ret = calcfcs(snd_buf + 1, len + 3);
    snd_buf[len + 4] = ret;
    //for(i=0; i<len+5; i++)
    //  printf("%02x", snd_buf[i]);
    for (i = 0; i < len + 5; i++)
        sprintf((char *)dst_buf + i * 2, "%02x", snd_buf[i]);

    printf("%s", dst_buf);

    return OK;
}

//
int zigbee_switch_mode(uint8_t mode)
{
    uint8_t snd_buf[CMDSIZE];
    uint8_t dst_buf[CMDSIZE];
    uint8_t ret;
    uint8_t i;

    snd_buf[0] = 0xfe;
    snd_buf[1] = 0x01;
    snd_buf[2] = 0x21;
    snd_buf[3] = 0x2a;
    snd_buf[4] = mode;

    ret = calcfcs(snd_buf + 1, 4);
    snd_buf[5] = ret;
    //for(i=0; i<len+5; i++)
    //  printf("%02x", snd_buf[i]);
    for (i = 0; i < 6; i++)
        sprintf((char *)dst_buf + i * 2, "%02x", snd_buf[i]);

    printf("%s", dst_buf);

    return OK;
}


int snd()
{
#if 0
    uint8_t buf[9];

    buf[0] = 0x41;
    buf[1] = 0x41;
    buf[2] = 0x41;
    buf[3] = 0x41;
    buf[4] = 0;

    //printf("%s", buf);
    zigbee_snd_data(6, 0x5f24, 0x00, buf);
#endif

    //zigbee_req_net_io(4, 0x0e10, 2);
    //zigbee_set_net_io(7, 0xe10, 2, 1, 0);
    //zigbee_req_all_net_stat(1);
    //zigbee_req_net_mac(3, 0xe10);
    //zigbee_req_net_addr(9, 0x00124B000228CFAA);
    //zigbee_switch_mode(ZIGBEE_MODE_API);
    return OK;
}

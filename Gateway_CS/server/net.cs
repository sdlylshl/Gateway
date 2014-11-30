//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;

//namespace server
//{

//    public class net
//    {
//        // 接收指令最大条数
//        const int  RECV_CMDS_NUM  = 30;
//        // 发送指令最大条数
//        const int   SEND_CMDS_NUM = 30;

//        // 指令最大长度
//        const int   CMD_LEN = 50;
//        // 指令数据区最大长度 指令长度减去头
//        const int   CMD_DATA_LEN = CMD_LEN-10;

//        // 接收缓冲区大小
//        const int   NET_DATA_LEN = 512;

//        // 设备 最大条数
//        const int    MAX_DEVTABLE_NUM = 20;
//        // 策略表 最大条数
//        const int    MAX_DESTABLE_NUM = 20;

//        const string   NET_CMD_HEAD = "E0" ;
//        const string   NET_CMD_END = "18";


//        const string   DEVTAB_REQUEST = "10";
//        const string   DEVTAB_UPDATE  = "11";

//        const string   DEV_STATE = "01";



//        public enum devTbCV
//        {
//            STATE = 1,      //设备状态 0
//            PROTOCOL = 2, //接口类型 1
//            NETID = 0x04,   //网络号     2
//            MAC = 0x08,         //唯一设备MAC 3
//            ACTST = 0x10,   //执行状态
//            CURST = 0x20,   //当前状态
//            TYPE = 0x40,        //设备类型
//            NAME = 0x80         //设备名称
//        };


//        /*
//          1、与服务端通信的数据格式
//            1[1]           2[1]      3[2]     4 [8]             5[4]       6 [1]      7[10+1]
//         设备状态    接口类型     网络号   唯一设备MAC  执行状态 当前状态 设备类型  “名称”
//         在线0x01    zigbee--0x01 0x8232   0x12345678   4字节状态值     烟感0x81 “烟雾传感器”
//         离线0x00    wifi--0x02   0x0233   0x12312354   2字节    2字节  灯--0x01 “卧室灯2”
//         休眠0x02    蓝牙-0x03    0x0234   0x32132122   2字节    2字节  开关0x02 “客厅开关”
//        */
//        //28字节
//        public struct  devTable
//        {
//            byte     devstate;
//            byte     protocol;
//            UInt16    netId;
//            byte[]     mac = new byte[8];
//            UInt16    ActSt;
//            UInt16    curSt;
//            byte     type;
//            byte[]     name = new byte[11];
//        };
//        /*
//        struct  devTable
//        {
//            byte     state;
//            byte     protocol;
//            uint16_t    netId;
//            uint64_t    mac;
//                  union
//            {
//                UInt32 Sensor_State;
//                struct
//                {
//                            uint16_t  Actuator_ActSt;
//                            uint16_t  Actuator_curSt;
//                } st;
//            }status;

//            byte   type;
//            byte   name[10];
//        };
//        */

//        struct sensorInfo
//        {

//            byte[] sensorId = new byte[8];  //设备ID 可选 网络号|MAC
//            UInt32 sensorTrigger;

//        };

//        struct actuatorInfo
//        {

//            byte[] actuatorId = new byte[8];    //设备ID 可选 网络号|MAC
//            byte actuatorTrigger;

//        };
//        /**
//          * @brief 全局策略结构体:
//                                序号  1               2           3                   4                   5                   6                       7
//                                            优先级       id      触发数量        触发类型    传感器*N         控制器*M         名称
//                                字节  1字节     1字节 1字节         1字节     2字节+4字节 2字节+1字节 10字节
//                                            0为禁用  最大255   高2位传感器（4个）  低6位 控制器数量（64个）  两位(2bit)标识一个传感器       传感器标识 触发条件    控制器标识 控制状态    设备名称

//            * @pram  usable  0 未使用 1正在填充 2可用 >=3 定时自加
//                             type 无效    0b00 开关 0b01 小触发  0b10 大触发  0b11
//            * @writer liuzhishan
//            * @modify
//          */
//        struct strgytable
//        {
//            byte usable; //操作状态

//            byte first;  //优先级
//            byte id;         //
//            byte num;
//            byte type;
//            sensorInfo[] sensor = new sensorInfo[4];
//            actuatorInfo[] actuator = new actuatorInfo[10];
//            byte[] name = new byte[10];

//        };


//        /**
//          * @brief 单条指令存储结构体
//            序号  1   2   3   4   5   6
//            字节数   1   2   1   N   4   1
//            指令头   随机号   数据长度    数据  校验  结束符

//            *@parm usable   [netSendDataCMDS]0没有使用 1正在填充 2有效 n 超时递增
//                            [netRecvDataCMDS]0没有使用 1正在填充 2有效
//            *@parm head 起始符 NET_CMD_HEAD 0xE0
//            *@parm sn   0 心跳包 [奇数] 指令包 [偶数] 应答包
//            *@parm len  data 数据长度
//            *@parm crc  sn+len+data
//            *@parm endl 结束符NET_CMD_END0x18
//            head    sn      len         Data    CRC         End
//            * @writer liuzhishan
//            * @modify lishoulei
//          */
//        //0x56字节
//        struct msgStu
//        {
//            byte    usable;
//            byte    head  ;//0xE0
//            byte[]    sn = new byte[2]  ;//
//            byte    len;//data 的长度
//            byte[]    data =new byte[CMD_DATA_LEN];
//            byte[]    crc = new byte[4];
//            byte    endl;//0x18

//        };

////发送缓冲器 最大条数30x单条指令长度50
//byte[,] sendBuf = new byte[SEND_CMDS_NUM,CMD_DATA_LEN];

////接收缓冲器 512
//byte[] NET_buf = new byte[NET_DATA_LEN];
////读写指针
//volatile UInt32 NET_write ;
//volatile UInt32 NET_read ;

//void Net_PutChar(byte ch)
//{
//    USART_SendData(USART1, (byte) ch);
//    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//}

//void uart_print( byte len, byte *data)
//{
//    UInt32 i = 1;
//    while (i < len)
//    {
//        Net_PutChar(data[i++]);
//    }
//    data[0] = 2;

//}


//void USART1_IRQHandler(void)            //在中断服务程序中，由于主机响应中断时并不知道是哪个中断源发出中断请求，因此必须在中断服务程序中对中断源进行判别，然后分别进行处理。当然，如果只涉及到一个中断请求，是不用做上述判别的。但是无论什么情况，做上述判别是个好习惯
//{
//    byte dat;
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {
//        dat = USART1->DR;
//        NET_buf[NET_write] = dat;
//        if (NET_write == NET_DATA_LEN - 1)
//            NET_write = 0;
//        else
//            NET_write++;
//    }
//    //SendCMD(dat);

//}

//void sendTimer()
//{
//    UInt32 i = 0;
//    while (i < SEND_CMDS_NUM)
//    {
//        if (sendBuf[i][0] > 1)
//        {
//            sendBuf[i][0]++;
//        }
//        if (sendBuf[i][0] > 3)
//        {
//            uart_print(0, sendBuf[i]);
//        }
//        i++;
//    }
//}




////全局设备表
//struct devTable  devTbs[MAX_DEVTABLE_NUM];

////最大全局策略表大小
//struct strgytable strategytable[MAX_DESTABLE_NUM];

////最大 指令表 发送到服务器指令
//struct msgStu netSendDataCMDS[SEND_CMDS_NUM];
////最大 指令表 接收到服务器指令
//struct msgStu netRecvDataCMDS[RECV_CMDS_NUM];


////extern UInt32 Net_cmd[20];
////byte data[200];
////UInt32 cmdRcv = 0;
////UInt32 cmdDeal = -1;

////发送数据包随机号 ：指令包为奇数 应答包为用偶数，  0 表示心跳
//uint16_t  msgSN = 1;


//void loadDevMsg()
//{


//}
///**
//  * @brief  获取可用的发送缓冲
//  * @retval 返回发送结构体指针
//  * @writer lishoulei
//  * @modify
//  */
//struct msgStu *getSendBuf(void)
//{
//    byte i = 0;
//    while (i < SEND_CMDS_NUM)
//    {
//        if (netSendDataCMDS[i].usable == 0)
//        {
//            return &netSendDataCMDS[i];
//        }

//        i++;
//    }
//    return NULL;
//}

//byte *getOneSendBuf()
//{
//    UInt32 i = 1;
//    while (i < SEND_CMDS_NUM)
//    {

//        if (sendBuf[i][0] == 0)
//        {
//            sendBuf[i][0] = 1;
//            sendBuf[i][1] = 0xE0;
//            msgSN += 2;
//            if (msgSN >= 65533)
//                msgSN = 1;
//            sendBuf[i][2] = (msgSN >> 8) & 0xFF;
//            sendBuf[i][3] = msgSN & 0xFF;
//            return &sendBuf[i][0];
//        }
//        i++;
//    }
//    return NULL;
//}

///**
//  * @brief  获取msgSN
//  * @retval 返回msgSN
//  * @writer lishoulei
//  * @modify
//  */
//uint16_t getmsgSN()
//{

//    if (msgSN % 2)
//    {
//        // 奇数
//        msgSN += 2;
//    }
//    else
//    {
//        msgSN++;
//    }
//    return msgSN;
//}

///**
//  * @brief  发送数据，最大25字符
//  * @param  NETID:
//  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
//  * @retval 发送成功返回 OK
//  * @writer lishoulei
//  *   @modify
//  */
//int8_t Net_send(struct msgStu *senddat)
//{
//    if (senddat->usable == 2)
//    {
//        byte i;
//        Net_PutChar(senddat->head);
//        Net_PutChar(senddat->sn[0]);
//        Net_PutChar(senddat->sn[1]);
//        Net_PutChar(senddat->len);

//        for (i = 0; i < senddat->len; i++)
//        {
//            Net_PutChar(senddat->data[i]);
//        }
//        Net_PutChar(senddat->crc[0]);
//        Net_PutChar(senddat->crc[1]);
//        Net_PutChar(senddat->crc[2]);
//        Net_PutChar(senddat->crc[3]);
//        Net_PutChar(senddat->endl);
//    }
//    senddat->usable = 3;
//    return OK;
//}
//void Net_Ans(struct msgStu *ansdat)
//{
//    uint16_t sn;
//    sn = (uint16_t)((ansdat->sn[0] << 8 | ansdat->sn[1]) + 1);

//    if (0 == sn)
//    {
//        sn = 2;
//    }
//    Net_PutChar(ansdat->head);
//    Net_PutChar((byte)(sn >> 8) & 0xff);
//    Net_PutChar((byte)sn & 0xff);
//    Net_PutChar(0);
//    Net_PutChar(ansdat->endl);

//}



///**
//  * @brief  发送数据，最大25字符
//  * @param  NETID:
//  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
//  * @retval 发送成功返回 OK
//  * @writer lishoulei
//  *   @modify
//  */
//int8_t Net_send_data(byte len, byte data[])
//{
//    byte i;
//    UInt32 crc = 0;
//    byte *psndat;
//    struct msgStu *psendbf;
//    if (len > NET_DATA_LEN)
//    {
//        return DATAFLOW;
//    }
//    //从缓冲区中获取获取
//    psendbf = getSendBuf();
//    if (NULL == psendbf)
//    {
//        return NOSPACE;
//    }

//    psendbf->usable = 1; //正在填充数据
//    psendbf->head = NET_CMD_HEAD;
//    //获取新的发送Sn
//    getmsgSN();
//    psendbf->sn[0] = (byte)((msgSN >> 8) & 0xff);
//    psendbf->sn[1] = (byte) msgSN & 0xff;
//    psendbf->len = len;
//    for (i = 0; i < len; i++)
//    {
//        psendbf->data[i] = data[i];
//    }
//    //TODO
//    //生成CRC校验码
//    psndat = &psendbf->sn[0];
//    CRC_ResetDR();
//    for (i = 0; i < len + 3; i++)
//    {
//        CRC_CalcCRC((UInt32)*psndat++);
//    }
//    crc = CRC_GetCRC();

//    psendbf->crc[0] = (byte)((crc >> 24) & 0xff);
//    psendbf->crc[1] = (byte)((crc >> 16) & 0xff);
//    psendbf->crc[2] = (byte)((crc >> 8) & 0xff);
//    psendbf->crc[3] = (byte)((crc >> 0) & 0xff);
//    psendbf->endl = NET_CMD_END;
//    psendbf->usable = 2;//填充完成

//    //数据发送
//    Net_send(psendbf);
//    return OK;

//}

///**
//  * @brief  发送设备表信息
//  * @param  CMD:
//  * @param  control:
//  * @retval 发送成功返回 OK
//  * @writer lishoulei
//  * @modify
//  */
//byte Net_send_device(struct  devTable *devTbs, byte CMD, byte control)
//{
//    byte len = 0;
//    byte data[CMD_DATA_LEN];
//    data[len++] = CMD;
//    data[len++] = control;

//    //control为0全部更新
//    if (control & 0x01)
//    {
//        data[len++] = devTbs->devstate;

//    }
//    if (control & 0x02)
//    {
//        data[len++] = devTbs->protocol;

//    }
//    if (control & 0x04)
//    {
//        data[len++] = (byte)((devTbs->netId >> 8) & 0xff);
//        data[len++] = (byte)((devTbs->netId >> 0) & 0xff);

//    }
//    if (control & 0x08)
//    {
//        data[len++] = devTbs->mac[0];
//        data[len++] = devTbs->mac[1];
//        data[len++] = devTbs->mac[2];
//        data[len++] = devTbs->mac[3];
//        data[len++] = devTbs->mac[4];
//        data[len++] = devTbs->mac[5];
//        data[len++] = devTbs->mac[6];
//        data[len++] = devTbs->mac[7];

//    }
//    if (control & 0x10)
//    {
//        data[len++] = (byte)((devTbs->ActSt >> 8) & 0xff);
//        data[len++] = (byte)((devTbs->ActSt >> 0) & 0xff);

//    }
//    if (control & 0x20)
//    {
//        data[len++] = (byte)((devTbs->curSt >> 8) & 0xff);
//        data[len++] = (byte)((devTbs->curSt >> 0) & 0xff);
//    }
//    if (control & 0x40)
//    {
//        data[len++] = devTbs->type;
//    }
//    if (control & 0x80)
//    {
//        data[len++] = devTbs->name[0];
//        data[len++] = devTbs->name[1];
//        data[len++] = devTbs->name[2];
//        data[len++] = devTbs->name[3];
//        data[len++] = devTbs->name[4];
//        data[len++] = devTbs->name[5];
//        data[len++] = devTbs->name[6];
//        data[len++] = devTbs->name[7];
//        data[len++] = devTbs->name[8];
//        data[len++] = devTbs->name[9];
//        data[len++] = devTbs->name[10];
//    }

//    Net_send_data(len, data);
//    return OK;
//}

//// Net_send_device(11,MAC|CURST);
///**
//  * @brief  发送celue
//  * @param  CMD:
//  * @param  control:
//  * @retval 发送成功返回 OK
//  * @writer
//  * @modify
//  */

///*******************************parse******************************************/

///**
//  * @brief  通过MAC地址表获取设备表
//  * @param  None
//  * @retval None
//    * @writer liuzhishan
//    *   @modify
//  */

//struct devTable *getDevTbsByMac(byte *mac)
//{
//    UInt32 i = 0;
//    UInt32 j = 0;
//    byte flag = 0;

//    while (i < MAX_DEVTABLE_NUM)
//    {

//        flag = 1;

//        for (j = 0; j < 8; j++)
//        {
//            if (devTbs[i].mac[i] != mac[i])
//                flag = 0;
//        }

//        if ( flag)
//        {
//            return &devTbs[i];
//        }
//        i++;
//    }
//    return NULL;
//}
///**
//  * @brief  通过NetId地址表获取设备表
//  * @param  None
//  * @retval struct devTable *
//  * @writer liuzhishan
//  *   @modify
//  */
//struct devTable *getDevTbsByNetId(uint16_t id)
//{
//    UInt32 i = 0;
//    while (i < MAX_DEVTABLE_NUM)
//    {
//        if (devTbs[i].netId == id)
//        {
//            return &devTbs[i];
//        }
//                i++;
//    }
//    return NULL;
//}

///**
//  * @brief  从头获取可用指令列表
//  * @param  None
//  * @retval struct msgStu *
//  * @writer liuzhishan
//  *   @modify
//  */
//struct msgStu *getACmdS()
//{
//    UInt32 i = 0;
//    while (i < RECV_CMDS_NUM)
//    {
//        if (netRecvDataCMDS[i].usable == 0)
//        {
//            return &netRecvDataCMDS[i];
//        }
//        i++;
//    }
//    return NULL;

//}
////DEBUG
//void print_CMDS()
//{
//    uint16_t i = 0;
//    uint16_t j = 0;
//    //byte *pcmd;
//    SendCMD(0x5A);
//    SendCMD(NET_write);
//    SendCMD(NET_read);

//    while (i < RECV_CMDS_NUM)
//    {

//        if (netRecvDataCMDS[i].usable > 1)
//        {
//            SendCMD(netRecvDataCMDS[i].usable);
//            SendCMD(netRecvDataCMDS[i].head);
//            SendCMD(netRecvDataCMDS[i].sn[0]);
//            SendCMD(netRecvDataCMDS[i].sn[1]);
//            SendCMD(netRecvDataCMDS[i].len);

//            for (j = 0; j < netRecvDataCMDS[i].len; j++)
//            {
//                SendCMD(netRecvDataCMDS[i].data[i]);
//            }
//            SendCMD(netRecvDataCMDS[i].crc[0]);
//            SendCMD(netRecvDataCMDS[i].crc[1]);
//            SendCMD(netRecvDataCMDS[i].crc[2]);
//            SendCMD(netRecvDataCMDS[i].crc[3]);
//            SendCMD(netRecvDataCMDS[i].endl);
//        }
//        i++;
//    }

//}

//void print_DEVS()
//{
//    uint16_t i = 0;
//    uint16_t j = 0;
//    //byte *pcmd;

//    while (i < MAX_DEVTABLE_NUM)
//    {

//        if (devTbs[i].devstate )
//        {

//            //pcmd = (byte *)&devTbs[i];

//            j++;

//        }
//        i++;
//    }

//    SendCMD(j);
//    SendCMD(0x5B);


//}
////DEBUG END
//void NET_read_backward(byte dec)
//{
//    while (dec--)
//    {
//        if (NET_read == 0)
//            NET_read = (NET_DATA_LEN - 1);
//        else
//            NET_read--;
//    }
//}


//void NET_read_forward(byte inc)
//{

//    while (inc--)
//    {
//        if (NET_read == (NET_DATA_LEN - 1))
//            NET_read = 0;
//        else
//            NET_read++;
//    }
//}
////将接收缓冲器中数据解析成单条指令 存放到cmdS[MAX_CMDS]
//void parseRcvBufToLst2()
//{
//    byte len_tmp;
//    byte i;
//    // SendCMD(NET_write);
//    // SendCMD(NET_read);
//    //包头检测 若接收到的数据长度小于最小心跳包长度 则不再检测包头
//    while ((NET_write + NET_DATA_LEN - NET_read) % NET_DATA_LEN > 4)
//    {


//        //检测到包头
//        if (NET_buf[NET_read] == NET_CMD_HEAD)
//        {

//            //取得接收数据长度
//            len_tmp = NET_buf[NET_read + 3];
//            //当前无效帧
//            if (len_tmp > CMD_DATA_LEN)
//            {
//                NET_read_forward(1);

//                break;
//            }
//            else if ((NET_write + NET_DATA_LEN - NET_read) % NET_DATA_LEN > len_tmp + 8)
//            {
//                //接收到完整指令 NET_read+3 代表len 位置  head1+sn2+len1+crc4+end1 =9字节
//                struct msgStu *pcmdS;
//                byte *psndat;
//                UInt32 crc;
//                //获取可用指令表
//                pcmdS = getACmdS();
//                pcmdS->usable = 1;

//                pcmdS->head = NET_CMD_HEAD;
//                pcmdS->sn[0] = NET_buf[NET_read + 1];
//                pcmdS->sn[1] = NET_buf[NET_read + 2];
//                pcmdS->len = NET_buf[NET_read + 3];
//                NET_read_forward(4);

//                //指向data
//                // SendCMD((byte)pcmdS);
//                // SendCMD((byte)dat);
//                for (i = 0; i < (len_tmp); i++)
//                {
//                    pcmdS->data[i] = NET_buf[NET_read];
//                    NET_read_forward(1);
//                }

//                pcmdS->crc[0] = NET_buf[NET_read + 0];
//                pcmdS->crc[1] = NET_buf[NET_read + 1];
//                pcmdS->crc[2] = NET_buf[NET_read + 2];
//                pcmdS->crc[3] = NET_buf[NET_read + 3];
//                NET_read_forward(4);
//                //
//                pcmdS->endl = NET_CMD_END;
//                NET_read_forward(1);

//                //CRC校验
//                psndat = &pcmdS->sn[0];
//                CRC_ResetDR();
//                for (i = 0; i < len_tmp + 3; i++)
//                {
//                    CRC_CalcCRC((UInt32)*psndat++);
//                }

//                crc = pcmdS->crc[0] << 24 | pcmdS->crc[1] << 16 | pcmdS->crc[2] << 8 | pcmdS->crc[3];

//                if (crc == CRC_GetCRC())
//                {
//                    pcmdS->usable = 2;
//                }
//                else
//                {
//                    //校验失败
//                    NET_read_forward(1);
//                    pcmdS->usable = 0;

//                }

//            }

//        }
//        else
//        {
//            NET_read_forward(1);
//        }


//    }//end while


//}

////将接收缓冲器中数据解析成单条指令 存放到cmdS[MAX_CMDS]
//void parseRcvBufToLst()
//{
//    byte pWR = NET_write;
//    struct msgStu *pcmdS;
//    byte pwNr = 0;
//    while ((pWR + NET_DATA_LEN - NET_read) % NET_DATA_LEN >= 1)
//    {
//        //
//        if (pwNr == 0)
//        {
//            if (NET_buf[NET_read] == NET_CMD_HEAD)
//            {
//                pcmdS->data[pwNr++] = NET_buf[NET_read];
//            }
//        }
//        else if (pwNr < CMD_DATA_LEN)
//        {
//            pcmdS->data[pwNr] = NET_buf[NET_read];
//            if (NET_buf[NET_read] == 0x18 )
//            {
//                /***********************************************
//                *
//                * 此处执行校验
//                * 1. len = data中数据的长度
//                * 2. crc校验
//                *
//                ************************************************/
//                if (pcmdS->data[3] == pwNr - 6)
//                {

//                    pcmdS->len = pwNr - 6;
//                    pcmdS->sn[0] = pcmdS->data[1];
//                    pcmdS->sn[1] = pcmdS->data[2];
//                    pcmdS->endl = 0x18;
//                    pcmdS->usable = 2;
//                }
//                pwNr = 0;
//                pcmdS = getACmdS();

//            }
//            else
//            {
//                pwNr++;
//            }
//        }
//        else//pwNr > CMD_DATA_LEN
//        {
//            pwNr = 0;
//            pcmdS = getACmdS();
//        }

//        if (NET_read >= NET_DATA_LEN - 1)
//        {
//            NET_read = 0;
//        }
//        else
//        {
//            NET_read++;
//        }
//    }

//}
////应答包解析
//void Ans_parse(struct msgStu *stData)
//{
//    uint16_t sn = (stData->sn[1] << 8) | stData->sn[0] - 1 ;
//    byte sn1 = (sn >> 8) & 0xff;
//    byte sn2 = sn & 0xff;


//    uint16_t i = 0;
//    //修改对应重发列表的 usable
//    while (i < SEND_CMDS_NUM)
//    {
//        if (sendBuf[i][0] > 1 && sn1 == sendBuf[i][2] && sn2 == sendBuf[i][3])
//        {
//            sendBuf[i][0] = 0;
//            break;
//        }
//        i++;
//    }
//}


////指令包解析
//void Ins_parse(struct msgStu *stData)
//{
//    byte i = 0;
//    byte index;
//    byte *mac;
//    uint16_t netId;
//    struct devTable *devtbs = NULL;
//    //byte ctr;
//    // byte *msgStu = (byte *) & (stData->data[0]);
//    switch (stData->data[0])
//    {
//    case 0x10://设备查询
//        //检测控制字
//        if (stData->data[1])
//        {
//            index = 2;
//            if (stData->data[1] & 0x01)
//            {
//                index++;
//            }
//            if (stData->data[1] & 0x02)
//            {
//                index++;
//            }

//            if (stData->data[1] & 0x04)
//            {
//                //通过网络号获取设备netId
//                netId = (uint16_t)((stData->data[index] << 8) | stData->data[index + 1]);
//                //查找netId所在的字节
//                devtbs = getDevTbsByNetId(netId);
//                if (devtbs != NULL)
//                {
//                    Net_send_device(devtbs, DEVTAB_UPDATE, stData->data[1]);
//                }
//                index += 2;
//            }
//            if (stData->data[1] & 0x08)
//            {
//                //通过MAC获取设备

//                //查找MAC所在的字节
//                mac = &stData->data[index];

//                devtbs = getDevTbsByMac(mac);
//                if (devtbs != NULL)
//                {
//                    Net_send_device(devtbs, DEVTAB_UPDATE, stData->data[1]);
//                }
//                index += 4;
//            }

//        }
//        else
//        {
//            i = 0;
//            //更新全部设备表
//            while (i < MAX_DEVTABLE_NUM)
//            {
//                if (devTbs[i].devstate)
//                {
//                    Net_send_device(&devTbs[i], DEVTAB_UPDATE, 0xFF);
//                }
//            }

//        }
//        stData->usable = 0;
//        break;
//    case 0x11://设备更新
//    {

//        index = 2;
//        if (stData->data[1] & 0x01)
//        {
//            index++;
//        }
//        if (stData->data[1] & 0x02)
//        {
//            index++;
//        }

//        if (stData->data[1] & 0x04)
//        {
//            //通过网络号获取设备netId
//            netId = (uint16_t)((stData->data[index] << 8) | stData->data[index + 1]);
//            //查找netId所在的字节
//            devtbs = getDevTbsByNetId(netId);

//            index += 2;
//        }
//        if (stData->data[1] & 0x08)
//        {
//            //只能通过MAC获取设备其他都是不可靠的

//            //查找MAC所在的字节
//            mac = &stData->data[index];
//            devtbs = getDevTbsByMac(mac);

//            index += 4;
//        }
//        //状态设置
//        if (stData->data[1] & 0x10)
//        {
//            if (devtbs != NULL)
//            {
//                //设置对应的状态
//                devtbs->ActSt = (stData->data[index]<<8|stData->data[index+1]);
//            }

//            index += 2;
//        }
//        //当前状态
//        if (stData->data[1] & 0x20)
//        {

//            index += 2;
//        }
//        //设备类型
//        if (stData->data[1] & 0x40)
//        {

//             if (devtbs != NULL)
//            {
//                //设置对应的状态
//                devtbs->type = stData->data[index];
//            }
//            index++;
//        }
//        //设备名称
//        if (stData->data[1] & 0x80)
//        {

//            if (devtbs != NULL)
//            {

//                //设置对应的状态
//                for (i = 0; i < 11; i++)
//                {
//                    devtbs->name[i] = stData->data[index++];
//                }

//            }


//        }


//    }
//    break;
//    }
//}

//// 解析指令cmdS[MAX_CMDS]
///**
//  * @brief  解析指令cmdS[MAX_CMDS]
//  * @param  None
//  * @retval None
//  * @writer liuzhishan
//  *  @modify
//  */
//void parseInstruction(struct msgStu *stData)
//{
//    if (stData->usable != 2)
//        return;


//    if ((stData->sn[0] == 0 && stData->sn[1] == 0)) //心跳sn=0
//    {

//    }
//    else if (stData->sn[1] % 2 == 0)
//    {
//        //偶数为应答信息
//        Ans_parse(stData);

//    }
//    else if (stData->sn[1] % 2)
//    {
//        //返回应答信息
//        Net_Ans(stData);
//        //奇数表示指令信息
//        Ins_parse(stData);

//    }

//    stData->usable = 0;
//}
///**
//  * @brief  解析所有命令
//  * @param  None
//  * @retval None
//  * @writer liuzhishan
//  *   @modify
//  */
//void parseAllCmd()
//{
//    UInt32 i = 0;
//    while (i < RECV_CMDS_NUM)
//    {
//        parseInstruction(&netRecvDataCMDS[i++]);
//    }
//}

///**********************************policy**************************************/
///**
//  * @brief  从策略表中获取可填充策略位置
//  * @param  None
//  * @retval None
//    * @writer liuzhishan
//    *   @modify
//  */
//struct strgytable *getANewDES()
//{
//    UInt32 i = 0;
//    while (i < MAX_DESTABLE_NUM)
//    {
//        if (strategytable[i].usable == 0)
//        {
//            return &strategytable[i];
//        }
//        i++;
//    }
//    return NULL;
//}

////策略解析
//void  policydecisions()
//{
//    UInt32 i = 0;
//    while (i < MAX_DESTABLE_NUM)
//    {
//        if (strategytable[i].usable)
//        {
//            if (strategytable[i].num == 0) //只有一个传感设备、一个执行设备
//            {
//                struct devTable *staDev = getDevTbsByMac(strategytable[i].sensor[0].sensorId);
//                if (staDev->curSt == strategytable[i].sensor[0].sensorTrigger & 0xffffffff)
//                {
//                    struct devTable *actDev = getDevTbsByMac(strategytable[i].actuator[0].actuatorId);
//                    actDev->ActSt = strategytable[i].actuator[0].actuatorTrigger && 0xffffffff;
//                }
//            }
//            else
//            {
//                byte numSta = (strategytable[i].num >> 6) & 0x03;
//                byte type;
//                byte flagTrue = 0;
//                byte j = 0;
//                UInt32 statusValue;
//                //传感器解析
//                while (j < numSta)
//                {

//                    //获取对应的设备
//                    struct devTable *staDev = getDevTbsByMac(strategytable[i].sensor[j].sensorId);

//                    flagTrue = 1;
//                    //触发类型type
//                    type = strategytable[i].type >> (j << 1);

//                    statusValue = ((staDev->ActSt & 0x7FFF) << 16) | staDev->curSt;
//                    //开关型触发
//                    if (1 == type)
//                    {
//                        if (statusValue ^ strategytable[i].sensor[j].sensorTrigger)
//                        {
//                            //不相等
//                            flagTrue = 0;
//                            break;
//                        }
//                    }
//                    //小触发   0b10
//                    else if (2 == type)
//                    {
//                        if (statusValue > strategytable[i].sensor[j].sensorTrigger)
//                        {
//                            flagTrue = 0;
//                            break;
//                        }
//                    }
//                    // 大触发 0b11
//                    else if (3 == type)
//                    {
//                        if (statusValue < strategytable[i].sensor[j].sensorTrigger)
//                        {
//                            flagTrue = 0;
//                            break;
//                        }
//                    }

//                    j++;
//                }
//                // 执行器解析
//                if (flagTrue == 1)
//                {
//                    byte numAct = strategytable[i].num & 0x3f;
//                    UInt32 j = 0;
//                    while (j < numAct)
//                    {
//                        struct devTable *actDev = getDevTbsByMac(strategytable[i].actuator[j].actuatorId);
//                        actDev->ActSt = strategytable[i].actuator[j].actuatorTrigger  & 0x1f;//最多设置5个IO
//                    }
//                }

//            }
//        }

//        i++;
//    }
//}

//void devInit(void)
//{
//    //pwNr = 0;

//    devTbs[1].mac[0] = 10;
//    devTbs[1].mac[1] = 10;
//    devTbs[1].netId = 0x3600;
//    devTbs[1].protocol = 3;
//    devTbs[1].devstate = 1;

//    devTbs[2].mac[0] = 25;
//    devTbs[2].netId = 3600;
//    devTbs[2].protocol = 4;
//    devTbs[2].devstate = 2;

//}
//UInt32 writeFlag = 0;
//byte numTick = 0;
//UInt32 net(void)
//{
//    //  UInt32 curRdNr = 0;
//    //RCC_Configuration();
//    //NVIC_Configuration();
//    //USART1_Configuration();
//    USARTx_Init(USART1);
//    loadDevMsg();


//    while (1)
//    {

//        //将指令从缓冲池中解析出来，放入指令表中
//        parseRcvBufToLst2();
//        //从指令表中取出所有指令 解析 完成 心跳、应答、指令处理
//        parseAllCmd();
//        //策略解析，修改对应 设备表
//        policydecisions();
//        //重发机制
//        sendTimer();
//        Delay(60);
//        Net_PutChar(0xaa);

//    }

//}


//    }//public class net
//}//namespace

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace stm32GateWayDebug
{
    public delegate void byteArrDg(byte[] data, int len = 0);
    public delegate void devTableDg(devTable dev);
	public delegate void msgCmdDg(msgCmd msg);
    
    public class msgCmd
    {
        public byte usable;
        public byte head;//0xE0
        public byte[] sn = new byte[2];//
        public byte len;//data 的长度
        public byte[] data = new byte[100];
        public byte[] crc = new byte[4];
        public byte endl;//0x18

    };
    
    public class netDev
    {
        /**
   * @brief 单条指令存储结构体
     序号  1   2   3   4   5   6
     字节数   1   2   1   N   4   1
     指令头   随机号   数据长度    数据  校验  结束符

     *@parm usable   [netSendDataCMDS]0没有使用 1正在填充 2有效 n 超时递增
                     [netRecvDataCMDS]0没有使用 1正在填充 2有效
     *@parm head 起始符 NET_CMD_HEAD 0xE0
     *@parm sn   0 心跳包 [奇数] 指令包 [偶数] 应答包
     *@parm len  data 数据长度
     *@parm crc  sn+len+data
     *@parm endl 结束符NET_CMD_END0x18
     head    sn      len         Data    CRC         End
     * @writer liuzhishan
     * @modify lishoulei
   */
        //0x56字节

        //IO 模式设置
        public const int IO_MODE_NOUSE = 0;
        public const int IO_MODE_ANALOG_INPUT = 1;
        public const int IO_MODE_GPIO_INPUT = 2;
        public const int IO_MODE_GPIO_OUTPUT_0 = 3;	//实际情况和手册相反
        public const int IO_MODE_GPIO_OUTPUT_1 = 4;	//实际情况和手册相反
        public const int IO_MODE_PULSE_COUNT = 5;

        public const int IO_MODE_CURTAIN_FWD = 0x11;
        public const int IO_MODE_CURTAIN_BWD = 0x12;
        public const int IO_MODE_CURTAIN_PAUSE = 0x13;
        public const int IO_MODE_CURTAIN_STOP = 0x10;

        //IO 接口定义
        const int IO_D0 =0;
        const int IO_D1 =1;
        const int IO_D2 =2;
        const int IO_D3 =3;
        const int IO_D4 =4;

// 每个设备 IO个数
const int DEV_IO_NUM =0x05;
public const int DEV_NAME_SIZE =12;

// 设备类型 netId高字节
// 灯
const int DEV_ACT_LIGHT         =0x0100;
// 插座
const int DEV_ACT_JACK         =0x0200;
// 窗帘
const int DEV_ACT_CURTAIN      =0x7000;
// 开关
const int DEV_SENSOR_SW          =0x8000;
// 人体红外感应
const int DEV_SENSOR_IR          =0x8100;
// 烟雾
const int DEV_SENSOR_SMOKE       =0x8200;
// 燃气
const int DEV_SENSOR_GAS       =0x8300;



const int NET_CMD_HEAD =0xE0;
const int NET_CMD_END =0x18;

// 服务器下发指令
// 查询所有设备
const int NET_CMD_REQ_ALL_DEV   =0x00;
// 查询单个设备信息
const int NET_CMD_REQ_ONE_DEV   =0x02;
// 设置设备名称
const int NET_CMD_SET_DEV_NAME  =0x04;
// 控制设备开关
const int NET_CMD_SET_ON_OFF    =0x08;

// 网关回应服务器指令
// 更新设备信息
const int NET_CMD_DEV_UPDATE    =0x01;

const int NET_CMD_PROCY_UPDATE    =0x20;




        public static byteArrDg sendDataHandle
        {
            get;
            set;
        }
        public static devTableDg updateDevsHandle
        {
            get;
            set;
        }

        const int BUFLEN = 1024;
        private static byte[] lstBuf = new byte[1024];
        static int buf_read = 0;
        static int buf_write = 0;
        private static byte[] recvBuf = new byte[1024];
        private static byte[] sendBuf = new byte[1024];

        public static msgCmdDg parseMsgCmdHandle
        {
            get;
            set;
        }
		
        static void buf_read_backward(int dec)
        {
            while (dec-- != 0)
            {
                if (buf_read == 0)
                    buf_read = (BUFLEN - 1);
                else
                    buf_read--;
            }
        }


        static void buf_read_forward(int inc)
        {

            while (inc-- != 0)
            {
                if (buf_read == (BUFLEN - 1))
                    buf_read = 0;
                else
                    buf_read++;
            }
        }
        public static void parseBufTolst()
        {
            int buf_write_tmp = buf_write;
            //包头检测 若接收到的数据长度小于最小心跳包长度 则不再检测包头
            while ((buf_write_tmp + BUFLEN - buf_read) % BUFLEN > 4)
            {
         //       Console.WriteLine("[parseBufTolst]" + (buf_write_tmp + BUFLEN - buf_read) % BUFLEN);
                //检测到包头
                if (lstBuf[buf_read] == NET_CMD_HEAD)
                {

                    //取得接收数据长度
                    byte[] crcData = new byte[200];
                    buf_read_forward(1);
                    int len_tmp = lstBuf[buf_read];
                    crcData[0] = lstBuf[buf_read];
                    if(len_tmp > 100)
                    {
                        continue;
                    }
                    if ((buf_write_tmp + BUFLEN - buf_read) % BUFLEN > len_tmp + 7)
                    {
                        //接收到完整指令 buf_read+3 代表len 位置  head1+sn2+len1+crc4+end1 =9字节
                        msgCmd pcmdS = new msgCmd();

                        pcmdS.head = NET_CMD_HEAD;
                        pcmdS.len = lstBuf[buf_read ];
                        
                        buf_read_forward(1);
                        pcmdS.sn[0] = lstBuf[buf_read ];
                        crcData[1] = lstBuf[buf_read];
                        buf_read_forward(1);
                        pcmdS.sn[1] = lstBuf[buf_read ];
                        crcData[2] = lstBuf[buf_read];
                        buf_read_forward(1);
                        //指向data
                        // SendCMD((uint8_t)pcmdS);
                        // SendCMD((uint8_t)dat);
                        for (int i = 0; i < (len_tmp); i++)
                        {
                            pcmdS.data[i] = lstBuf[buf_read];
                            crcData[3 + i] = lstBuf[buf_read];
                            buf_read_forward(1);
                        }

                        pcmdS.crc[0] = lstBuf[buf_read ];
                        buf_read_forward(1);
                        pcmdS.crc[1] = lstBuf[buf_read ];
                        buf_read_forward(1);
                        pcmdS.crc[2] = lstBuf[buf_read ];
                        buf_read_forward(1);
                        pcmdS.crc[3] = lstBuf[buf_read ];
                        buf_read_forward(1);
                        //
                        pcmdS.endl = NET_CMD_END;
                        buf_read_forward(1);

                        //Crc323.Reset();
                        //Crc323.Crc(pcmdS.sn[0]);
                        //Crc323.Crc(pcmdS.sn[1]);
                        //Crc323.Crc(pcmdS.data, 0, pcmdS.len);
                        //UInt32 crc32 = Crc323.Value;
                        ////CRC校验
                        //psndat = &pcmdS.sn[0];
                        //CRC_ResetDR();
                        //for (i = 0; i < len_tmp + 3; i++)
                        //{
                        //    CRC_CalcCRC((uint32_t)*psndat++);
                        //}
                        UInt32 crc_calc = crc32.stm32crc32(crcData, pcmdS.len + 3);

                        UInt32 crc = (UInt32)((pcmdS.crc[0] <<24) | (pcmdS.crc[1] <<16) | (pcmdS.crc[2] << 8) | pcmdS.crc[3]);


                       if (crc_calc == crc)
                       {
                            if(parseMsgCmdHandle != null)
                            parseMsgCmdHandle(pcmdS);
                            if(pcmdS.len > 0)
                            {
                                parseMsg_Data(pcmdS.data);
                            }
                       }

                       //else
                       //    Console.WriteLine("[parseBufTolst] crc check error! local crc : {0:x}\tnet crc : {1:x} ", crc32, crc);
                    }
                    else
                    {
                        buf_read_backward(1);
                    }
                }
                else
                {
                    buf_read_forward(1);
                }
                buf_write_tmp = buf_write;
            }//end while
        }
		public static void getBuf(byte[] buf,int len)
        {
			for(int i = 0;i< len;i++)
            {
                lstBuf[buf_write] = buf[i];
                buf_write++;
                if (buf_write >= BUFLEN)
                    buf_write = 0;
            }
        }

       static UInt16 msgSN = 0;
       static UInt16 getmsgSN()
        {

            if (msgSN % 2 == 1)
            {
                // 奇数
                msgSN ++;
            }
            else
            {
                msgSN+=2;
            }
            return msgSN;
            //return ((byte)((msgSN >> 8) & 0xff)).ToString("X2") + " " + ((byte)(msgSN & 0xff)).ToString("X2");// ((short)(msgSN >> 12) & 0x0f).ToString("X") + ((short)(msgSN >> 8) & 0x0f).ToString("X") + " " + ((short)(msgSN >> 4) & 0x0f).ToString("X") + ((short)(msgSN & 0x0f)).ToString("X");
        }

        public static void setMsgSN(UInt16 sn,byte[] data)
        {
            data[2] = (byte)((sn >> 8) & 0xff);
            data[3] = (byte)(sn & 0xff);
        }
        public static void setMsgSN(byte[] data)
        {
            UInt16 sn = getmsgSN();
            data[2] = (byte)((sn >> 8) & 0xff);
            data[3] = (byte)(sn & 0xff);
        }
		public static byte[] getAllDevs()
        {
            byte[] btMsg = new byte[] { 0xE0, 0x01, 0, 0x09, NET_CMD_REQ_ALL_DEV, 0x00, 0x00, 0x00, 0x00, NET_CMD_END };
            UInt16 sn = getmsgSN();
            btMsg[2] = (byte)((sn >> 8)&0xff);
            btMsg[3] = (byte)(sn & 0xff);
            crc32.setCrc32Value(btMsg);
            return btMsg;
        }

        public static void parseMsg_Data(byte[] data)
        {
            switch (data[0])
            {
                case 0x01://设备更新
                    {
                        if (data[1] != 0)
                        {
                            devTable dev = null;
                            byte state = 0;
                            byte protocol = 0;
                            UInt16 netId = 0;



                            int index = 1;
                            
                            state = data[index];
                            index++;//state      byte[1]

                            protocol = data[index];
                            index++;//protocol   byte[1]

                            netId = (UInt16)((data[index] << 8) | data[index + 1]);
                            index += 2;//netId   byte[2]

                            //通过MAC获取设备
                            dev = devMgr.getDev(data, index);
                            if (dev != null)
                            {
                                    
                            }
                            else
                            {
                                dev = new devTable();
                                dev.mac = getMac(data, index);
                                devMgr.addDev(dev);
                            }
                            index += 8;   //mac   byte[8]

                            dev.devstate = state;
                            dev.protocol = protocol;
                            dev.netId = netId;


                            dev.io = data[index];
                            index++;      //IO    byte[1]

                            dev.ioMode = data[index];
                            index++;

                            dev.curSt = (UInt16)((data[index] << 8) | data[index + 1]);
                            index += 2;   //curSt byte[2]
                            

                            dev.name = "";
                            for (int i = 0; i < DEV_NAME_SIZE; i++)
                            {
                                dev.name += data[index + i].ToString("X2") + " ";
                            }
                            index += DEV_NAME_SIZE;  //
                            
                            if (updateDevsHandle != null)
                                updateDevsHandle(dev);
                            //dev.show();
                        }
                    }
                    break;
            }
        }

        public static void sendDataToGateway(byte[] data)
        {
            data[0] = NET_CMD_HEAD;
            data[1] = (byte)(data.Length - 9);
            UInt16 sn = getmsgSN();
            data[2] = (byte)((sn >> 8) & 0xff);
            data[3] = (byte)(sn & 0xff);
            crc32.setCrc32Value(data);
            if (sendDataHandle != null)
                sendDataHandle(data);
          //  MainWindow.sendMsgToSerHandle(data);
            
        }
        public static void setMac(UInt64 mac ,byte[] data ,int offset = 0)
        {
            for (int i = offset; i < 8 + offset;i++ )
            {
                data[i] = (byte)((mac << (7 - i) * 8) & 0xff);
            }
        }
        public static string getMac(byte[] mac, int offset)
        {
            string strMac = "";
            for(int i = 0;i< 8 ;i++)
            {
                strMac += mac[offset + i].ToString("X2") ;
            }
           // macValue = (UInt64)(mac[offset + 0] | (mac[offset + 1] << 8) | (mac[offset + 2] << 16) | (mac[offset + 3] << 24) | (mac[offset + 4] << 32) | (mac[offset + 5] << 40) | (mac[offset + 6] << 48) | (mac[offset + 7] << 56));
            return strMac;
        }

        public static byte[] getByteMac(string mac)
        {
            try
            {
                if (mac.Length != 8)
                    return null;
                byte[] macByte = new byte[8];
                for (int i = 0; i < 8; i++)
                {
                    macByte[i] = Convert.ToByte(mac[i] + "", 16);
                }
                return macByte;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return null;
            }
        }

        public static int getData(UInt64 mac,UInt16 actSt,byte type,byte[] name,byte[] data)
        {
            int index = 4;
            data[0] = 0xe0;
            setMsgSN(data);
            data[4] = 0x01;
            //mac
            for (int i = 0; i < 8;i++ )
            {
                data[index + i] = (byte)((mac >> ((7 - i)*8))&0xff);
            }
            index += 8;
            //actSt
            data[index] = (byte)((actSt >> 8) & 0xff);
            data[index+1] = (byte)(actSt & 0xff);
            index += 2;
            //type
            data[index] = type;
            index++;
            //name
            for (int i = 0; i < 10;i++ )
            {
                data[index + i] = name[i];
            }
            index += 10;
            data[1] = (byte)(index - 4);
            crc32.setCrc32Value(data, 1, index - 1);
            index += 4;
            data[index] = NET_CMD_END;
            
                return index + 1;
        }
    }
}

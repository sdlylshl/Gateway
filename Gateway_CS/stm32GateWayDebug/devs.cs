using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace stm32GateWayDebug
{
    public class  devTable
    {
        public byte     devstate;
        public byte     protocol;
        public UInt16   netId;
        public string     mac;
        public byte     io;
        public byte ioMode;
        public UInt16   curSt;
        public byte     type;
        public string   name;
        public devTable()
        {

        }
        public devTable(string mac)
        {
            this.mac = mac;
        }

        public byte[] getMacByteArr()
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

        public int IOMode
        {
            set
            {
                byte[] data = new byte[1 + 19];// 1(0x0e) 1(len) 2(sn) 1(0x08) 8(mac) 1(io) 1(ioMode) 4(crc) 1(0x18)
                byte[] msgData = new byte[1]{(byte)value};
                paddingAddr(data, 0x08, msgData);
            }
        }

        public byte[] Name
        {
            set
            {
                byte[] data = new byte[netDev.DEV_NAME_SIZE + 19];// 1(0x0e) 1(len) 2(sn) 1(0x04) 8(mac) 1(io) netDev.DEV_NAME_SIZE(name) 4(crc) 1(0x18)
                paddingAddr(data, 0x04, value);
            }
        }

        public void query()
        {
            byte[] data = new byte[1 + 18];// 1(0x0e) 1(len) 2(sn) 1(0x04) 8(mac) 1(io) 4(crc) 1(0x18)
            paddingAddr(data, 0x02, null);
        }

        //public void copyFrom(devTable dev)
        //{
        //    devstate = dev.devstate;
        //    protocol = dev.protocol;
        //    netId = dev.netId;
        //    mac = dev.mac;
        //    mac.
        //}

        private void paddingAddr(byte[] data,byte cmdType,byte[] msgData)
        {
            try
            {
                data[0] = 0xe0;
                netDev.setMsgSN(data);
                byte index = 4;
                data[index++] = cmdType;//指令类型 0x00 获取全部设备 0x02 查询设备信息 0x04 设置设备名称 0x08 设置设备IO设置
                for (int i = 0; i < 8; i++)
                {
                    data[index + i] = Convert.ToByte(mac[2 * i] + "" + mac[2 * i + 1], 16);
                }
                index += 8;
                data[index++] = io;
                switch (cmdType)
                {
                    case 0x00:
                        //a)        [服务器—>网关]获取全部设备     00           ([00])
                        break;
                    case 0x02://query
                        //b)        [服务器—>网关]查询设备信息     02          ([02] [MAC] [Ion])
                        {

                        }
                        break;
                    case 0x04://name
                        //c)         [服务器—>网关]设置设备名称    04           ([04] [MAC] [Ion] [name])
                        {
                            for (int i = 0; i < netDev.DEV_NAME_SIZE && i < msgData.Length; i++)
                            {
                                data[index + i] = msgData[i];
                            }
                            index += netDev.DEV_NAME_SIZE;
                        }
                        break;
                    case 0x08://IO
                        //d)        [服务器—>网关]设置设备IO设置    08           ([08] [MAC] [Ion] [operate][ iomode])
                        {
                            data[index++] = msgData[0];
                        }
                        break;
                }
                data[1] = (byte)(index - 4); // netDev.DEV_NAME_SIZE + 10
                crc32.setCrc32Value(data);
                index += 4;
                data[index] = 0x18;

                MainWindow.sendDataToCmdAndSendDirectCtrl(data);
            }
            catch(Exception ex)
            {
                Console.WriteLine("[devTable.paddingAddr] " + ex.Message);
            }
        }
        public void show()
        {
           // Console.WriteLine("Mac      : " + mac.ToString("X8"));
            //Console.WriteLine("devstate : " + devstate.ToString("X2"));
            //Console.WriteLine("protocol : " + protocol.ToString("X2"));
            //Console.WriteLine("netId    : " + netId.ToString("X4"));
            //Console.WriteLine("ActSt    : " + ActSt.ToString("X4"));
            //Console.WriteLine("curSt    : " + curSt.ToString("X4"));
            //Console.WriteLine("type     : " + type.ToString("X2"));
            //Console.WriteLine("name     : " + name);
        }
    };
    public class devMgr
    {
        static Dictionary<string, devTable> devLst = new Dictionary<string, devTable>();

        public static void init()
        {
            //for(int i = 0;i < 8;i++)
            //{
            //    devTable dev = new devTable( i + 0xddeeff00);
            //    devLst.Add(dev.mac, dev);
            //}
        }
        public static List<devTable> getAllDevs()
        {
            List<devTable> list = new List<devTable>(devLst.Values);
            return list;
        }

        public static devTable  getDev(string mac)
        { 
            if (devLst.ContainsKey(mac))
            {
                return devLst[mac];
            }
            else
                return null;
        }
        public static devTable getDev(byte[] mac)
        {
           // UInt64 macValue = (UInt64)( mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24) | (mac[4] << 32) | (mac[5] << 40) | (mac[6] << 48) | (mac[7] << 56));
            return getDev(mac,0);
        }
        public static devTable getDev(byte[] mac,int offset)
        {
            string macValue = netDev.getMac(mac,offset);// (UInt64)(mac[offset + 0] | (mac[offset + 1] << 8) | (mac[offset + 2] << 16) | (mac[offset + 3] << 24) | (mac[offset + 4] << 32) | (mac[offset + 5] << 40) | (mac[offset + 6] << 48) | (mac[offset + 7] << 56));
            //for (int i = 0; i < 8;i++ )
            //{
            //    macValue |= (UInt64)((mac[offset + i] << (7 - i) * 8) & 0xff);
            //}

                return getDev(macValue);
        }
        public static bool addDev(devTable dev)
        {
            if(devLst.ContainsKey(dev.mac))
            {
                return false;
            }
            else
            {
                devLst.Add(dev.mac, dev);
                Console.WriteLine("[addDev]" + dev.mac);
                return true;
            }
        }

        public static void clear()
        {
            devLst.Clear();
        }
    }
}

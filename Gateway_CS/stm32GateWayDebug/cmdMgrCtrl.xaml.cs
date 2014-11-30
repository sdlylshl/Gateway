using System;
using System.Windows;
using System.Windows.Controls;

namespace stm32GateWayDebug
{
    /// <summary>
    /// cmdMgrCtrl.xaml 的交互逻辑
    /// </summary>
    public partial class cmdMgrCtrl : UserControl
    {
        byte[] sendBuf;
        string sendBufStr;
        public cmdMgrCtrl()
        {
            InitializeComponent();
            netDev.sendDataHandle = sendMsgToTxtCmd;
            netDev.parseMsgCmdHandle += parseMsgCmdInvoke;
        }

        public void parseMsgCmdInvoke(msgCmd msg)
        {
            this.Dispatcher.BeginInvoke(new Action(() =>
            {
                parseMsgCmdFun(msg);
            }));
        }
        private void parseMsgCmdFun(msgCmd msg)
        {
            string str = msg.head.ToString("X2") + " " + msg.len.ToString("X2") + " " + msg.sn[0].ToString("X2") + " " + msg.sn[1].ToString("X2") + " ";
            for (int i = 0; i < msg.len; i++)
            {
                str += msg.data[i].ToString("X2") + " ";
            }
            str += msg.crc[0].ToString("X2") + " ";
            str += msg.crc[1].ToString("X2") + " ";
            str += msg.crc[2].ToString("X2") + " ";
            str += msg.crc[3].ToString("X2") + " ";
            str += "18";
            addLog("<<<< " + str + Environment.NewLine);
            if (msg.sn[1] % 2 == 1)
            {
                byte[] data = new byte[] { 0xe0, 0, 0, 0, 0, 0, 0, 0, 0x18 };

                data[2] = msg.sn[0];
                data[3] = msg.sn[1];
                crc32.setCrc32Value(data);
                linkMgr.sendData(data,data.Length);
                str = "";
                for (int i = 0; i < data.Length;i++ )
                {
                    str += data[i].ToString("X2") + " ";
                }
                addLog(">>>> " + str + Environment.NewLine);
            }
        }


        private void btnClearLog_Click(object sender, RoutedEventArgs e)
        {
            txtLog.Text = "";
        }

        private void txtCmd_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (txtCmd.Text.Trim().Length == 0)
                btnSendCmd.IsEnabled = false;
            else
                btnSendCmd.IsEnabled = true;
        }

        private byte[] getTxtDataValue()
        {
            byte[] data;
            if (txtData.Text.Trim().Length != 0)
            {
                string[] strValue = txtData.Text.Trim().Split(' ');
                data = new byte[strValue.Length];
                for (int i = 0; i < data.Length; i++)
                {
                    try
                    {
                        data[i] = Convert.ToByte(strValue[i], 16);
                    }
                    catch
                    {
                        MessageBox.Show("消息格式错误！");
                        return null;
                    }
                }
                return data;
            }
            else
            {
                return null;
            }
        }
        private void txtData_TextChanged(object sender, TextChangedEventArgs e)
        {
            byte[] data = getTxtDataValue();
            if(data != null)
            { 
                byte[] crcArr = crc32.getByteArrCrc32(data);
                txtCmd.Text = "E0 " + txtData.Text.Trim() + " " + crcArr[0].ToString("X2") + " " + crcArr[1].ToString("X2") + " " + crcArr[2].ToString("X2") + " " + crcArr[3].ToString("X2") + " 18";
            }
        }

        private void btnSendCmd_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string strCmd = txtCmd.Text.Trim();
                if (strCmd.Substring(0, 8).ToUpper() == "CRC32 : ")
                {
                    string[] strValue = strCmd.Split(' ');
                    UInt32[] data = new UInt32[strValue.Length - 2];
                    for (int i = 0; i < strValue.Length - 2; i++)
                    {
                        try
                        {
                            data[i] = Convert.ToUInt32(strValue[i + 2], 16);
                        }
                        catch
                        {
                            MessageBox.Show("参数错误！\r\n正确的参数格式为:\r\n\"CRC32 : \"");
                            return;
                        }
                    }
                    UInt32 crc32Value = crc32.stm32crc32(data, data.Length);
                    txtLog.AppendText(strCmd + Environment.NewLine);
                    txtLog.AppendText("CRC32 : " + crc32Value.ToString("X8") + Environment.NewLine);
                }
                else if (strCmd.Substring(0, 2).ToUpper() == "E0")
                {
                    string[] strValue = strCmd.Split(' ');
                    byte[] data = new byte[strValue.Length];
                    for (int i = 0; i < data.Length;i ++ )
                    {
                        try
                        {
                            data[i] = Convert.ToByte(strValue[i], 16);
                        }
                        catch
                        {
                            MessageBox.Show("消息格式错误！");
                            return;
                        }
                    }
                        sendMsgToSer(data);
                }
                else
                {
                    throw new Exception();
                }
            }
            catch
            {
                MessageBox.Show("指令错误");
            }

        }

        public void getAllDevs()
        {
            sendBuf = netDev.getAllDevs();// new byte[] { 0xE0, 0x02, 0, 0x09, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18 };
            sendMsgToTxtCmd(sendBuf);
            
            //sendBufStr = "";
            //for (int i = 0; i < sendBuf.Length; i++)
            //{
            //    sendBufStr += sendBuf[i].ToString("X2") + " ";
            //}
            //txtCmd.Text = sendBufStr;
        }

        public void clearAllDevs()
        {

        }

        byte[] dataSend;
        public void sendMsgToTxtCmd(byte[] data, int len = 0)
        {
            if (len == 0)
                len = data.Length;
            if(len == 0)
            {
                MessageBox.Show("data error!");
                return;
            }
            dataSend = new byte[len - 6];
            for (int i = 1; i < len - 5; i++)
            {
                dataSend[i - 1] = data[i];
            }

            sendBufStr = "";
            for (int i = 0; i < dataSend.Length; i++)
            {
                sendBufStr += dataSend[i].ToString("X2") + " ";
            }
            txtData.Text = sendBufStr;
           // txtCmd.Text = sendBufStr;
        }

        public void sendMsgToTxtCmdAndSendDirect(byte[] data, int len = 0)
        {
            sendMsgToSer(data);
            sendMsgToTxtCmd(data, len);
        }


        public void sendMsgToSer(byte[] buf, int len = 0)
        {
            if (len == 0)
                len = buf.Length;
            if (buf.Length == 0)
                MessageBox.Show("内容为空");
            linkMgr.sendData(buf, len);
            sendBufStr = "";
            for (int i = 0; i < len; i++)
            {
                sendBufStr += buf[i].ToString("X2") + " ";
            }
            addLog(">>>> " + sendBufStr + Environment.NewLine);
            return;
        }


        public void addLog(string str)
        {
            txtLog.AppendText(str);
            logFileMgr.addLog(str);
        }
    }
}

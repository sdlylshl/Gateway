using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Threading;

namespace stm32GateWayDebug
{
    /// <summary>
    /// userDevManagerWindow.xaml 的交互逻辑
    /// </summary>
    public partial class userDevManagerWindow : Window
    {
        public userDevManagerWindow()
        {
            InitializeComponent();
        }

        public void update(devTable dev)
        {
            switch (dev.netId >> 8)
            {
                case 0x01://"灯";
                    灯.updateValue(dev);
                    break;
                case 0x02://"插座";

                    break;
                case 0x03://"窗帘";
                    {
                        窗帘.updateValue(dev);
                    }
                    break;
                case 0x04://"门锁";
                    {
                        门锁.updateValue(dev);
                    }
                    break;
                case 0x05://"警报器";
                    {
                        警报器.updateValue(dev);
                    }
                    break;

                case 0x80://"开关";

                    break;
                case 0x81://"人体";
                    if(dev.netId == 0x8101)
                    {
                        客厅感应.updateValue(dev);
                    }
                    else
                    {
                        卧室感应.updateValue(dev);
                    }
                    break;
                case 0x82://"烟感";
                   烟感.updateValue(dev);
                    break;
                case 0x83://"燃气";
                   燃气.updateValue(dev);
                    break;
                default:

                    break;

            }
        }


        private void btnRestMode_Click(object sender, RoutedEventArgs e)
        {
            E_ModeMgr.curMode = E_ModeMgr.modeType.MODE_Rest;
        }

        private void btnLeaveMode_Click(object sender, RoutedEventArgs e)
        {
            E_ModeMgr.curMode = E_ModeMgr.modeType.MODE_LEAVE;
        }

        private void btnHomeMode_Click(object sender, RoutedEventArgs e)
        {
            E_ModeMgr.curMode = E_ModeMgr.modeType.MODE_HOME;
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Escape)
            {
                linkMgr.clientSocket.Close();
                Console.WriteLine("关闭客户端");
                Thread.Sleep(50);
                Environment.Exit(0);
            }
        }

        private void Window_Unloaded(object sender, RoutedEventArgs e)
        {
            
            linkMgr.clientSocket.Close();
            Console.WriteLine("关闭客户端");
            Thread.Sleep(50);
            Environment.Exit(0);
        }
    }
}

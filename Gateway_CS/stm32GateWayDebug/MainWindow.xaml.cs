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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace stm32GateWayDebug
{

    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    /// 

    public enum devTYPE
    {
        LIGHT,
        SWITCH,
        RANQI,
        YANWU,
        RENTI

    }

    public partial class MainWindow : Window
    {
        private Dictionary<string, string> newsInfo = new Dictionary<string, string>()
        {
            {"allDevs","E0 00 00 02 10 00 00 00 00 00 18"}
        };
        public static byteArrDg sendDataToCmdCtrl;
        public static byteArrDg sendDataToCmdAndSendDirectCtrl;
        userDevManagerWindow userWindow = new userDevManagerWindow();
        public MainWindow()
        {
            InitializeComponent();
            this.Visibility = Visibility.Hidden;
            crc32.crc32Init();
            netDev.updateDevsHandle = refreshCtr;
            sendDataToCmdCtrl = ctrlCmdMgr.sendMsgToTxtCmd;
            sendDataToCmdAndSendDirectCtrl = ctrlCmdMgr.sendMsgToTxtCmdAndSendDirect;
            userWindow.Show();
        }

         void refreshCtr(devTable devObj)
        {
            this.Dispatcher.BeginInvoke(new devTableDg(refreshDevCtrls), devObj);
        }

        void refreshDevCtrls(devTable dev)
        {
            devCtrls.update(dev);
            userWindow.update(dev);
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

 

        private void btnGetAllDevs_Click(object sender, RoutedEventArgs e)
        {
            ctrlCmdMgr.getAllDevs();
        }

        private void btnCrc32_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
            crc32Mgr crc32MgrCtrl = new crc32Mgr();
            crc32MgrCtrl.backToMainHandle = this.Show;
            crc32MgrCtrl.Show();
        }

        private void btnClearAllDevs_Click(object sender, RoutedEventArgs e)
        {
            devMgr.clear();
            devCtrls.refreshDevs();


        }

        private void btnModeSet_1_Click(object sender, RoutedEventArgs e)
        {
            E_ModeMgr.curMode = E_ModeMgr.modeType.MODE_HOME;

        }
        private void btnModeSet_2_Click(object sender, RoutedEventArgs e)
        {
            E_ModeMgr.curMode = E_ModeMgr.modeType.MODE_LEAVE;

        }
        private void btnModeSet_3_Click(object sender, RoutedEventArgs e)
        {
            E_ModeMgr.curMode = E_ModeMgr.modeType.MODE_Rest;

        }
        private void btnModeSet_4_Click(object sender, RoutedEventArgs e)
        {
            E_ModeMgr.curMode = E_ModeMgr.modeType.MODE_DEF;

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

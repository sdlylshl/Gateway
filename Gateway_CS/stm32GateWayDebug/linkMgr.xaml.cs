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
using System.Net;
using System.Net.Sockets;
using System.Threading;


namespace stm32GateWayDebug
{
    /// <summary>
    /// linkMgr.xaml 的交互逻辑
    /// </summary>
    public partial class linkMgr : Window
    {
        public static Socket clientSocket;
        static int port = 0;
        IPAddress ip;
        public linkMgr()
        {
            InitializeComponent();
            crc32.crc32Init();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            ip = IPAddress.Parse(txtClnIp.Text.Trim());
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                port = Int32.Parse(txtClnPort.Text.Trim());
                clientSocket.Connect(new IPEndPoint(ip, port)); //配置服务器IP与端口  
                Console.WriteLine("连接服务器成功");
                Button btn = sender as Button;
                btn.IsEnabled = false;
                //tabLink.SelectedItem = tbmDev;
                //tbmDev.IsEnabled = true;

                MessageBox.Show("连接服务器成功");
                MainWindow mainWin = new MainWindow();
                userDevManagerWindow userWin = new userDevManagerWindow();
                this.Hide();
                mainWin.Show();
                //userWin.Show();
                //userWin.Hide();

                Thread myThread = new Thread(receiveDataToBuf);
                myThread.Start();
                Thread thrdBufLst = new Thread(parseBufData);
                thrdBufLst.Start();
            }
            catch
            {
                Console.WriteLine("连接服务器失败，请按回车键退出！");
                MessageBox.Show("连接服务器失败，请检查网络连接！");
                return;
            }
        }

        /// <summary>  
        /// 监听客户端连接  
        /// </summary>  
        private void receiveDataToBuf()
        {
            byte[] recvBuf = new byte[1024];
            while (true)
            {
                try
                { 
                    int receiveLength = clientSocket.Receive(recvBuf);
                    netDev.getBuf(recvBuf, receiveLength);
                    }
                catch(Exception ex)
                {

                    Console.WriteLine("<----> " + ex.ToString());
                    Thread.Sleep(1000);
                    try
                    {
                        clientSocket.Connect(new IPEndPoint(ip, port)); //配置服务器IP与端口  
                    }
                    catch
                    {
                        Console.WriteLine("<====> relink error!");
                    }
                }
            }
        }

        private void parseBufData()
        {
            while (true)
            {
                netDev.parseBufTolst();
            }
        }

        public static void sendData(byte[] buffer, int size)
        {
            try
            {
                if (clientSocket.Connected)
                {
                    clientSocket.Send(buffer, size, 0);
                    if(buffer[4] == 0x08)
                        Console.WriteLine("[sendData]" + buffer[4].ToString("X2"));
                }
                else
                    MessageBox.Show("连接已断开");
            }
            catch(Exception ex)
            {
                MessageBox.Show("link error! " + ex.Message);
            }
        }

        private void btnCrc32_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
            crc32Mgr crc32MgrCtrl = new crc32Mgr();
            crc32MgrCtrl.backToMainHandle = this.Show;
            crc32MgrCtrl.Show();
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape)
            {
                clientSocket.Close();
                Console.WriteLine("关闭客户端");
                Thread.Sleep(50);
                Environment.Exit(0);
            }
        }

        private void Window_Unloaded(object sender, RoutedEventArgs e)
        {
            clientSocket.Close();
            Console.WriteLine("关闭客户端");
            Thread.Sleep(50);
            Environment.Exit(0);
        }

    }
}

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

namespace stm32GateWayDebug
{
    public enum performType
    {
          灯,
          警报器,
          窗帘,
          门锁
    }

    /// <summary>
    /// performCtrl.xaml 的交互逻辑
    /// </summary>
    public partial class performCtrl : UserControl
    {

        devTable obj = new devTable();
        public performCtrl()
        {
            InitializeComponent();
            Usable = false;
            
        }
        public bool Usable
        {
            get
            {
                return lbUsable.Visibility == Visibility.Hidden;
            }
            set
            {
                lbUsable.Visibility = value ? Visibility.Hidden : Visibility.Visible;
            }
        }

        performType _type;
        public performType Type
        {
            get
            {
                return _type;
            }
            set
            {
                switch (value)
                {
                    case performType.窗帘:
                        {
                            lbDis.Content = "窗帘";
                        }
                        break;
                    case performType.灯:
                        {
                            lbDis.Content = "灯";
                            btnStop.Visibility = Visibility.Hidden;
                            btnBack.Content = "开";
                            btnFore.Content = "关";
                        }
                        break;
                    case performType.警报器:
                        {
                            lbDis.Content = "警报器";
                            btnBack.Visibility = Visibility.Hidden;
                            btnStop.Visibility = Visibility.Hidden;
                            btnFore.Visibility = Visibility.Hidden;
                        }
                        break;
                    case performType.门锁:
                        {
                            lbDis.Content = "门锁";
                            btnStop.Visibility = Visibility.Hidden;
                            btnBack.Content = "开";
                            btnFore.Content = "关";
                        }
                        break;
                }
                _type = value;
            }
        }

        public void updateValue(devTable dev)
        {
            obj = dev;
            Usable = true;
            switch (dev.netId >> 8)
            {
                case 0x01://"灯"
                case 0x04://"门锁";
                case 0x05://"警报器";
                    {
                        if(dev.curSt > 0)
                        {
                            cvsMain.Background = Brushes.Red;
                            //btnBack.IsEnabled = false;
                            //btnFore.IsEnabled = true;
                        }
                        else
                        {
                            cvsMain.Background = Brushes.Transparent;
                            //btnBack.IsEnabled = true;
                            //btnFore.IsEnabled = false;
                        }
                    }
                    break;
                case 0x02://"插座";
                    {

                    }
                    break;
                case 0x03://"窗帘";
                    {

                    }
                    break;
                case 0x80://"开关";
                    {

                    }
                    break;
                default:
                    break;

            }
        }

        public performCtrl(performType type)
        {
            InitializeComponent();
            Type = type;
        }

        private void btnBack_Click(object sender, RoutedEventArgs e)
        {
            switch(Type)
            {
                case performType.窗帘:
                    {
                        obj.IOMode = netDev.IO_MODE_CURTAIN_FWD;
                    }
                    break;
                case performType.灯: //开灯
                case performType.门锁:
                case performType.警报器:
                    {
                        try
                        {
                           obj.IOMode = netDev.IO_MODE_GPIO_OUTPUT_1;
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show("btnOn_Click() : " + ex.Message);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        private void btnStop_Click(object sender, RoutedEventArgs e)
        {
            switch (Type)
            {
                case performType.窗帘:
                    {
                        obj.IOMode = netDev.IO_MODE_CURTAIN_PAUSE;
                       // obj.IOMode = netDev.IO_MODE_CURTAIN_STOP;
                    }
                    break;
                case performType.灯:
                    {

                    }
                    break;
                case performType.警报器:
                    {

                    }
                    break;
                case performType.门锁:
                    {

                    }
                    break;
                default:
                    break;
            }
        }

        private void btnFore_Click(object sender, RoutedEventArgs e)
        {
            switch (Type)
            {
                case performType.窗帘:
                    {

                        obj.IOMode = netDev.IO_MODE_CURTAIN_BWD;
                    }
                    break;
                case performType.灯:
                case performType.警报器:
                case performType.门锁:
                    {
                        try
                        {
                           obj.IOMode = netDev.IO_MODE_GPIO_OUTPUT_0;
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show("btnOff_Click() : " + ex.Message);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

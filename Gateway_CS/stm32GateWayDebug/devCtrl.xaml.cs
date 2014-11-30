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
    /// <summary>
    /// devCtrl.xaml 的交互逻辑
    /// </summary>
    public partial class devCtrl : UserControl
    {
        public devCtrl()
        {
            InitializeComponent();
        }
        public devTable devObj = null;
        public devCtrl(devTable dev)
        {
            InitializeComponent();
            update(dev);
        }

        public void update(devTable dev)
        {
            if (dev != null)
                devObj = dev;
            else
                throw new Exception("设备对象为空");
            txtState.Text = dev.devstate.ToString("X2");
            txtProtocol.Text = dev.protocol.ToString("X2");
            txtNetId.Text = dev.netId.ToString("X4");
            txtMac.Text = dev.mac;
            txtIO.Text = dev.io.ToString("X2");
            txtIOMode.Text = dev.ioMode.ToString("X2");
            txtCurSt.Text = dev.curSt.ToString("X4");
            txtType.Text = dev.type.ToString("X2");
            txtName.Text = dev.name;
            if(dev.io == 0)
            {
                btnOn.Visibility = Visibility.Hidden;
                btnOff.Visibility = Visibility.Hidden;
            }
            else if((devObj.netId & 0x8000) > 0)
            {
                btnOn.Visibility = Visibility.Hidden;
                btnOff.Visibility = Visibility.Hidden;
            }

            if(dev.curSt == 0)
            {
                btnOff.IsEnabled = false;
                btnOn.IsEnabled = true;
            }
            else if(dev.curSt == 1)
            {
                btnOff.IsEnabled = true;
                btnOn.IsEnabled = false;
            }
            btnStop.Visibility = Visibility.Hidden;
            switch(dev.netId >> 8)
            {
                case 0x01:
                    lbDevName.Content = "灯";
                    break;
                case 0x02:
                    lbDevName.Content = "插座";
                    break;
                case 0x03:
                    {
                        lbDevName.Content = "窗帘";
                        btnOn.Content = "FWD";
                        btnOff.Content = "BWD";
                        btnStop.Visibility = Visibility.Visible;
                    }
                    break;
                case 0x04:
                    {
                        lbDevName.Content = "门锁";
                    }
                    break;
                case 0x05:
                    {
                        lbDevName.Content = "警报器";
                    }
                    break;

                case 0x80:
                    lbDevName.Content = "开关";
                    break;
                case 0x81:
                    lbDevName.Content = "人体";
                    break;
                case 0x82:
                    lbDevName.Content = "烟感";
                    break;
                case 0x83:
                    lbDevName.Content = "燃气";
                    break;
                default:

                    break;

            }

           // lbDevName.Content = dev.mac.ToString("X8");

        }

        public bool focus
        {
            get
            {
                return bdFocus.Visibility == Visibility.Visible;
            }
            set
            {
                bdFocus.Visibility = value ? Visibility.Visible : Visibility.Hidden;
            }
        }

        

        devTYPE _dev = devTYPE.LIGHT;
        public devTYPE devType
        {
            get
            {
                return  _dev;
            }
            set
            {
                switch(value)
                {
                    case devTYPE.RENTI:
                        {
                           // imgDev.Source = ((Image)this.Resources["imgrenti"]).Source;
                            lbDevName.Content = "人体";
                        }
                        break;
                    case devTYPE.LIGHT:
                        {
                          //  imgDev.Source = ((Image)this.Resources["imglight_off"]).Source;
                            lbDevName.Content = "灯";
                        }
                        break;
                    case devTYPE.RANQI:
                        {
                           // imgDev.Source = ((Image)this.Resources["imgranqi"]).Source;
                            lbDevName.Content = "燃气";
                        }
                        break;
                    case devTYPE.SWITCH:
                        {
                          //  imgDev.Source = ((Image)this.Resources["imgSwitch_off"]).Source;
                            lbDevName.Content = "开关";
                        }
                        break;
                    case devTYPE.YANWU:
                        {
                          //  imgDev.Source = ((Image)this.Resources["imgyanwu"]).Source;
                            lbDevName.Content = "烟雾";
                        }
                        break;
                    default:
                        return;
                }
                _dev = value;
            }

        }

        private void btnName_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string[] name = txtName.Text.ToString().Trim().Split(' ');
                byte[] dataName = new byte[netDev.DEV_NAME_SIZE];
                for (int i = 0; i < name.Length && i < dataName.Length;i++ )
                {
                    dataName[i] = Convert.ToByte(name[i], 16);
                }

                devObj.Name = dataName;
            }
            catch(Exception ex)
            {
                MessageBox.Show("btnName_Click() : " + ex.Message);
            }
        }

        private void btnOn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if ((devObj.netId & 0xff00) == 0x7000)
                    devObj.IOMode = netDev.IO_MODE_CURTAIN_FWD;
                else
                    devObj.IOMode = netDev.IO_MODE_GPIO_OUTPUT_1;
            }
            catch (Exception ex)
            {
                MessageBox.Show("btnOn_Click() : " + ex.Message);
            }
        }

        private void btnOff_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if ((devObj.netId & 0xff00) == 0x7000)
                    devObj.IOMode = netDev.IO_MODE_CURTAIN_BWD;
                else
                    devObj.IOMode = netDev.IO_MODE_GPIO_OUTPUT_0;
            }
            catch (Exception ex)
            {
                MessageBox.Show("btnOff_Click() : " + ex.Message);
            }
        }

        private void btnStop_Click(object sender, RoutedEventArgs e)
        {
            devObj.IOMode = netDev.IO_MODE_CURTAIN_STOP;
        }

        private void btnQuery_Click(object sender, RoutedEventArgs e)
        {
            devObj.query();
        }

        private void txtMac_TextChanged(object sender, TextChangedEventArgs e)
        {
            if((sender as TextBox).Text.Trim() != devObj.mac)
            {
                (sender as TextBox).Text = devObj.mac;
                MessageBox.Show("mac 不能修改");
            }
        }

        public void setId(int id)
        {
            lbDevName.Content = id;
        }
    }

}

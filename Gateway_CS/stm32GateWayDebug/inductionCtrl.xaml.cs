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
    public enum inductionType
    {
          客厅感应,
          卧室感应,
          烟感,
          燃气
    }

    /// <summary>
    /// inductionCtrl.xaml 的交互逻辑
    /// </summary>
    public partial class inductionCtrl : UserControl
    {
        devTable obj = new devTable();
        public inductionCtrl()
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

        inductionType _type;
        public inductionType type
        {
            get
            {
                return _type;
            }
            set
            {
                switch(value)
                {
                    case inductionType.燃气:
                        {
                            lbDis.Content = "燃气";
                            lbValue.Content = "0";
                        }
                        break;
                    case inductionType.客厅感应:
                        {
                            lbValue.Content = "";
                            lbDis.Content = "客厅感应";
                            cvsMap.Visibility = Visibility.Hidden;
                        }
                        break;
                    case inductionType.卧室感应:
                        {
                            lbValue.Content = "";
                            lbDis.Content = "卧室感应";
                            cvsMap.Visibility = Visibility.Hidden;
                        }
                        break;
                    case inductionType.烟感:
                        {
                            lbDis.Content = "烟感";
                            lbValue.Content = "0";
                        }
                        break;
                }
            }
        }

        public void updateValue(devTable dev)
        {
            //0x200黄色 
            //0x300红色
            obj = dev;
            Usable = true;
            switch (dev.netId >> 8)
            {
                case 0x81://"人体";
                    {
                        if( dev.curSt < 0) 
                            cvsMain.Background = Brushes.Red;
                        else
                            cvsMain.Background = Brushes.Transparent;
                    }
                    break;
                case 0x82://"烟感";
                case 0x83://"燃气";
                    {
                        if(dev.curSt > 0x300)
                        {
                            cvsMain.Background = Brushes.Red;
                            double pos = 66.0 + (dev.curSt - 0x300) * 34.0 / 0x200 - 1.5;
                            if (pos > 97.5)
                                pos = 97.5;
                            Canvas.SetLeft(pgValue,pos);
                        }
                        else if(dev.curSt > 0x200)
                        {
                            cvsMain.Background = Brushes.Yellow;
                            double pos = 33.0 + (dev.curSt - 0x200) * 33.0 / 0x100 - 1.5;
                            Canvas.SetLeft(pgValue, pos);
                        }
                        else
                        {
                            cvsMain.Background = Brushes.Transparent;
                            double pos = dev.curSt * 33.0 / 0x200 - 1.5;
                            Canvas.SetLeft(pgValue, pos);
                        }
                        lbValue.Content = dev.curSt.ToString();
                    }
                    break;
                //case 0x83://"燃气";
                //    {

                //    }
                //    break;
                default:
                    break;

            }
        }
    }
}

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
    /// devMgrCtrl.xaml 的交互逻辑
    /// </summary>
    public partial class devMgrCtrl : UserControl
    {
        public devMgrCtrl()
        {
            InitializeComponent();
           // netDev.parseMsgCmdHandle
        }

        public void refreshDevs()
        {
            cvsPanel.Children.Clear();
            List<devTable> devsLst = devMgr.getAllDevs();
            for(int i = 0;i< devsLst.Count;i++)
            {
                add(devsLst[i]);
            }
        }
        public void update(devTable dev)
        {
            bool getDev = false;
            foreach(devCtrl ctrl in cvsPanel.Children)
            {
                if(ctrl.devObj.mac == dev.mac)
                {
                    ctrl.update(dev);
                    getDev = true;
                    break;
                }
            }
            if(!getDev)
            {
                add(dev);
            }
        }

        public void add(devTable dev)
        {
            devCtrl ctrl = new devCtrl(dev);
            cvsPanel.Children.Add(ctrl);
            cvsPanel.Height = cvsPanel.Children.Count * 30;
            int count = cvsPanel.Children.Count - 1;
            Canvas.SetLeft(ctrl, 10.0);
            Canvas.SetTop(ctrl, count  * 30);
           // ctrl.setId(count + 1);
        }

        bool isMousedown = false;
        Canvas ctrlObj;
        Point downPos;
        private void cvsPanel_MouseDown(object sender, MouseButtonEventArgs e)
        {
            ctrlObj = sender as Canvas;
            isMousedown = true;
            downPos = e.GetPosition(cvsBox);
        }

        private void cvsBox_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if(isMousedown)
            {
                isMousedown = false;
                ctrlObj = null;
            }
        }

        private void cvsBox_MouseMove(object sender, MouseEventArgs e)
        {
            if(isMousedown)
            {
                if(e.LeftButton == MouseButtonState.Pressed)
                {
                    Point curPos = e.GetPosition(cvsBox);
                    double top = curPos.Y - downPos.Y + Canvas.GetTop(ctrlObj);
                    Canvas.SetTop(ctrlObj, top);
                    downPos = curPos;
                }
            }
        }

        
    }
}

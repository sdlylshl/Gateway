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

namespace stm32GateWayDebug
{
    public delegate void nullEvent();
    /// <summary>
    /// crc32Mgr.xaml 的交互逻辑
    /// </summary>
    public partial class crc32Mgr : Window
    {
        public crc32Mgr()
        {
            InitializeComponent();
        }
        public nullEvent backToMainHandle
        {
            get;
            set;
        }

        private void Window_Unloaded(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("quit");
            if (backToMainHandle != null)
                backToMainHandle();
        }

        private void txtCmd_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                string strCmd = txtCmd.Text.Trim();
                string[] strValue = strCmd.Split(' ');
                UInt32[] data = new UInt32[strValue.Length];
                for (int i = 0; i < strValue.Length; i++)
                {
                    try
                    {
                        data[i] = Convert.ToUInt32(strValue[i], 16);
                    }
                    catch
                    {
                        MessageBox.Show("参数错误！\r\n正确的参数格式为:\r\n\"CRC32 : \"");
                        return;
                    }
                }
                UInt32 crc32Value = crc32.stm32crc32(data, data.Length);
                txtCrcResult.Text = crc32Value.ToString("X8");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error!" + ex.Message);
            }
        }

        private void btnTable_Click(object sender, RoutedEventArgs e)
        {
            string str = "";
            for (int i = 0; i < 256; i++)
            {
                str += crc32.crcTable[i].ToString("X8") + " ";
            }
            Console.WriteLine("crc table : " + str);
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace stm32GateWayDebug
{
    public class E_ModeMgr
    {
        /// <summary>
        /// MODE_HOME = 0x01
        /// MODE_LEAVE = 0x02
        /// MODE_PROTECT = 0x03
        /// MODE_DEF = 0x04
        /// </summary>
        public enum modeType : byte
        {
            MODE_HOME = 0x01,
            MODE_LEAVE = 0x02,
            MODE_Rest = 0x03,
            MODE_DEF = 0x04,

 
        }
        static modeType _curMode;
        public static modeType curMode
        {
            get
            {
                return _curMode;
            }
            set
            {
                _curMode = value;
                byte[] data = new byte[11]{0xE0,0x02/*len*/, 0,0,/*sn*/ 0xB2,(byte)_curMode,/*mode*/  0,0,0,0,/*crc32*/ 0x18};//0xE0 (len 1) (sn 2) B2 mode (crc 4) 0x18

                netDev.setMsgSN(data);
                crc32.setCrc32Value(data);
                MainWindow.sendDataToCmdAndSendDirectCtrl(data);
            }
        }
        
        
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;


namespace stm32GateWayDebug
{
    public class logFileMgr
    {
        static FileStream fsLog;
        static StreamWriter swLog;

        public static void addLog(string str)
        {
            if(fsLog == null)
            {
                fsLog = new FileStream("gateWay.log", FileMode.OpenOrCreate);
            }
            if(swLog == null)
            {
                swLog = new StreamWriter(fsLog);
            }
            string strTime = DateTime.Now.ToString("F");
            swLog.WriteLine(strTime + " " + str);
            swLog.Flush();
        }
        
        public static void addSysErr(string str)
        {
            if (fsLog == null)
            {
                fsLog = new FileStream("gateWay.log", FileMode.OpenOrCreate); ;
            }
            if (swLog == null)
            {
                swLog = new StreamWriter(fsLog);
            }
            string strTime = DateTime.Now.ToString("F");
            swLog.WriteLine(strTime + "[error] " + str);
            swLog.Flush();
        }
    }
}

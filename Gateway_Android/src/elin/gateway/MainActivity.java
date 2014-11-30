package elin.gateway;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.view.View.OnClickListener;
import android.view.View;
import java.net.Socket;
import java.io.*;

import android.os.Handler;
import android.os.Message;

public class MainActivity extends ActionBarActivity {

	//devMgr 
	
	final int CMDLEN = 50;
	final int DEV_NAME_SIZE = 12;
	E_DevMgr devMgr;
	
	Button btnLink;
	Button btnStop;
	Button btnWinFWD;
	Button btnWinPause;
	Button btnWinBWD;
	Button btnAlarm;
	Button btnLightOpen;
	Button btnLightClose;
	Button btnDoorOpen;
	Button btnDoorClose;
	Button btnHumen1;
	Button btnHumen2;
	Button btnGas;
	Button btnFire;
	Button btnModeHome;
	Button btnModeLeave;
	Button btnModeRest;
	
	
	
	Socket sockSer;
	InputStream  readStream; 
	OutputStream writeStream;
	Boolean flagQuit = true;
	displayHandler updateListHandler;// parseHandler;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        
        
        crc32Init();
        devMgr = new E_DevMgr();
        
        updateListHandler = new displayHandler();

        btnStop = (Button)findViewById(R.id.btnStop);
        btnLink = (Button)findViewById(R.id.btnLink);
        btnWinFWD = (Button)findViewById(R.id.btnWindowFWD);
    	btnWinPause = (Button)findViewById(R.id.btnWindowPause);
    	btnWinBWD = (Button)findViewById(R.id.btnWindowBWD);
    	btnAlarm = (Button)findViewById(R.id.btnAlarm);
    	btnLightOpen = (Button)findViewById(R.id.btnLightOpen);
    	btnLightClose = (Button)findViewById(R.id.btnLightClose);
    	btnDoorOpen = (Button)findViewById(R.id.btnDoorOpen);
    	btnDoorClose = (Button)findViewById(R.id.btnDoorClose);
    	btnHumen1 = (Button)findViewById(R.id.btnHumen1);
    	btnHumen2 = (Button)findViewById(R.id.btnHumen2);
    	btnGas = (Button)findViewById(R.id.btnGas);
    	btnFire = (Button)findViewById(R.id.btnFire);
        btnModeHome= (Button)findViewById(R.id.btnModeHome);
        btnModeLeave= (Button)findViewById(R.id.btnModeLeave);
        btnModeRest= (Button)findViewById(R.id.btnModeRest);
        
        btnStop.setOnClickListener(new btnStopOnClickListener()); 
        btnLink.setOnClickListener(new btnLinkOnClickListener());
        btnWinFWD.setOnClickListener(new btnWinFWDOnClickListener());
        btnWinPause.setOnClickListener(new btnWinPauseOnClickListener());
        btnWinBWD.setOnClickListener(new btnWinBWDOnClickListener());
        btnAlarm.setOnClickListener(new btnAlarmOnClickListener());
        btnLightOpen.setOnClickListener(new btnLightOpenOnClickListener());
        btnLightClose.setOnClickListener(new btnLightCloseOnClickListener());
        btnDoorOpen.setOnClickListener(new btnDoorOpenOnClickListener());
        btnDoorClose.setOnClickListener(new btnDoorCloseOnClickListener());
        btnHumen1.setOnClickListener(new btnHumen1OnClickListener());
        btnHumen2.setOnClickListener(new btnHumen2OnClickListener());
        btnGas.setOnClickListener(new btnGasOnClickListener());
        btnFire.setOnClickListener(new btnFireOnClickListener());
        btnModeHome.setOnClickListener(new btnModeHomeOnClickListener());
        btnModeLeave.setOnClickListener(new btnModeLeaveOnClickListener());
        btnModeRest.setOnClickListener(new btnModeRestOnClickListener());
    }
    class btnTestOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		toastLongShow("test");
    	}
    }
    class btnStopOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
				flagQuit = false;
				readStream.close();
				writeStream.close();
				sockSer.close();
				finish();
    		}
    		catch(Exception ex)
    		{
    			toastLongShow("stop thread error!");
    		}
			
    	}
    }
    class btnLinkOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			sockSer = new Socket("192.168.1.202",20107);
    			
    			readStream = sockSer.getInputStream();
    			writeStream = sockSer.getOutputStream();
    			toastLongShow("socket ok!");
    			
    			flagQuit = true;
    			readTread.start();
    			btnLink.setEnabled(false);
    			//Log.i("[gateWay]","click");
    		}
    		catch(Exception ex)
    		{
    			System.out.println("[sockTest] " + "link Ser Err !");
    			Log.d("[sockTest]", "link Ser Err !");
    			toastLongShow("err" + ex.getMessage());
    			ex.printStackTrace();
    		}
    	
    	}
    }
    class btnWinFWDOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{								//E0    0B    00    26    08    C7              E5           D3     02   00    4B   12   00   03    12    C7 23 E2 ED 18
    			byte[] rq = new byte[]{(byte)0xE0, 0x0B, 0x00, 0x26, 0x08, (byte)0xC7, (byte)0xE5, (byte)0xD3, 0x02, 00, 0x4B, 0x12, 00, 0x03, 0x12, (byte)0xC7, 0x23, (byte)0xE2, (byte)0xED, 0x18};
    			writeStream.write(rq);
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnWinPauseOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    											//E0 0B   00   2A 08 C7 					E5 				D3 02 00 	4B 	12 		00 03 	13 88 45 55 08 18
    			byte[] rq = new byte[]{(byte)0xE0, 0x0B, 00, 0x2A, 0x08, (byte)0xC7, (byte)0xE5, (byte)0xD3, 0x02, 00, 0x4B, 0x12, 00, 0x03, 0x13, (byte)0x88, 0x45, 0x55, (byte)0x08, 0x18};
    			writeStream.write(rq);

    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnWinBWDOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    										//E0 	0B 	00 	2C 		08 		C7 				E5 			D3 	02 		00 		4B 12 	00 	03 		11 	A4 14 BB 4F 18
    			byte[] rq = new byte[]{(byte)0xE0, 0x0B, 00, 0x2C, 0x08, (byte)0xC7, (byte)0xE5, (byte)0xD3, 0x02, 0x00, 0x4B, 0x12, 00, 0x03, 0x11, (byte)0xA4, (byte)0x14, (byte)0xBB, 0x4F, 0x18};
    			writeStream.write(rq);
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnAlarmOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnLightOpenOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
//(byte)0xE0, (byte)0x0B, (byte)0x00, (byte)0x30, (byte)0x08, (byte)0xFB, (byte)0xE4, (byte)0xD3, (byte)0x02, (byte)0x00, (byte)0x4B, (byte)0x12, (byte)0x00, (byte)0x03, (byte)0x04, (byte)0x9A, (byte)0xD5, (byte)0xFC, (byte)0x08, (byte)0x18
       			byte[] rq = new byte[]{(byte)0xE0, (byte)0x0B, (byte)0x00, (byte)0x30, (byte)0x08, (byte)0xFB, (byte)0xE4, (byte)0xD3, (byte)0x02, (byte)0x00, (byte)0x4B, (byte)0x12, (byte)0x00, (byte)0x03, (byte)0x04, (byte)0x9A, (byte)0xD5, (byte)0xFC, (byte)0x08, (byte)0x18};
    			writeStream.write(rq);

    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnLightCloseOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			//E0 0B 00 2E 08 FB E4 D3 02 00 4B 12 00 03 03 36 0E 2D 80 18
    			byte[] rq = new byte[]{(byte)0xE0, 0x0B, 0x00, 0x2E, 0x08, (byte)0xFB, (byte)0xE4, (byte)0xD3, 0x02, 0x00, 0x4B, 0x12, 0x00, 0x03, 0x03, 0x36, 0x0E, 0x2D, (byte)0x80, 0x18};
    			writeStream.write(rq);

    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnDoorOpenOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnDoorCloseOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnHumen1OnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnHumen2OnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnGasOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnFireOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			
    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
 /// home E0 02 00 14 B2 01 A7 7C 79 9B 18
    // leave E0 02 00 16 B2 02 4A FF 16 39 18
    class btnModeLeaveOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
       			byte[] rq = new byte[]{(byte)0xE0, 0x02, 00, 0x16, (byte)0xB2, 0x02, 0x4A, (byte)0xFF, 0x16, 0x39, 0x18};
    			writeStream.write(rq);

    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnModeRestOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{
    			//E0 02 00 1A B2 03 05 3D 87 FA 18
    			byte[] rq = new byte[]{(byte)0xE0, 0x02, 00, 0x1A, (byte)0xB2, 0x03, 0x05, 0x3D, (byte)0x87, (byte)0xFA, 0x18};
    			writeStream.write(rq);

    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    class btnModeHomeOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    		try
    		{								//E0 02 00 02 B2 01 9A B8 A4 7C 18
       			byte[] rq = new byte[]{(byte)0xE0, 0x02, 00, 0x02, (byte)0xB2, 0x01, (byte)0x9A, (byte)0xB8, (byte)0xA4, (byte)0x7C, 0x18};
    			writeStream.write(rq);

    		}
    		catch(Exception ex)
    		{
    		}
    	}
    }
    
    
    class btnCrcOnClickListener implements OnClickListener
    {
    	public void onClick(View v) {
    	//	EditText txt = (EditText)findViewById(R.id.txtEdit);
    	//	String str = txt.getText().toString();
//    		String[] strBuf = str.split(" ");
//    		byte[]  data = new byte[strBuf.length];
//    		int[] dataCrc = new int[strBuf.length];
//   		
//    		for(int i = 0;i< strBuf.length;i++)
//    		{
//    			//Log.i("gateWay","strBuf [" + strBuf[i] + "]");
//    			data[i] = (byte)(Integer.parseInt(strBuf[i],16) & 0xff);
//    			dataCrc[i] = Integer.parseInt(strBuf[i], 16)&0xff;
//    			//Log.i("gateWay",Integer.toHexString(data[i]));
//    		}
//    		
//    		int crc = stm32crc32(data, 0, data.length);
//    		int crcInt = stm32crc32(dataCrc,0,dataCrc.length);
//    		Log.i("gateWay","CRC " + Integer.toHexString(crc));
//    		Log.i("gateWay","CRCInt " + Integer.toHexString(crcInt));
    		
    	}
    }

	 class displayHandler extends Handler{
		public displayHandler()
		{
			
		}
		@Override
		public void handleMessage(Message msg) {
			Bundle bundle = msg.getData();
			String strCmd = "";
			E_DevObj devObj = new E_DevObj();
			try
			{
//				设备测试指令
//				E0 1D 03 09 01 01 01 01 02 C1 3A D7 02 00 4B 12 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 52 FF AC 59 18
//
//				E0 1D 03 2F 01 01 01 81 02 DC 81 D4 02 00 4B 12 00 01 01 00 A0 00 00 00 00 00 00 00 00 00 00 00 00 FF C2 D8 8B 18
				  byte state = bundle.getByte("state");
				  devObj.devstate = state;
				  //strCmd += "state : " + Integer.toHexString(state & 0xff) + "\n";

                  byte protocol = bundle.getByte("protocol");
                  //strCmd += "protocol : " + Integer.toHexString(protocol & 0xff) + "\n";

                  short netId = bundle.getShort("netId");
                  short curSt = bundle.getShort("curSt");
                 // Log.i("[gateWay]","NetId " + Integer.toHexString(((netId >> 8)&0xff)));
                  switch((netId >> 8)&0xff)
                  {
                  case 0x01://"灯";
                  {
                	if(curSt > 0)
                	{
                		btnLightOpen.setBackgroundColor(getResources().getColor(R.color.colorON));
                		btnLightClose.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                	}
                	else
                	{
                		btnLightOpen.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                		btnLightClose.setBackgroundColor(getResources().getColor(R.color.colorON));
                	}
                	
                  }
                      //灯.updateValue(dev);
                      break;
                  case 0x02://"插座";

                      break;
                  case 0x03://"窗帘";
                      {
                         // 窗帘.updateValue(dev);
                      }
                      break;
                  case 0x04://"门锁";
                      {
                      	if(curSt > 0)
                    	{
                    		btnDoorOpen.setBackgroundColor(getResources().getColor(R.color.colorON));
                    		btnDoorClose.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                    	}
                    	else
                    	{
                    		btnDoorOpen.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                    		btnDoorClose.setBackgroundColor(getResources().getColor(R.color.colorON));
                    	}
     
                      }
                      break;
                  case 0x05://"警报器";
                      {
                        ///  警报器.updateValue(dev);
                      	if(curSt > 0)
                    	{
                    		btnAlarm.setBackgroundColor(getResources().getColor(R.color.colorON));
                    	}
                    	else
                    	{
                    		btnAlarm.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                    	}
     
                      }
                      break;

                  case 0x80://"开关";

                      break;
                  case 0x81://"人体";
                      if(netId == 0x8101)
                      {
                       	if(curSt > 0)
                    	{
                    		btnHumen1.setBackgroundColor(getResources().getColor(R.color.colorON));
                    	}
                    	else
                    	{
                    		btnHumen1.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                    	}
     
                      }
                      else
                      {
                       	if(curSt > 0)
                    	{
                    		btnHumen2.setBackgroundColor(getResources().getColor(R.color.colorON));
                    	}
                    	else
                    	{
                    		btnHumen2.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                    	}
     
                      }
 
                	  
                	  
                      break;
                  case 0x82://"烟感";
                   	if(curSt > 0)
                	{
                		btnGas.setBackgroundColor(getResources().getColor(R.color.colorON));
                	}
                	else
                	{
                		btnGas.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                	}
 
                      break;
                  case 0x83://"燃气";
                   	if(curSt > 0)
                	{
                		btnFire.setBackgroundColor(getResources().getColor(R.color.colorON));
                	}
                	else
                	{
                		btnFire.setBackgroundColor(getResources().getColor(R.color.colorOFF));
                	}
 
                      break;
                  default:

                      break;
                  }
                  
                  //strCmd += "netId : " + Integer.toHexString(netId & 0xff) + "\n";

            	  byte[] mac = bundle.getByteArray("mac");
            	  strCmd += "mac : ";
            	  for(int i = 0;i < mac.length;i++)
            	  {
            		  strCmd += Integer.toHexString(mac[i]&0xff);
            	  }
            	  strCmd += "\n";
            	  
            	  byte io = bundle.getByte("io");
            	  strCmd += "io : " + Integer.toHexString(io & 0xff) + "\n";

            	  byte ioMode = bundle.getByte("ioMode");
            	  strCmd += "ioMode : " + Integer.toHexString(ioMode & 0xff) + "\n";

//                  short curSt = bundle.getShort("curSt");
                  strCmd += "curSt : " + Integer.toHexString(curSt & 0xff) + "\n";

                  byte type = bundle.getByte("type");
            	  strCmd += "type : " + Integer.toHexString(type & 0xff) + "\n";

	              byte[] name = bundle.getByteArray("name");
	              strCmd += "name : ";
	              for(int i = 0;i< name.length;i++)
	              {
	              	strCmd += Integer.toHexString(name[i] & 0xff) + " ";
	              }
	              strCmd += "\n";

                 // Log.i("[gateWay]","handlerMessage " + strCmd);
                  //tvData.setText(strCmd);

//				else
//				{
//					Log.i("[gateWay]","ctrl < 0");
//				}
				
			}
			catch(Exception ex)
			{
				Log.e("[gateWay]","handleMessage");
				ex.printStackTrace();
			}
			super.handleMessage(msg);
		}
    	
    }
      
    //此线程只负责接受服务端的数据，然后交给updateListHandler.handleMessage解析
    byte[] cmdData = new byte[CMDLEN];
    int cmdIndex = 0;
    Thread readTread = new Thread()
    {
    	@Override
		public void start(){
			Log.i("[gateWay]","start---------");
			super.start();
		}
		@Override
		public void run() {
			try {
				while(flagQuit)
				{
					byte[] buf = new byte[1024];
					int dataLen = readStream.read(buf);
					//String str = "dataBuf : ";
//					for(int i = 0;i< dataLen;i++)
//					{
//						str += Integer.toHexString(buf[i]&0xff) + " ";
//					}
					//Log.i("gateWay","[1] " + str);
				//	Log.i("[gateWay]","[cmdIndex] " + dataLen + " " + cmdIndex);
					for(int i = 0;i < dataLen;i++)
					{
						if(cmdIndex == 0)
						{
							if( (int)(buf[i]&0xff) == 0xe0)
							{
								cmdData[cmdIndex++] = buf[i];
							}
						}
						else
						{
						//	Log.i("[gateWay]","[cmdIndex] " + cmdIndex);
							cmdData[cmdIndex++] = buf[i];
							if((int)(cmdData[1]&0xff) + 9 == cmdIndex)
							{
//								String strData = "" + buf[i] + " " + cmdIndex + " : ";
//								for(int j = 0;j < cmdIndex;j++)
//								{
//									strData += Integer.toHexString(cmdData[j]&0xff) + " ";
//								}
//								Log.i("gateWay","[2] " + strData);
								parseDataToList(cmdData);
								cmdIndex = 0;
							}
							if(cmdIndex >= CMDLEN)
							{
								//Message msg = updateListHandler.obtainMessage();
								cmdIndex = 0;
								//msg.arg1 = 0;
//								Bundle bundle = new Bundle();
//								bundle.putByte("ctrl", (byte)0);
								Log.i("[gateWay]","[CMDLEN] error len " + ((int)(cmdData[1]&0xff) + 9));
//								msg.setData(bundle);
								//msg.sendToTarget();
							}
						}
					}
				}
				
			} catch (Exception ex) {
				// TODO Auto-generated catch block
				Log.e("[gateWay]","[readTread]" + cmdIndex);
				ex.printStackTrace();
			}
		}
    };
    public class msgCmd
    {
        public byte usable;
        public byte head;//0xE0
        public byte[] sn = new byte[2];//
        public byte len;//data 的长度
        public byte[] data = new byte[50];
        public byte[] crc = new byte[4];
        public byte endl;//0x18
    }
    
    final byte NET_CMD_HEAD = (byte)0xe0;
    final byte NET_CMD_END = (byte)0x18;
    //在此解析单条指令
    public Boolean parseDataToList(byte[] data)
    {
    	//Log.i("[gateWay]", "parseDataToList");
    	int index = 0;
    	if((int)(data[index] & 0xff) == 0xe0)
    	{
    		//Log.i("gateWay",data.length + " " + (data[1] + 9));
    		if(data.length >= (data[1]&0xff) + 9)
    		{		
    			//Log.i("gateWay", data[(data[1]&0xff) + 8] + "");
	    		if(data[ (data[1]&0xff) + 8] == 0x18)
	    		{
	    			if(checkCrc(data))
	    			{
		    			//Log.i("gateWay","check crc32 ok!");
		    			try
		    			{
		    				String strRq = "";
			    			byte[] rq = new byte[]{(byte)0xe0,0,0,0,0,0,0,0,0x18};
			    			rq[2] = data[2];
			    			rq[3] = data[3];
			    			int crc = stm32crc32(rq, rq.length);
			    			rq[4] = (byte)((crc>>>24)&0xff);
			    			rq[5] = (byte)((crc>>>16)&0xff);
			    			rq[6] = (byte)((crc>>>8)&0xff);
			    			rq[7] = (byte)((crc>>>0)&0xff);
			    			writeStream.write(rq);
			    			for(int i = 0;i< rq.length;i++)
			    			{
			    				strRq += Integer.toHexString(rq[i]&0xff) + " ";
			    			}
			    			//Log.i("[gateWay]","[rq] " + strRq);
		    			}
		    			catch(Exception ex)
		    			{
		    				ex.printStackTrace();
		    				return false;
		    			}
		    			index = 4;
		    			switch(data[index])
		    			{
			    			case 0x01:
			    			{
			    				msgCmd pcmdS = new msgCmd();
			    				
		    				    pcmdS.head = NET_CMD_HEAD;
		                        pcmdS.len = data[1];
		
		                        pcmdS.sn[0] = data[2 ];
		                        pcmdS.sn[1] = data[3 ];
		                        
		                        for (int i = 0; i < data[1] && i < CMDLEN; i++)
		                        {
		                            pcmdS.data[i] = data[index++];
		                            
		                        }
		                       // Log.i("[gateWay]","[pcmdS]");
		                        parseCmd(pcmdS.data);
		                        pcmdS.crc[0] = data[index++ ];
		                        pcmdS.crc[1] = data[index++ ];
		                        pcmdS.crc[2] = data[index++ ];
		                        pcmdS.crc[3] = data[index++ ];
		                        pcmdS.endl = NET_CMD_END;
		                        
			    				    
			    			}
			    				break;
			    			case 0x10:
			    			{
			    				
			    			}
			    				break;
			    				default:
			    					break;
		    			}
		    			return true;
		    		}
	    		}
    		}
    	}
    	return false;
    }
    
    public void parseCmd(byte[] data)
    {
    	switch (data[0])
        {
            case 0x01://设备更新
                {

                    	Bundle bundle = new Bundle();
                    	bundle.putByte("ctrl", data[1]);
                    	
                    	 //byte state = 0;
                         //byte protocol = 0;
                         short netId = 0;



                         int index = 1;
                         
                         //state = data[index];
                         bundle.putByte("state", data[index]);
                         index++;//state      byte[1]

                         //protocol = data[index];
                         bundle.putByte("protocol", data[index]);
                         index++;//protocol   byte[1]

                         netId = (short)((data[index] << 8) | data[index + 1]);
                         bundle.putShort("netId", netId);
                         index += 2;//netId   byte[2]
                         //long mac = getMac(data,index);
                        byte[] macArr = new byte[8];
                    	for(int i = 0;i< 8;i++)
                    	{
                    		macArr[i] = data[index + i];
                    	}
                    	bundle.putByteArray("mac", macArr);
                            //通过MAC获取设备
                        index += 8;
                        //io设置
                        	byte io = data[index];
                        	bundle.putShort("io", io);
                            index += 1;
                        //ioMode设置
                    	byte ioMode = data[index] ;
                    	bundle.putShort("ioMode", ioMode);
                        index += 1;
                        //当前状态
                    	short curSt = (short)((data[index] << 8) | data[index + 1]);
                    	bundle.putShort("curSt", curSt);
                        index += 2;
                        //设备名称

                    	byte[] name = new byte[DEV_NAME_SIZE];
                        for(int i = 0;i< DEV_NAME_SIZE;i++)
                        {
                        	name[i] = data[index + i];
                        }
                        bundle.putByteArray("name", name);
                        
						Message msg = updateListHandler.obtainMessage();
						msg.setData(bundle);
						msg.sendToTarget();
                }
                break;
        }
    }
    
    public static long getMac(byte[] mac, int offset)
    {
        long macValue = 0;
        for(int i = 0;i< 8;i++)
        {
            macValue |= (long)mac[offset + i] << (7 - i) * 8 ;
        }
       // macValue = (UInt64)(mac[offset + 0] | (mac[offset + 1] << 8) | (mac[offset + 2] << 16) | (mac[offset + 3] << 24) | (mac[offset + 4] << 32) | (mac[offset + 5] << 40) | (mac[offset + 6] << 48) | (mac[offset + 7] << 56));
        return macValue;
    }
    
    public Boolean checkCrc(byte[] arr)
    {
//    	String str = "";
    	if(arr.length >= arr[1] + 8)
    	{
//        	for(int i = 0;i< arr[1] + 9;i++)
//        	{
//        		str += Integer.toHexString(arr[i]&0xff) + " ";
//        	}
        	
    		int crcLen = arr[1];
	    	int crc_calc = stm32crc32(arr,1,crcLen + 3);
	    	int crcStart = crcLen + 4;

	    	int crc0 = (crc_calc >>> 24)&0xff;
	    	int crc1 = (crc_calc >>> 16)&0xff;
	    	int crc2 = (crc_calc >>> 8)&0xff;
	    	int crc3 = (crc_calc >>> 0)&0xff;
	    	
	    	int crcArr0 = arr[crcStart] & 0xff;
	    	int crcArr1 = arr[crcStart + 1] & 0xff;
	    	int crcArr2 = arr[crcStart + 2] & 0xff;
	    	int crcArr3 = arr[crcStart + 3] & 0xff;
	    	
//	    	String strValue = "";
//	    	strValue += Integer.toHexString(crc0) + " " + Integer.toHexString(arr[crcStart] & 0xff) + " ";
//	    	strValue += Integer.toHexString(crc1) + " " + Integer.toHexString(arr[crcStart + 1] & 0xff) + " ";
//	    	strValue += Integer.toHexString(crc2) + " " + Integer.toHexString(arr[crcStart + 2] & 0xff) + " ";
//	    	strValue += Integer.toHexString(crc3) + " " + Integer.toHexString(arr[crcStart + 3] & 0xff) + " ";
//	    	
//	    	Log.i("gateWay","strValue " + strValue);
	    	if((crc0 == crcArr0) &&(crc1 == crcArr1)&&(crc2 == crcArr2)&&(crc3 == crcArr3))
	    	{
	    		//Log.i("gateWay","Crc true " + str);
	    		return true;
	    	}
    	}
    	//Log.i("gateWay","Crc false " + str);
    	return false;
    }
    
    final int WIDTH = (8 * 4);
    final int TOPBIT = (1 << (WIDTH - 1));
    int[] crcTable = new int[256];
    final int POLYNOMIAL = 0x04C11DB7;
    final int INITIAL_REMAINDER =  0xFFFFFFFF;
    final int FINAL_XOR_VALUE =    0x00000000;
    public  void crc32Init()
    {
        int remainder;
        int bit;
       // String str = "";
        /* Perform binary long division, a bit at a time. */
        for(int dividend = 0; dividend < 256; dividend++)
        {
            /* Initialize the remainder.  */
            remainder = dividend << (WIDTH - 8);
            /* Shift and XOR with the polynomial.   */
            for(bit = 0; bit < 8; bit++)
            {
                /* Try to divide the current data bit.  */
                if((remainder & TOPBIT) != 0)
                {
                    remainder = (remainder << 1) ^ POLYNOMIAL;
                }
                else
                {
                    remainder = remainder << 1;
                }
            }
            /* Save the result in the table. */
            crcTable[dividend] = remainder;
         //   str += Integer.toHexString(remainder) + " ";
        }
       // System.out.println(str);
    } /* crcInit() */
    
    public int stm32crc32(byte[] message, int nWords)
    {
        int[] data = new int[nWords];
        for (int i = 0; i < nWords; i++)
        {
            data[i] = message[i]&0xff;
        }
        return stm32crc32(data,0, nWords);
    }
    public int stm32crc32(byte[] message,int offset,int nWords)
    {
        int[] data = new int[nWords];
        int j = 0;
       // String str = "stm32crc32 ";
        for (int i = offset; i < offset + nWords; i++)
        {
            data[j++] = message[i]&0xff;
            //str += Integer.toHexString(message[i] & 0xff) + " ";
        }
        //Log.i("gateWay",str);
        return stm32crc32(data,0, nWords);
    }
    public int stm32crc32(int[] message,int offset, int nWords)
    {
        int byteValue;
        int remainder = INITIAL_REMAINDER;
        for (; offset < nWords; offset++)
        {
            byteValue = ((remainder >>> 24) ^ ((message[offset] >>> 24) & 0xff))&0xff;
            remainder = (crcTable[byteValue]) ^ (remainder << 8);
            
            byteValue = ((remainder >>> 24) ^ ((message[offset] >>> 16) & 0xff))&0xff;
            remainder = (crcTable[byteValue]) ^ (remainder << 8);

            byteValue = ((remainder >>> 24) ^ ((message[offset] >>> 8) & 0xff))&0xff;
            remainder = (crcTable[byteValue]) ^ (remainder << 8);

            byteValue = ((remainder >>> 24) ^ (message[offset] & 0xff))&0xff;
            remainder = (crcTable[byteValue]) ^ (remainder << 8);
            
        }
        return (remainder);
    } 

    public void toastLongShow(String msg)
    {
		Toast.makeText(getApplicationContext(), msg,
				Toast.LENGTH_LONG).show();
    }

    public void toastShortShow(String msg)
    {
		Toast.makeText(getApplicationContext(), msg,
				Toast.LENGTH_SHORT).show();
    }
    
//    @Override
//    public boolean onCreateOptionsMenu(Menu menu) {
//        // Inflate the menu; this adds items to the action bar if it is present.
//        getMenuInflater().inflate(R.menu.main, menu);
//        return true;
//    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        /*
         * 
         * add()方法的四个参数，依次是：
         * 
         * 1、组别，如果不分组的话就写Menu.NONE,
         * 
         * 2、Id，这个很重要，Android根据这个Id来确定不同的菜单
         * 
         * 3、顺序，那个菜单现在在前面由这个参数的大小决定
         * 
         * 4、文本，菜单的显示文本
         */

//        menu.add(Menu.NONE, Menu.FIRST + 1, 5, "删除").setIcon(
//
//        android.R.drawable.ic_menu_delete);

        // setIcon()方法为菜单设置图标，这里使用的是系统自带的图标，同学们留意一下,以

        // android.R开头的资源是系统提供的，我们自己提供的资源是以R开头的

//        menu.add(Menu.NONE, Menu.FIRST + 1, 2, "保存").setIcon(
//
//        android.R.drawable.ic_menu_edit);
        menu.add(Menu.NONE, Menu.FIRST + 1, 1, "退出").setIcon(

        android.R.drawable.ic_menu_close_clear_cancel);
        
        menu.add(Menu.NONE, Menu.FIRST + 2, 2, "帮助").setIcon(

        android.R.drawable.ic_menu_help);
//
//        menu.add(Menu.NONE, Menu.FIRST + 4, 1, "添加").setIcon(
//
//        android.R.drawable.ic_menu_add);
//
//        menu.add(Menu.NONE, Menu.FIRST + 5, 4, "详细").setIcon(
//
//        android.R.drawable.ic_menu_info_details);
//
//        menu.add(Menu.NONE, Menu.FIRST + 6, 3, "发送").setIcon(
//
//        android.R.drawable.ic_menu_send);

        getMenuInflater().inflate(R.menu.main, menu);
        return true;

   }
}






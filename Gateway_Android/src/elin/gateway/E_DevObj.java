package elin.gateway;

import java.util.Arrays;

public class E_DevObj {
	public final int DEV_NAME_SIZE = 12;
	
	public boolean  usable;
    public byte     devstate;
    public byte     protocol;
    public short   	netId;
    public byte[]   mac = new byte[8];
    public byte     io;
    public byte 	ioMode;
    public short   	curSt;
    public byte     type;
    public byte[]   name = new byte[DEV_NAME_SIZE];
    
    public E_DevObj()
    {
    	usable = false;
    	devstate = 0;
    	protocol = 0;
    	netId = 0;
    	//mac = "";
    	io = 0;
    	ioMode = 0;
    	curSt = 0;
    	type = 0;
    	//name = "";
    }
    public void copyFrom(E_DevObj dev)
    {
    	devstate = dev.devstate;
    	protocol = dev.protocol;
    	netId = dev.netId;
    	System.arraycopy(dev.mac, 0, mac, 0, dev.mac.length);
    	io = dev.io;
    	ioMode = dev.ioMode;
    	curSt = dev.curSt;
    	type = dev.type; 
    	System.arraycopy(dev.name, 0, name, 0, dev.name.length);
    }
    
    public boolean equals(E_DevObj dev)
    {
    	if(Arrays.equals(dev.mac, mac) && dev.io == io)
    	{
    		return true;
    	}
    	return false;
    }
     
}

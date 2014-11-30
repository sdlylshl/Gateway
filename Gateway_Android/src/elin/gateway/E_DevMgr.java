package elin.gateway;

import java.util.*;


public class E_DevMgr {
	E_DevObj[] devs = new E_DevObj[10];
	
	public boolean AddDev(E_DevObj dev)
	{
		for(int i = 0;i< devs.length;i++)
		{
			if(devs[i] == null)
			{
				devs[i] = new E_DevObj();
				devs[i].copyFrom(dev);
				devs[i].usable = true;
				return true;
			}
			else if(!devs[i].usable)
			{
				devs[i].copyFrom(dev);
				devs[i].usable = true;
				return true;
			}
		}
		return false;
	}
	
	public boolean DelDev(E_DevObj dev)
	{
		for(int i = 0;i< devs.length;i++)
		{
			if(devs[i].mac == dev.mac)
			{
				devs[i].usable = false;
				return true;
			}
		}
		return false;
	}
	
	public boolean IsExist(byte[] mac ,byte io)
	{
		for(int i = 0;i< devs.length;i++)
		{
			if(Arrays.equals(devs[i].mac,mac) && devs[i].io == io)
			{
				return true;
			}
		}
		return false;
	}
	
	
}

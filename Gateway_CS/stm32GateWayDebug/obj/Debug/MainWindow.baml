   M S B A M L   `   `   `  ����L  Hstm32GateWayDebug, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null5clr-namespace:stm32GateWayDebugstm32GateWayDebug  5   	   "    stm32GateWayDebug.MainWindow   5   	   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   1localclr-namespace:stm32GateWayDebug     5   	   -   5          � Title$  stm32GateWay��5   	   $
��1150��6   $
��1080��6+    � WindowStartupLocation$ CenterScreen=�6R    � WindowState$ 	Maximized=�6w   .��5      �� -   5         ��Name$ cvsPanel��5      $
��1140��6   .+�5   
   "   stm32GateWayDebug.devMgrCtrl  -   5          Name$ devCtrls��5      
 �� Top$ 52��6.    �� Left$ 20��6>   �� 5	   
   -   5        $ cvsSend��5	   >   $
��1000��6   $	��300��6    �� Bottom$ 50��6+   $y�Visible=�6O   $	 503��6d   $ 16��6u   5      �� 5   
   -   5         ��Name$ btnGetAllDevs��5       $��获取设备��6   $ 20��67   $ 10��6H   $��80��6X   $��32��6c   6o   �� 5   
   -   5        $ btnClearAllDevs��5       $��清空设备��6   $	 114��69   $ 10��6K   $��80��6[   $��32��6f   6r   �� 5   
   -   5        $ btnModeSet_1��5       $��回家模式��6   $	 304��66   $ 10��6H   $��80��6X   $��32��6c   6o   �� 5   
   -   5        $ btnModeSet_2��5       $��离家模式��6   $	 404��66   $ 10��6H   $��80��6X   $��32��6c   6o   �� 5   
   -	   5        $ btnModeSet_3��5      $��布防��6   $	 504��64   $ 10��6F   $��80��6V   $��32��6a   6m   �� 5   
   -
   5        $ btnModeSet_4��5      $��撤防��6   $	 604��64   $ 10��6F   $��80��6V   $��32��6a   6m   "   stm32GateWayDebug.cmdMgrCtrl  5   
   -   5        	  Name$	 
ctrlCmdMgr��5      $	 503��6/   $ 16��6@   �� 5   
   -   5        $ btnCrc32��5      $��CRC32��6#   $	 210��63   $ 10��6E   $��75��6U   $��32��6`   6l   5      5      
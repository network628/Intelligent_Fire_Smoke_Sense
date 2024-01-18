
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "wdg.h"
#include "stm32f10x.h"    //包含需要的头文件
#include "main.h"         //包含需要的头文件
#include "delay.h"        //包含需要的头文件
#include "usart1.h"       //包含需要的头文件
#include "eeprom_iic.h"          //包含需要的头文件
#include "24c02.h" 		    //包含需要的头文件
//#include "timer1.h"       //包含需要的头文件
#include "timer3.h"       //包含需要的头文件
#include "led.h"          //包含需要的头文件
//#include "key.h"          //包含需要的头文件
#include "wifi.h"	       //包含需要的头文件
#include "W5500_APP.h"    //包含需要的头文件
#include "usart4.h"
#include "bc28.h"
#include "usart5.h"
#include "i2c.h"
#include "SHT2X.h"
#include "usart2.h"        //包含需要的头文件
#include "timer4.h"        //包含需要的头文件   
#include "rtc.h"
#include "malloc.h"  
#include "MMC_SD.h" 
#include "ff.h"  
#include "exfuns.h"
#include "fattester.h" 

 
Sensor_TypeDef Sensor;
u16 add_num = 0;
 
int main(void) 
{	
//	u16 min_state; //分钟每加一次写一次文件

 	u32 total,free;
	#ifdef Wifi_ESP8266 
   int Wifi_stime;	
	#endif
	Delay_Init();                   //延时功能初始化  
	Usart1_Init(9600);              //串口1功能初始化，波特率9600
	#ifdef Blue_APP	 
 	    Uart4_init(115200);									 
	#endif
  LED_GPIO_Init();	    //初始化与LED连接的硬件接口 
	ALARM_GPIO_Init(); 
	EEPROM_IIC_Init();
	I2C_Configuration();  //SHT20

  u4_printf("start_smoky  \r\n\r\n");  //提示信息
	EEPROM_Data_Init();    
  mem_init();			//初始化内存池	
	if(SD_Initialize())					//检测SD卡
	{
		u4_printf("SD Card Error! \r\n");
		Delay_Ms(200);
		LED=!LED;//DS0闪烁
	}								   	
 	exfuns_init();							//为fatfs相关变量申请内存				 
  f_mount(fs[0],"0:",1); 					//挂载SD卡   
	if(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
	{
		u4_printf("Fatfs Error! \r\n");
		Delay_Ms(200);
		LED=!LED;//DS0闪烁
	}	
  u4_printf("FATFS OK! \r\n");	
	u4_printf("SD_total_Size: %d MB<--->SD_free_Size: %d MB \r\n",total>>10, free>>10);
	
  if(RTC_Init())		//RTC初始化	，一定要初始化成功
	{ 
		u4_printf("RTC ERROR!   \r\n");
		Delay_Ms(800);
		u4_printf("RTC Trying...\r\n");		
	}	
	
	#ifdef W5500_APP 	 
		W5500_init();                   //初始化W5500										 
	#endif
	
	#ifdef GPRS_BC26
	  Usart2_Init(9600);           //串口2功能初始化，波特率115200	
    while(BC28_Init()); 	
    BC28_PDPACT();
    BC28_ConTCP();
  #endif
	  
  #ifdef Wifi_ESP8266 	 
		Usart2_Init(115200);           //串口2功能初始化，波特率115200	
    TIM3_Init(300,7200);           //10Khz的计数频率，定时时间 300*7200*1000/72000000 = 30ms	
		WiFi_ResetIO_Init();           //初始化WiFi的复位IO B3	
			while(WiFi_ConnectServer()){ //循环，初始化，连接服务器，直到成功
			u4_printf("\r\nServerIP:%d.%d.%d.%d:Port:%04d \r\n",DstIP[0],DstIP[1],DstIP[2],DstIP[3],DstPort);
			Delay_Ms(2000);              //延时
		}      
		WiFi_RxCounter=0;                      //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);//清空WiFi接收缓冲区               
		WiFi_Connect_flag = 1;                      //Connect_flag=1,表示连接上服务器	
	#endif
	
	#ifdef PM1006    //PB9
   Uart5_init(9600);	
	#endif
	
  while(Sensor.DEV_Num==0)
	{
	  Sensor.DEV_Num = SHT2X_Init();   //SHT20
		 u4_printf("\r\n\r\n Sensor.DEV_Num = %d\r\n\r\n",Sensor.DEV_Num);  //提示信息
		Delay_Ms(300);
   	
	}

	SHT2X_TEST();
 	                          // 256    3125
	//时间计算(大概):Tout=((4*2^8)*rlr)/40 (ms).
	IWDG_Init(6,2048);    //与分频数为256,重载值为6250,溢出时间为40s	
 
 
	while(1)                        //主循环
	{		
			Time3_Count++;
//		u4_printf("\r\n\r\n Time3_Count = %d\r\n\r\n",Time3_Count);  //提示信息
			if(Time3_Count > 60000)
			{
			  Time3_Count = 0;
			}	
			if(KEY == 0)
			{
			  write_txt_file(add_num);
//      	LED = ~LED; 
				Delay_Ms(300);	
				add_num++;			
			}
			if(Time3_Count%20==0)
			{
				RTC_Get();
				LED = ~LED;
        SHT2X_TEST();
			}			
			IWDG_Feed();//喂狗
      Modbus_Get_Data();
			
		 #ifdef	Dian_Smoke
			if(SMOKE_IN == 0)
			{
				Sensor.Smoke_Dev=1;
			}
			else 
			{
				Sensor.Smoke_Dev=0;
			}
  
			if(KEY == 0)
			{
				LED = 0;  //灭灯
				SMOKE_VCC = 0;  // SMOKE_ON  0关闭   
				Delay_Ms(2000);
				SMOKE_VCC = 1;  // SMOKE_ON  1打开
			}
			#endif 
			
		  #ifdef PM1006
//			 Sensor.PM2_5 = Get_PM1006_Value(); //PM2.5传感器
      if(Time3_Count%10==0)
			{
				Uart5_Send_Data(); Delay_Ms(200);
			}
			Uart5_Receive_Data();  //新型PM2.5传感器
		  if(Sensor.PM2_5>Sensor.PM2_5_Waring)	
		  {
				Sensor.Smoke_Dev=1;
			}
			else 
			{
				Sensor.Smoke_Dev=0;
			}

			
			#endif
		 
	#ifdef W5500_APP 	
 
 	    w5500_app();
//			Delay_Ms(200);	
	#endif 
 
	#ifdef Wifi_ESP8266 	

    Wifi_Receive_UseData();
		if(Wifi_stime>=50){                                           //当time大于等于1000的时候，大概经过1s的时间
			Wifi_stime=0;                                               //清除time计数
		  Wifi_Send_UseData();	
		}
		Delay_Ms(1);  //延时1ms
		Wifi_stime++;       //time计数器+1 					 
	#endif
	
	#ifdef Blue_APP	 
		  Blue_app();	
 
			if((Sensor.Temperature_Waring < Sensor.Temperature)||(Sensor.Humidity_Waring > Sensor.Humidity)||(Sensor.Smoke_Dev==1))
			{
			  BEEP = 1;  //ALARM_EN 响
				LED = 1;   //开灯
				u4_printf("SET_DATA:%04d %3.2f'C %3.2f%% %03d %3.2f'C %3.2f%% %03d OK\r\n",Sensor.DEV_Num,Sensor.Temperature,Sensor.Humidity,Sensor.PM2_5,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
			}	
			else
			{
			  BEEP = 0;  // 不响
				LED = 0;  //关灯
			}
  
	#endif
  
	#ifdef GPRS_BC26
	   if(Time3_Count%30==0)
		 {
			 Send_BlueData();
       NB_iot_Send_Data();
			 
		 }
 
		 if(Time3_Count%5==0)
			 
			 BC28_RECData();
		 
	#endif
	}
}
 
char Wifi_IP[20];	
void EEPROM_Data_Init(void)
{
 
		/********************读出EEPROM*************************/	
	  Sensor.DEV_Num = (AT24C02_ReadOneByte(Addr_Base-0)*255);
		Sensor.DEV_Num = Sensor.DEV_Num + (AT24C02_ReadOneByte(Addr_Base-1)%255);
	  Sensor.Temperature_Waring = AT24C02_ReadOneByte(Addr_Base-2)<<8;
		Sensor.Temperature_Waring = (Sensor.Temperature_Waring + AT24C02_ReadOneByte(Addr_Base-3))/10;
	  Sensor.Humidity_Waring = (AT24C02_ReadOneByte(Addr_Base-4)<<8);
		Sensor.Humidity_Waring = (Sensor.Humidity_Waring + AT24C02_ReadOneByte(Addr_Base-5))/10;
	  Sensor.PM2_5_Waring = (AT24C02_ReadOneByte(Addr_Base-6)<<8);
		Sensor.PM2_5_Waring = (Sensor.PM2_5_Waring + AT24C02_ReadOneByte(Addr_Base-7));
	/**************************************************************/
	
		DstIP[0] = AT24C02_ReadOneByte(Addr_Base- 8);    
		DstIP[1] = AT24C02_ReadOneByte(Addr_Base- 9);  
		DstIP[2] = AT24C02_ReadOneByte(Addr_Base-10);   
		DstIP[3] = AT24C02_ReadOneByte(Addr_Base-11);   
		DstPort  = AT24C02_ReadOneByte(Addr_Base-12)<<8;   
		DstPort  = DstPort + AT24C02_ReadOneByte(Addr_Base-13); 
#ifdef W5500_APP 	
	u4_printf("\r\nW5500_Server_IP:%d.%d.%d.%d:%04d \r\n",DstIP[0],DstIP[1],DstIP[2],DstIP[3],DstPort);
#endif

#ifdef Wifi_ESP8266 
//extern char *ServerIP;           //存放服务器IP或是域名
//extern int  ServerPort;          //存放服务器的端口号区
    ServerPort = DstPort;
		sprintf(Wifi_IP,"%d.%d.%d.%d",DstIP[0],DstIP[1],DstIP[2],DstIP[3]);
//		u4_printf(Wifi_IP);
//		u4_printf("\r\n");
    strcat(Wifi_IP,"\0");
		ServerIP = Wifi_IP;
 
 u4_printf("WIFI_Server_IP:%s:%d\r\n",ServerIP,ServerPort);		
#endif

#ifdef GPRS_BC26
//#define SERVERIP "49.235.207.142"
//#define SERVERPORT 8001
    SERVERPORT = DstPort;
		sprintf(Wifi_IP,"%d.%d.%d.%d",DstIP[0],DstIP[1],DstIP[2],DstIP[3]);
    strcat(Wifi_IP,"\0");
		SERVERIP = Wifi_IP;
 
 u4_printf("GPRS_BC26_Server_IP:%s:%d\r\n",SERVERIP,SERVERPORT);		
#endif

}



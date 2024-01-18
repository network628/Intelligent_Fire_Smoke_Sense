
/*-----------------------------------------------------*/
/*                                                     */
/*           ����main��������ں���Դ�ļ�              */
/*                                                     */
/*-----------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "wdg.h"
#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "main.h"         //������Ҫ��ͷ�ļ�
#include "delay.h"        //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "eeprom_iic.h"          //������Ҫ��ͷ�ļ�
#include "24c02.h" 		    //������Ҫ��ͷ�ļ�
//#include "timer1.h"       //������Ҫ��ͷ�ļ�
#include "timer3.h"       //������Ҫ��ͷ�ļ�
#include "led.h"          //������Ҫ��ͷ�ļ�
//#include "key.h"          //������Ҫ��ͷ�ļ�
#include "wifi.h"	       //������Ҫ��ͷ�ļ�
#include "W5500_APP.h"    //������Ҫ��ͷ�ļ�
#include "usart4.h"
#include "bc28.h"
#include "usart5.h"
#include "i2c.h"
#include "SHT2X.h"
#include "usart2.h"        //������Ҫ��ͷ�ļ�
#include "timer4.h"        //������Ҫ��ͷ�ļ�   
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
//	u16 min_state; //����ÿ��һ��дһ���ļ�

 	u32 total,free;
	#ifdef Wifi_ESP8266 
   int Wifi_stime;	
	#endif
	Delay_Init();                   //��ʱ���ܳ�ʼ��  
	Usart1_Init(9600);              //����1���ܳ�ʼ����������9600
	#ifdef Blue_APP	 
 	    Uart4_init(115200);									 
	#endif
  LED_GPIO_Init();	    //��ʼ����LED���ӵ�Ӳ���ӿ� 
	ALARM_GPIO_Init(); 
	EEPROM_IIC_Init();
	I2C_Configuration();  //SHT20

  u4_printf("start_smoky  \r\n\r\n");  //��ʾ��Ϣ
	EEPROM_Data_Init();    
  mem_init();			//��ʼ���ڴ��	
	if(SD_Initialize())					//���SD��
	{
		u4_printf("SD Card Error! \r\n");
		Delay_Ms(200);
		LED=!LED;//DS0��˸
	}								   	
 	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  f_mount(fs[0],"0:",1); 					//����SD��   
	if(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
		u4_printf("Fatfs Error! \r\n");
		Delay_Ms(200);
		LED=!LED;//DS0��˸
	}	
  u4_printf("FATFS OK! \r\n");	
	u4_printf("SD_total_Size: %d MB<--->SD_free_Size: %d MB \r\n",total>>10, free>>10);
	
  if(RTC_Init())		//RTC��ʼ��	��һ��Ҫ��ʼ���ɹ�
	{ 
		u4_printf("RTC ERROR!   \r\n");
		Delay_Ms(800);
		u4_printf("RTC Trying...\r\n");		
	}	
	
	#ifdef W5500_APP 	 
		W5500_init();                   //��ʼ��W5500										 
	#endif
	
	#ifdef GPRS_BC26
	  Usart2_Init(9600);           //����2���ܳ�ʼ����������115200	
    while(BC28_Init()); 	
    BC28_PDPACT();
    BC28_ConTCP();
  #endif
	  
  #ifdef Wifi_ESP8266 	 
		Usart2_Init(115200);           //����2���ܳ�ʼ����������115200	
    TIM3_Init(300,7200);           //10Khz�ļ���Ƶ�ʣ���ʱʱ�� 300*7200*1000/72000000 = 30ms	
		WiFi_ResetIO_Init();           //��ʼ��WiFi�ĸ�λIO B3	
			while(WiFi_ConnectServer()){ //ѭ������ʼ�������ӷ�������ֱ���ɹ�
			u4_printf("\r\nServerIP:%d.%d.%d.%d:Port:%04d \r\n",DstIP[0],DstIP[1],DstIP[2],DstIP[3],DstPort);
			Delay_Ms(2000);              //��ʱ
		}      
		WiFi_RxCounter=0;                      //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);//���WiFi���ջ�����               
		WiFi_Connect_flag = 1;                      //Connect_flag=1,��ʾ�����Ϸ�����	
	#endif
	
	#ifdef PM1006    //PB9
   Uart5_init(9600);	
	#endif
	
  while(Sensor.DEV_Num==0)
	{
	  Sensor.DEV_Num = SHT2X_Init();   //SHT20
		 u4_printf("\r\n\r\n Sensor.DEV_Num = %d\r\n\r\n",Sensor.DEV_Num);  //��ʾ��Ϣ
		Delay_Ms(300);
   	
	}

	SHT2X_TEST();
 	                          // 256    3125
	//ʱ�����(���):Tout=((4*2^8)*rlr)/40 (ms).
	IWDG_Init(6,2048);    //���Ƶ��Ϊ256,����ֵΪ6250,���ʱ��Ϊ40s	
 
 
	while(1)                        //��ѭ��
	{		
			Time3_Count++;
//		u4_printf("\r\n\r\n Time3_Count = %d\r\n\r\n",Time3_Count);  //��ʾ��Ϣ
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
			IWDG_Feed();//ι��
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
				LED = 0;  //���
				SMOKE_VCC = 0;  // SMOKE_ON  0�ر�   
				Delay_Ms(2000);
				SMOKE_VCC = 1;  // SMOKE_ON  1��
			}
			#endif 
			
		  #ifdef PM1006
//			 Sensor.PM2_5 = Get_PM1006_Value(); //PM2.5������
      if(Time3_Count%10==0)
			{
				Uart5_Send_Data(); Delay_Ms(200);
			}
			Uart5_Receive_Data();  //����PM2.5������
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
		if(Wifi_stime>=50){                                           //��time���ڵ���1000��ʱ�򣬴�ž���1s��ʱ��
			Wifi_stime=0;                                               //���time����
		  Wifi_Send_UseData();	
		}
		Delay_Ms(1);  //��ʱ1ms
		Wifi_stime++;       //time������+1 					 
	#endif
	
	#ifdef Blue_APP	 
		  Blue_app();	
 
			if((Sensor.Temperature_Waring < Sensor.Temperature)||(Sensor.Humidity_Waring > Sensor.Humidity)||(Sensor.Smoke_Dev==1))
			{
			  BEEP = 1;  //ALARM_EN ��
				LED = 1;   //����
				u4_printf("SET_DATA:%04d %3.2f'C %3.2f%% %03d %3.2f'C %3.2f%% %03d OK\r\n",Sensor.DEV_Num,Sensor.Temperature,Sensor.Humidity,Sensor.PM2_5,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
			}	
			else
			{
			  BEEP = 0;  // ����
				LED = 0;  //�ص�
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
 
		/********************����EEPROM*************************/	
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
//extern char *ServerIP;           //��ŷ�����IP��������
//extern int  ServerPort;          //��ŷ������Ķ˿ں���
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



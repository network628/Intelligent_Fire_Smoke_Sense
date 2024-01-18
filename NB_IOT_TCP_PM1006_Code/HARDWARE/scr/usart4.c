#include "main.h"         //包含需要的头文件
#include "usart4.h"	  
#include "delay.h"
#include "stm32f10x.h"  //包含需要的头文件
#include "usart1.h"       //包含需要的头文件
#include "24c02.h"      //包含需要的头文件
#include "SHT2X.h"
#include "rtc.h"
#include "fattester.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
 
//////////////////////////////////////////////////////////////////



//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
 
	while((UART4->SR&0X40)==0);//循环发送,直到发送完毕   
	UART4->DR = (u8) ch;   

	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_UART4_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void UART4_IRQHandler(void)
{
	u8 res;	
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntEnter();    
#endif
	if(UART4->SR&(1<<5))//接收到数据
	{	 
		res=UART4->DR; 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	}
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif		

//初始化IO 串口1
//bound:波特率
//CHECK OK
//091209
void Uart4_init(u32 bound)
{  	
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//使能 GPIOD 时钟
 
	RCC->APB2ENR|=1<<4;   //使能PORTC口时钟  
	RCC->APB1ENR|=1<<19;  //使能串口4时钟 
	GPIOC->CRH&=0XFFFF00FF;//IO状态设置 PC10 PC11
	GPIOC->CRH|=0X00008B00;//IO状态设置
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD2      BT_BR
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD   BT_BR
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PA15      BT_EN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOD   BT_EN
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;//PC12  BT_BC
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 
 
	RCC->APB1RSTR|=1<<19;   //复位串口4
	RCC->APB1RSTR&=~(1<<19);//停止复位	   	   

  //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(UART4, &USART_InitStructure); //初始化串口1
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART4, ENABLE);                    //使能串口1 
	
#if EN_UART4_RX		  //如果使能了接收
	//使能接收中断
	UART4->CR1|=1<<8;    //PE中断使能
	UART4->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
//	MY_NVIC_Init(3,3,UART4_IRQChannel,2);//组2，最低优先级 
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
#endif
}


/*-------------------------------------------------*/
/*函数名：串口1 printf函数                         */
/*参  数：char* fmt,...  格式化输出字符串和参数    */
/*返回值：无                                       */
/*-------------------------------------------------*/

__align(8) char Usart4_TxBuff[USART4_TXBUFF_SIZE];  

void u4_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(Usart4_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)Usart4_TxBuff);		
	while((UART4->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		UART4->DR = Usart4_TxBuff[i];
		while((UART4->SR&0X40)==0);	
	}	
}

//u8 U_REC_LEN = 200;

void Blue_app(void)
{
	Receive_BlueData();		
}

void Receive_BlueData(void)
{
 
	 uint8_t IP[4];
	 uint16_t PORT;
	 uint16_t T_Waring, H_Waring, P_Waring;
   uint16_t num,cnt;
	if(strstr((char *)USART_RX_BUF,"DATA")) 
	{
		u4_printf("SET_DATA:%04d %3.2f'C %3.2f%% %03d %3.2f'C %3.2f%% %03d OK\r\n",Sensor.DEV_Num,Sensor.Temperature,Sensor.Humidity,Sensor.PM2_5,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
//	  u4_printf("USART_RX_BUF = %s\r\n",USART_RX_BUF);
	}
	else if(strstr((char *)USART_RX_BUF,"IP:"))   //设置IP
	{
		IP[0] = ((USART_RX_BUF[ 3]-'0')*100 + ((USART_RX_BUF[ 4]-'0')*10) + (USART_RX_BUF[ 5]-'0'));
		IP[1] = ((USART_RX_BUF[ 7]-'0')*100 + ((USART_RX_BUF[ 8]-'0')*10) + (USART_RX_BUF[ 9]-'0')); 
		IP[2] = ((USART_RX_BUF[11]-'0')*100 + ((USART_RX_BUF[12]-'0')*10) + (USART_RX_BUF[13]-'0'));
		IP[3] = ((USART_RX_BUF[15]-'0')*100 + ((USART_RX_BUF[16]-'0')*10) + (USART_RX_BUF[17]-'0')); 
    PORT = 	((USART_RX_BUF[19]-'0')*10000 + (USART_RX_BUF[20]-'0')*1000 + (USART_RX_BUF[21]-'0')*100 + ((USART_RX_BUF[22]-'0')*10) + (USART_RX_BUF[23]-'0')); 
//	  u4_printf("%d.%d.%d.%d->%d \r\n",IP[0],IP[1],IP[2],IP[3],PORT);
		AT24C02_WriteOneByte(Addr_Base- 8, IP[0]);    //DstIP[0]
		AT24C02_WriteOneByte(Addr_Base- 9, IP[1]);    //DstIP[1]  
		AT24C02_WriteOneByte(Addr_Base-10, IP[2]);    //DstIP[2]   
		AT24C02_WriteOneByte(Addr_Base-11, IP[3]);    //DstIP[3]   
		AT24C02_WriteOneByte(Addr_Base-12, (PORT>>8)&0x00ff);    //port h
		AT24C02_WriteOneByte(Addr_Base-13,  PORT    &0x00ff);    //port l
		
// 	  u4_printf("SET_IP=%s OK\r\n",USART_RX_BUF);
//		u4_printf("%s\r\n",USART_RX_BUF);
		u4_printf("\r\nSET_IP:%d.%d.%d.%d:Port:%04d OK\r\n",DstIP[0],DstIP[1],DstIP[2],DstIP[3],DstPort);
	}
	else if(strstr((char *)USART_RX_BUF,"IP?"))   //查看IP
	{
	  u4_printf("\r\nSET_IP:%d.%d.%d.%d:Port:%04d OK\r\n",DstIP[0],DstIP[1],DstIP[2],DstIP[3],DstPort);
	}
	else if(strstr((char *)USART_RX_BUF,"SET_D:")) 
	{
	  Sensor.DEV_Num = (USART_RX_BUF[6]-0x30)*1000 + (USART_RX_BUF[7]-0x30)*100 + (USART_RX_BUF[8]-0x30)*10 + (USART_RX_BUF[9]-0x30);
    /********************写入EEPROM**************************/
		AT24C02_WriteOneByte(Addr_Base-0, Sensor.DEV_Num/255);   //设备号千万位
		AT24C02_WriteOneByte(Addr_Base-1, Sensor.DEV_Num%255);   //设备号个百位 65535

 		u4_printf("SET_D:%04d OK\r\n",Sensor.DEV_Num);
//	  u4_printf("%s\r\n",USART_RX_BUF);
	}	
	else if(strstr((char *)USART_RX_BUF,"SET_T:")) 
	{
		T_Waring = (USART_RX_BUF[6]-0x30)*1000 + (USART_RX_BUF[7]-0x30)*100 + (USART_RX_BUF[8]-0x30)*10 + (USART_RX_BUF[9]-0x30);
 		AT24C02_WriteOneByte(Addr_Base-2,(T_Waring>>8)&0x00ff);   //温度预警十位
 		AT24C02_WriteOneByte(Addr_Base-3, T_Waring    &0x00ff);   //温度预警个位
		Sensor.Temperature_Waring = T_Waring;
		Sensor.Temperature_Waring = Sensor.Temperature_Waring/10;
		
	  u4_printf("SET_T:%3.2f'C OK\r\n",Sensor.Temperature_Waring);
//	  u4_printf("%s\r\n",USART_RX_BUF);
	}
	else if(strstr((char *)USART_RX_BUF,"SET_H:")) 
	{
		H_Waring = (USART_RX_BUF[6]-0x30)*1000 + (USART_RX_BUF[7]-0x30)*100 + (USART_RX_BUF[8]-0x30)*10 + (USART_RX_BUF[9]-0x30);
 		AT24C02_WriteOneByte(Addr_Base-4,(H_Waring>>8)&0x00ff);   //温度预警十位
 		AT24C02_WriteOneByte(Addr_Base-5, H_Waring    &0x00ff);   //温度预警个位
		Sensor.Humidity_Waring = H_Waring;
		Sensor.Humidity_Waring = Sensor.Humidity_Waring/10;
		
	  u4_printf("SET_H:%3.2f%%RH OK\r\n",Sensor.Humidity_Waring);
 
	}
	else if(strstr((char *)USART_RX_BUF,"SET_P:")) 
	{
		P_Waring = (USART_RX_BUF[6]-0x30)*1000 + (USART_RX_BUF[7]-0x30)*100 + (USART_RX_BUF[8]-0x30)*10 + (USART_RX_BUF[9]-0x30);
 		AT24C02_WriteOneByte(Addr_Base-6,(P_Waring>>8)&0x00ff);   //温度预警十位
 		AT24C02_WriteOneByte(Addr_Base-7, P_Waring    &0x00ff);   //温度预警个位
		Sensor.PM2_5_Waring = P_Waring;
		Sensor.Humidity_Waring = Sensor.Humidity_Waring/10;
		
	  u4_printf("SET_P:%dug/m3 OK\r\n",Sensor.PM2_5_Waring);
 
	}
	else if(strstr((char *)USART_RX_BUF,"SET_TIME:")) 
	{
		calendar.w_year = (USART_RX_BUF[9]-0x30)*1000 + (USART_RX_BUF[10]-0x30)*100 + (USART_RX_BUF[11]-0x30)*10 + (USART_RX_BUF[12]-0x30);
		calendar.w_month= (USART_RX_BUF[14]-0x30)*10 + (USART_RX_BUF[15]-0x30);
		calendar.w_date = (USART_RX_BUF[17]-0x30)*10 + (USART_RX_BUF[18]-0x30);
		calendar.hour   = (USART_RX_BUF[20]-0x30)*10 + (USART_RX_BUF[21]-0x30);
		calendar.min    = (USART_RX_BUF[23]-0x30)*10 + (USART_RX_BUF[24]-0x30);
		calendar.sec    = (USART_RX_BUF[26]-0x30)*10 + (USART_RX_BUF[27]-0x30);
 
		RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);  //设置时间	
    RTC_Get();
		u4_printf("SET_TIME:%04d-%02d-%02d %02d:%02d:%02d SET_OK\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	}
	else if(strstr((char *)USART_RX_BUF,"ls")) 
	{
	  mf_scan_files("0:/Data_Cache");
	}
	else if(strstr((char *)USART_RX_BUF,"read_sd:"))   // read_sd:0003-0011
	{
		num = (USART_RX_BUF[8]-0x30)*1000 + (USART_RX_BUF[9]-0x30)*100 + (USART_RX_BUF[10]-0x30)*10 + (USART_RX_BUF[11]-0x30);
		cnt = (USART_RX_BUF[13]-0x30)*1000 + (USART_RX_BUF[14]-0x30)*100 + (USART_RX_BUF[15]-0x30)*10 + (USART_RX_BUF[16]-0x30);
	  read_txt_file(num,cnt);  //从0条开始  共10条
	}
	
	memset(USART_RX_BUF,0,256);	
	USART_RX_STA = 0;
}

void Send_BlueData(void)
{
		  char i;
	    u32 chack_sum = 0;
			Send_String[0] = 0xEF;  //包头
//		  if((Sensor.Temperature_Waring < Sensor.Temperature)||(Sensor.Humidity_Waring > Sensor.Humidity)||(Sensor.Smoke_Dev==1))
//			Send_String[1] = 0X05;  //数据类型
//			else
			Send_String[1] = 0X04;  //数据类型
			Send_String[2] = 'M';   //烟雾、 温湿度混合型探测器
			Send_String[3] = 'E';   //防爆
			Send_String[4] = 'I';   //智能型
	 
	    Sensor.Year = calendar.w_year;
			Sensor.Month = calendar.w_month;
	    Sensor.Day = calendar.w_date;
	    
			Send_String[5]   = 0X30+Sensor.Year/10%10;
			Send_String[6]   = 0X30+Sensor.Year%10;   //出厂日期 年
			Send_String[7]   = 0X30+Sensor.Month/10;
			Send_String[8]   = 0X30+Sensor.Month%10;  //出厂日期 月
			Send_String[9]   = 0X30+Sensor.Day/10;
			Send_String[10]  = 0X30+Sensor.Day%10;  //出厂日期 日
			
			Send_String[11]  = 0X30+Sensor.DEV_Num/1000%10;  //设备号
			Send_String[12]  = 0X30+Sensor.DEV_Num/100%10;
			Send_String[13]  = 0X30+Sensor.DEV_Num/10%10;
			Send_String[14]  = 0X30+Sensor.DEV_Num%10;
			
			Send_String[15]  = 0X00;   //00：设备类型
			
			Send_String[16]  = (Sensor.Temperature*10)/255;   //温度
			Send_String[17]  = (int)(Sensor.Temperature*10)%255;
			
			Send_String[18]  = (Sensor.Humidity*10)/255;      //湿度
			Send_String[19]  = (int)(Sensor.Humidity*10)%255;

   #ifdef PM1006
			Send_String[20] = Sensor.PM2_5/255;
			Send_String[21] = Sensor.PM2_5%255;
//			u4_printf("\r\n\r\n Sensor.PM2_5 = %d\r\n\r\n",Sensor.PM2_5);  //提示信息
	 #endif
	 #ifdef	Dian_Smoke
			Send_String[20]  = 0;
			Send_String[21]  = Sensor.Smoke_Dev;
	 #endif	
			
			if(SHT2x_Calc_T() && (SHT2x_Calc_RH()))  //if
	     Send_String[22]  = 0X00;  //设备状态
			else
			 Send_String[22]  = 0X01;  //设备状态	
			
			for(i=1;i<23;i++)
			{
				chack_sum += Send_String[i]; 
			}
			Send_String[23]  = chack_sum % 255;
			//校验码
			Send_String[24]  = 0X1D;  //包尾（结束符）
 
//			for(i=0;i<25;i++)
//			{
//			  USART_SendData(UART4, Send_String[i]);
//				Delay_Ms(1); 
//			}
}



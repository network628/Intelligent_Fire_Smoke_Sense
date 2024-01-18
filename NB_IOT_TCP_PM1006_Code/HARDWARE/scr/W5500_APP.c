#include "stm32f10x.h"    //包含需要的头文件
#include "usart1.h"       //包含需要的头文件
#include "delay.h"        //包含需要的头文件
#include "wizchip_conf.h" //包含需要的头文件
#include "socket.h"       //包含需要的头文件
#include "dhcp.h"         //包含需要的头文件
#include "spi.h"          //包含需要的头文件
#include "dns.h"          //包含需要的头文件
#include "W5500_APP.h"    //包含需要的头文件
#include "24c02.h" 		    //包含需要的头文件
#include "usart4.h"
#include "SHT2X.h"
#include "main.h"         //包含需要的头文件

 
unsigned char  gRec_BUF[DATA_Rec_SIZE];    //数据接收缓冲区
unsigned char  gSend_BUF[100];    //数据发送缓冲区
//unsigned char  gDNSBUF[DATA_Rec_SIZE];     //数据缓冲区
char Connect_flag = 0;          //连接标志  0:未成功  1：成功
int  tcp_state;                 //用于保存TCP返回的状态
int  temp_state;                //用于临时保存TCP返回的状态
char my_dhcp_retry = 0;         //DHCP当前共计重试的次数
char SOCK_flag = 0;             //端口链接标志 0：没有链接服务器  1：已经链接服务器，等待服务器响应
wiz_NetInfo gWIZNETINFO =       //MAC地址自己写一个，不要和路由器下其他的设备一样即可
{        
0x00, 0x08, 0xdc,0x00, 0xab, 0xcd, 
}; 
/*---------------------------------------------------------------*/
/*         服务器IP地址和端口号，根据自己的情况修改              */
/*---------------------------------------------------------------*/
unsigned char DstIP[]={172,16,0,106};     //服务器IP地址
unsigned int  DstPort=40191; 

//unsigned char DstIP[]={47,96,28,113};     //服务器IP地址
//unsigned int  DstPort=40191; 
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
	int  ret,ret_s,ret_r;    //用于保存函数返回值
	int sended;  //原样返回接收到的数据长度
//  char ch_cnt=27; 
	unsigned char Send_String[25];
	unsigned char Receive_String[25];

void w5500_app(void)
{
 
  switch(DHCP_run())           //判断DHCP执行在哪个过程
	{
		case DHCP_IP_ASSIGN:     //该状态表示，路由器分配给开发板ip了
		case DHCP_IP_CHANGED:    //该状态表示，路由器改变了分配给开发板的ip
								 my_ip_assign();   //调用IP参数获取函数，记录各个参数
								 break;            //跳出
		
		case DHCP_IP_LEASED:     //该状态表示，路由器分配给的开发板ip，正式租用了，表示可以联网通信了
								 
								 ret=getSn_SR(SOCK_TCPS);       //获取TCP连接状态
		               
								 switch(ret)                    //判断socket0的状态
								 {
									  case SOCK_INIT:          //该状态表示：SOCK初始化成功，准备侦听  
										 u4_printf("Ready to connect to server\r\n");       //提示信息  准备连接服务器
										 ret = connect(SOCK_TCPS,DstIP,DstPort);  //连接服务器
										 if(ret == SOCKERR_NOPEN)                 //判断结果，如果等于SOCKERR_NOPEN，表示服务器未开启
										 {
											 u4_printf("The server is not turned on and will be reconnected in 3S\r\n");  //提示信息  服务器未开启，3s后准备重新连接
											 Delay_Ms(3000);				                //3s延时，再次启动连接服务器													 
										 } 
										 break;	//跳出
										 
										case SOCK_ESTABLISHED:   //该状态表示：连接建立成功
											   if((Connect_flag==0)&&(getSn_IR(SOCK_TCPS)==Sn_IR_CON))   //判断一下了解成功与否
												 {
													 Connect_flag = 1;
													 u4_printf("Connection established\r\n");      //串口提示信息  连接已建立
												 }
										     if(Time3_Count%40==0)           
													 Send_UseData();
													 Receive_UseData();
												 break;                                           //跳出
																 
									  case SOCK_CLOSE_WAIT:      //该状态表示：等待关闭连接
																 u4_printf("Wait for the connection to be closed\r\n");   //串口提示信息  等待关闭连接
																 if((ret=disconnect(SOCK_TCPS)) != SOCK_OK)  //关闭连接，并判断关闭成功与否
																 {
																	 u4_printf("Connection closing failed, ready to restart\r\n");   //提示信息    连接关闭失败，准备重启
																	 Delay_Ms(500);                              //延时500ms
																	 NVIC_SystemReset();	                     //重启
																 }
										             Connect_flag = 0; 
																 u4_printf("Connection closed successfully\r\n");         //提示关闭成功  连接关闭成功
																 break;  //跳出
																 
									  case SOCK_CLOSED:          //该状态表示：处在关闭阶段，我们连接服务器
																 u4_printf("Ready to open local port\r\n");      //提示信息  准备打开本地端口
																 ret = socket(SOCK_TCPS,Sn_MR_TCP,DstPort,Sn_MR_ND); //打开socket0的一个端口
																 if(ret != SOCK_TCPS)                             //判断，如果返回值不等于SOCK_TCPS，表示打开错误
																 {
																	  u4_printf("Port error, ready to restart\r\n");          //提示信息 端口错误，准备重启
																	  Delay_Ms(500);                             //延时500ms
																	  NVIC_SystemReset();	                     //重启
																 }
														     Connect_flag = 0; 
																 u4_printf("Opening local port succeeded\r\n");   //提示信息   打开本地端口成功
																 break;                                          //跳出
																 
									 default:             //其他状态
																 break;      //跳出 
								 }
								 break;  //跳出
		
		case DHCP_FAILED:   //该状态表示DHCP获取IP失败     									 
							my_dhcp_retry++;                        //失败次数+1
							if(my_dhcp_retry > MY_MAX_DHCP_RETRY){  //如果失败次数大于最大次数，进入if							
								u4_printf("DHCP failed, ready to restart \r\n");//串口提示信息  DHCP失败，准备重启
								NVIC_SystemReset();		            //重启
							}
							break;                                  //跳出
	}	
}

/*-------------------------------------------------*/
/*函数名：获取到IP时的回调函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void my_ip_assign(void)
{
   getIPfromDHCP(gWIZNETINFO.ip);     //把获取到的ip参数，记录到机构体中
   getGWfromDHCP(gWIZNETINFO.gw);     //把获取到的网关参数，记录到机构体中
   getSNfromDHCP(gWIZNETINFO.sn);     //把获取到的子网掩码参数，记录到机构体中
   getDNSfromDHCP(gWIZNETINFO.dns);   //把获取到的DNS服务器参数，记录到机构体中
   gWIZNETINFO.dhcp = NETINFO_DHCP;   //标记使用的是DHCP方式
   network_init();                    //初始化网络  
   u4_printf("DHCP lease term : %d S\r\n", getDHCPLeasetime());  //租期
}
/*-------------------------------------------------*/
/*函数名：获取IP的失败函数                         */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void my_ip_conflict(void)
{
	u4_printf("Failed to get IP address, ready to restart\r\n");   //提示获取IP失败  获取IP失败，准备重启
	NVIC_SystemReset();                      //重启
}
/*-------------------------------------------------*/
/*函数名：初始化网络函数                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void network_init(void)
{
	char tmpstr[6] = {0};
	wiz_NetInfo netinfo;
	
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);//设置网络参数
	ctlnetwork(CN_GET_NETINFO, (void*)&netinfo);	//读取网络参数
	ctlwizchip(CW_GET_ID,(void*)tmpstr);	        //读取芯片ID

	//打印网络参数
	if(netinfo.dhcp == NETINFO_DHCP) u4_printf("\r\n=== %s NET CONF : DHCP ===\r\n",(char*)tmpstr);
	else u4_printf("\r\n=== %s NET CONF : Static ===\r\n",(char*)tmpstr);	
  u4_printf("===========================\r\n");
	u4_printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",netinfo.mac[0],netinfo.mac[1],netinfo.mac[2],netinfo.mac[3],netinfo.mac[4],netinfo.mac[5]);			
	u4_printf("IP address: %d.%d.%d.%d\r\n", netinfo.ip[0],netinfo.ip[1],netinfo.ip[2],netinfo.ip[3]);
	u4_printf("default gateway: %d.%d.%d.%d\r\n", netinfo.gw[0],netinfo.gw[1],netinfo.gw[2],netinfo.gw[3]);
	u4_printf("Subnet mask: %d.%d.%d.%d\r\n", netinfo.sn[0],netinfo.sn[1],netinfo.sn[2],netinfo.sn[3]);
	u4_printf("DNS server: %d.%d.%d.%d\r\n", netinfo.dns[0],netinfo.dns[1],netinfo.dns[2],netinfo.dns[3]);
	u4_printf("===========================\r\n");
}
/*-------------------------------------------------*/
/*函数名：初始化W5500                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W5500_init(void)
{
	//W5500收发内存分区，收发缓冲区各自总的空间是16K，（0-7）每个端口的收发缓冲区我们分配 2K
    char memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}}; 
	char tmp;
		
	SPI_Configuration();                                    //初始化SPI接口
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);	//注册临界区函数
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  //注册SPI片选信号函数
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);	//注册读写函数
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){   //如果if成立，表示收发内存分区失败
		 u4_printf("Failed to initialize transceiver partition, ready to restart\r\n");      //提示信息  初始化收发分区失败,准备重启
  		 NVIC_SystemReset();                                //重启
	}	
    do{                                                     //检查连接状态
		 if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){ //如果if成立，表示未知错误		 
			u4_printf("Unknown error, ready to restart\r\n");            //提示信息  未知错误，准备重启
			NVIC_SystemReset();                             //重启
		 }
		 if(tmp == PHY_LINK_OFF){
			 u4_printf("The network cable is not connected\r\n");//如果检测到，网线没连接，提示连接网线
			 Delay_Ms(100);              //延时
		 }
	}while(tmp == PHY_LINK_OFF);                            //循环执行，直到连接上网线

	setSHAR(gWIZNETINFO.mac);                                   //设置MAC地址
	DHCP_init(SOCK_DHCP, gRec_BUF);                             //初始化DHCP
	reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);//注册DHCP回调函数 

	my_dhcp_retry = 0;	                                        //DHCP重试次数=0
	tcp_state = 0;                                              //TCP状态=0
	temp_state = -1;                                            //上一次TCP状态=-1
}


void Receive_UseData(void)
{
   char i;

	 if(getSn_IR(SOCK_TCPS) & Sn_IR_RECV)
    {
      setSn_IR(SOCK_TCPS, Sn_IR_RECV);														
    }
    if((ret=getSn_RX_RSR(SOCK_TCPS))>0)
		{
      memset(gRec_BUF,0,25);
			ret_r = recv(SOCK_TCPS,gRec_BUF,DATA_Rec_SIZE);    //接收来自Client的数据
	    if(ret_r == 25 && gRec_BUF[0]==0XEF && gRec_BUF[24]==0X1D && gRec_BUF[1]==0X06)   //如果接收数据量大于0
			{
				for(i=0;i<ret_r;i++)
				{
					Receive_String[i] = gRec_BUF[i];
//					USART_SendData(USART1, Receive_String[i]);
//					Delay_Ms(1); 
				}
				send(SOCK_TCPS,Receive_String,ret_r);              //将数据原样返回给客户端
		/********************写入EEPROM**************************/
				Sensor.DEV_Num = (Receive_String[11]-0x30)*1000 + (Receive_String[12]-0x30)*100 + (Receive_String[13]-0x30)*10 + (Receive_String[14]-0x30);
				AT24C02_WriteOneByte(Addr_Base-0, Sensor.DEV_Num/255);   //设备号千万位
				AT24C02_WriteOneByte(Addr_Base-1, Sensor.DEV_Num%255);   //设备号个百位 65535
				AT24C02_WriteOneByte(Addr_Base-2, Receive_String[16]);   //温度预警十位
				AT24C02_WriteOneByte(Addr_Base-3, Receive_String[17]);   //温度预警个位
				AT24C02_WriteOneByte(Addr_Base-4, Receive_String[18]);   //湿度预警十位
				AT24C02_WriteOneByte(Addr_Base-5, Receive_String[19]);   //湿度预警个位
				AT24C02_WriteOneByte(Addr_Base-6, Receive_String[20]);		
				AT24C02_WriteOneByte(Addr_Base-7, Receive_String[21]);   //PM2.5预警 
						
				Sensor.Temperature_Waring = (Receive_String[16]<<8);
				Sensor.Temperature_Waring = (Sensor.Temperature_Waring + Receive_String[17]) /10;
				Sensor.Humidity_Waring = Receive_String[18]<<8;
				Sensor.Humidity_Waring = (Sensor.Humidity_Waring + Receive_String[19]) / 10;
				Sensor.PM2_5_Waring = Receive_String[21];
						
				u4_printf("---W5500--->%04d %3.2f'C %3.2f%% %03d \r\n",Sensor.DEV_Num,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
			}
		} 
}


void Send_UseData(void)
{
  char i;
	u32 chack_sum = 0;
		  Sensor.Year  = 2019;
	    Sensor.Month =  6;
	    Sensor.Day   =  5;
			Send_String[0] = 0xEF;  //包头
//			if((Sensor.Temperature_Waring < Sensor.Temperature)||(Sensor.Humidity_Waring > Sensor.Humidity)||(Sensor.Smoke_Dev==1))
//			Send_String[1] = 0X05;  //数据类型
//			else
			Send_String[1] = 0X04;  //设备数据
			Send_String[2] = 'M';   //烟雾、 温湿度混合型探测器
			Send_String[3] = 'E';   //防爆
			Send_String[4] = 'I';   //智能型
 
			Send_String[5]   = 0X30+Sensor.Year/10%10;
			Send_String[6]   = 0X30+Sensor.Year%10;   //出厂日期 年
			Send_String[7]   = 0X30+Sensor.Month/10;
			Send_String[8]  = 0X30+Sensor.Month%10;  //出厂日期 月
			Send_String[9]  = 0X30+Sensor.Day/10;
			Send_String[10]  = 0X30+Sensor.Day%10;  //出厂日期 日	
			Send_String[11]  = 0X30+Sensor.DEV_Num/1000%10;
			Send_String[12]  = 0X30+Sensor.DEV_Num/100%10;
			Send_String[13]  = 0X30+Sensor.DEV_Num/10%10;
			Send_String[14]  = 0X30+Sensor.DEV_Num%10;
			
			if(Sensor.Temperature_Waring < Sensor.Temperature+10)
			{
			  Send_String[15]  = 0X0B;//高温报警
			}		
      else if(Sensor.Temperature_Waring < Sensor.Temperature)
			{
 	      Send_String[15]  = 0X0D;//高温预警
			}				
			else if(Sensor.Humidity_Waring > Sensor.Humidity)
			{
			  Send_String[15]  = 0X12;//湿度报警
			}		
			else if(Sensor.Humidity_Waring > Sensor.Humidity-10)
			{
			  Send_String[15]  = 0X11;//湿度预警
			}
			else if(Sensor.PM2_5_Waring<Sensor.PM2_5) 
			{
			  Send_String[15]  = 0X10;//烟雾浓度报警
			}
			else if(Sensor.PM2_5_Waring<Sensor.PM2_5+50) 
			{
			  Send_String[15]  = 0X0F;//烟雾浓度预警
			}
			else if(Sensor.Smoke_Dev==1)
			{
			  Send_String[15]  = 0X14;//火灾报警
			}
			else
			Send_String[15]  = 0X00;//正常 ：报警类型  设备类型
			
			Send_String[16]  = (Sensor.Temperature*10)/255;
			Send_String[17]  = (int)(Sensor.Temperature*10)%255;
			
			Send_String[18]  = (Sensor.Humidity*10)/255;
			Send_String[19]  = (int)(Sensor.Humidity*10)%255;

   #ifdef PM1006
			Send_String[20] = Sensor.PM2_5/255;
			Send_String[21] = Sensor.PM2_5%255;
	 #endif
	 #ifdef	Dian_Smoke
			Send_String[20]  = 0;
			Send_String[21]  = Sensor.Smoke_Dev;
	 #endif	
			
			if(Sensor.Temperature && Sensor.Humidity)  //if
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
			ret_s = 25;
//		  sended = sendto(SOCK_UDPS,Send_String,ret_s,DstIP,DstPort);
			send(SOCK_TCPS,Send_String,ret_s);              //将数据原样返回给客户端
//			for(i=0;i<23;i++)
//			{
//			  USART_SendData(USART1, Send_String[i]);
//				Delay_Ms(1); 
//			}
			
//     u4_printf("%04d %3.2f'C %3.2f%% %03d %3.2f'C %3.2f%% %03d\r\n",Sensor.DEV_Num,Sensor.Temperature,Sensor.Humidity,Sensor.Smoke_Dev,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
		
		if(sended != ret_s)  //如果发送的数据数据量 不等于 接收的数据量长度
		{
//			u4_printf("数据有丢失情况\r\n");   //提示信息
		}

//		AT24C02_ReadPage(0, out_String);
//		u4_printf("%s\r\n",out_String);
	
}













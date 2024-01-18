/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            操作602Wifi功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/
#include "usart4.h"	 
#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"       //包含需要的头文件
#include "wifi.h"	    //包含需要的头文件
#include "delay.h"	    //包含需要的头文件
#include "usart1.h"	    //包含需要的头文件
#include "led.h"        //包含需要的头文件
#include "key.h"        //包含需要的头文件
#include "24c02.h" 		    //包含需要的头文件
#include "SHT2X.h"
#include "rtc.h"
/*-----------------------------------------------------------------------------*/
/*                              根据自己的网络环境修改                         */
/*-----------------------------------------------------------------------------*/
char *ServerIP = "49.235.207.142";           //存放服务器IP或是域名
//char *ServerIP = "192.168.0.99";           //存放服务器IP或是域名
int  ServerPort = 8001;                   //存放服务器的端口号区
/*-----------------------------------------------------------------------------*/

char  Wifi_Data_buff[2048];     //数据缓冲区

char wifi_mode = 1;     //联网模式 0：SSID和密码写在程序里   1：Smartconfig方式用APP发送
char WiFi_Connect_flag;      //同服务器连接状态  0：还没有连接服务器  1：连接上服务器了

/*-------------------------------------------------*/
/*函数名：初始化WiFi的复位IO                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void WiFi_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //定义一个设置IO端口参数的结构体
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);   //使能PA端口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //ENABLEPBCLK	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				      //LED0-->PB8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					        //根据设定参数初始化GPIOB.8
	GPIO_SetBits(GPIOB,GPIO_Pin_3);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;             //key
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}
/*-------------------------------------------------*/
/*函数名：WiFi发送设置指令                         */
/*参  数：cmd：指令                                */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter=0;                           //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //清空WiFi接收缓冲区 
	WiFi_printf("%s\r\n",cmd);                  //发送指令
	while(timeout--){                           //等待超时时间到0
		Delay_Ms(100);                          //延时100ms
		if(strstr(WiFi_RX_BUF,"OK"))            //如果接收到OK表示指令成功
			break;       						//主动跳出while循环
		u4_printf("%d ",timeout);               //串口输出现在的超时时间
	}
	u4_printf("\r\n");                          //串口输出信息
	if(timeout<=0)return 1;                     //如果timeout<=0，说明超时时间到了，也没能收到OK，返回1
	else return 0;		         				//反之，表示正确，说明收到OK，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：WiFi复位                                 */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Reset(int timeout)
{
	RESET_IO(0);                                    //复位IO拉低电平
	Delay_Ms(500);                                  //延时500ms
	RESET_IO(1);                                    //复位IO拉高电平	
	while(timeout--){                               //等待超时时间到0
		Delay_Ms(100);                              //延时100ms
		if(strstr(WiFi_RX_BUF,"ready"))             //如果接收到ready表示复位成功
			break;       						    //主动跳出while循环
		u4_printf("%d ",timeout);                   //串口输出现在的超时时间
	}
	u4_printf("\r\n");                              //串口输出信息
	if(timeout<=0)return 1;                         //如果timeout<=0，说明超时时间到了，也没能收到ready，返回1
	else return 0;		         				    //反之，表示正确，说明收到ready，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：WiFi加入路由器指令                       */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter=0;                               //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //清空WiFi接收缓冲区 
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS); //发送指令	
	while(timeout--){                               //等待超时时间到0
		Delay_Ms(1000);                             //延时1s
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP\r\n\r\nOK")) //如果接收到WIFI GOT IP表示成功
			break;       						    //主动跳出while循环
		u4_printf("%d ",timeout);                   //串口输出现在的超时时间
	}
	u4_printf("\r\n");                              //串口输出信息
	if(timeout<=0)return 1;                         //如果timeout<=0，说明超时时间到了，也没能收到WIFI GOT IP，返回1
	return 0;                                       //正确，返回0
}
/*-------------------------------------------------*/
/*函数名：WiFi_Smartconfig                         */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{	
	WiFi_RxCounter=0;                           //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //清空WiFi接收缓冲区     
	while(timeout--){                           //等待超时时间到0
		Delay_Ms(1000);                         //延时1s
		if(strstr(WiFi_RX_BUF,"connected"))     //如果串口接受到connected表示成功
			break;                              //跳出while循环  
		u4_printf("%d ",timeout);               //串口输出现在的超时时间  
	}	
	u4_printf("\r\n");                          //串口输出信息
	if(timeout<=0)return 1;                     //超时错误，返回1
	return 0;                                   //正确返回0
}
/*-------------------------------------------------*/
/*函数名：等待加入路由器                           */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               //等待超时时间到0
		Delay_Ms(1000);                             //延时1s
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP"))       //如果接收到WIFI GOT IP表示成功
			break;       						    //主动跳出while循环
		u4_printf("%d ",timeout);                   //串口输出现在的超时时间
	}
	u4_printf("\r\n");                              //串口输出信息
	if(timeout<=0)return 1;                         //如果timeout<=0，说明超时时间到了，也没能收到WIFI GOT IP，返回1
	return 0;                                       //正确，返回0
}
/*-------------------------------------------------*/
/*函数名：等待连接wifi，获取IP地址                 */
/*参  数：ip：保存IP的数组                         */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_GetIP(int timeout)
{
	char *presult1,*presult2;
	char ip[4];
	
	WiFi_RxCounter=0;                               //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //清空WiFi接收缓冲区 
	WiFi_printf("AT+CIFSR\r\n");                    //发送指令	
	while(timeout--){                               //等待超时时间到0
		Delay_Ms(100);                              //延时100ms
		if(strstr(WiFi_RX_BUF,"OK"))                //如果接收到OK表示成功
			break;       						    //主动跳出while循环
		u4_printf("%d ",timeout);                   //串口输出现在的超时时间
	}
	u4_printf("\r\n");                              //串口输出信息
	if(timeout<=0)return 1;                         //如果timeout<=0，说明超时时间到了，也没能收到OK，返回1
	else{
		presult1 = strstr(WiFi_RX_BUF,"\"");
		if( presult1 != NULL ){
			presult2 = strstr(presult1+1,"\"");
			if( presult2 != NULL ){
				memcpy(ip,presult1+1,presult2-presult1-1);
				u4_printf("ESP8266_IP：%s\r\n",ip);     //串口显示IP地址
				return 0;    //正确返回0
			}else return 2;  //未收到预期数据
		}else return 3;      //未收到预期数据	
	}
}
/*-------------------------------------------------*/
/*函数名：获取连接状态                             */
/*参  数：无                                       */
/*返回值：连接状态                                 */
/*        0：无状态                                */
/*        1：有客户端接入                          */
/*        2：有客户端断开                          */
/*-------------------------------------------------*/
char WiFi_Get_LinkSta(void)
{
	char id_temp[10]={0};    //缓冲区，存放ID
	char sta_temp[10]={0};   //缓冲区，存放状态
	
	if(strstr(WiFi_RX_BUF,"CONNECT")){                 //如果接受到CONNECT表示有客户端连接	
		sscanf(WiFi_RX_BUF,"%[^,],%[^,]",id_temp,sta_temp);
		u4_printf("With client access,ID=%s\r\n",id_temp);  //有客户端接入 串口显示信息  
		WiFi_RxCounter=0;                              //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);        //清空WiFi接收缓冲区     
		return 1;                                      //有客户端接入
	}else if(strstr(WiFi_RX_BUF,"CLOSED")){            //如果接受到CLOSED表示有链接断开	
		sscanf(WiFi_RX_BUF,"%[^,],%[^,]",id_temp,sta_temp);
		u4_printf("A client is disconnected,ID=%s\r\n",id_temp);        //有客户端断开，串口显示信息
		WiFi_RxCounter=0;                                    //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);              //清空WiFi接收缓冲区     
		return 2;                                            //有客户端断开
	}else return 0;                                          //无状态改变	
}
/*-------------------------------------------------*/
/*函数名：获取客户端数据                           */
/*        两组回车换行符\r\n\r\n作为数据的结束符   */
/*参  数：data：数据缓冲区                         */
/*参  数：len： 数据量                             */
/*参  数：id：  发来数据的客户端的连接ID           */
/*返回值：数据状态                                 */
/*        0：无数据                                */
/*        1：有数据                                */
/*-------------------------------------------------*/
char WiFi_Get_Data(char *data, char *len, char *id)
{
	char temp[10]={0};      //缓冲区
	char *presult;

	if(strstr(WiFi_RX_BUF,"\r\n\r\n")){                     //两个连着的回车换行作为数据的结束符
		sscanf(WiFi_RX_BUF,"%[^,],%[^,],%[^:]",temp,id,len);//截取各段数据，主要是id和数据长度	
		presult = strstr(WiFi_RX_BUF,":");                  //查找冒号。冒号后的是数据
		if( presult != NULL )                               //找到冒号
			sprintf((char *)data,"%s",(presult+1));         //冒号后的数据，复制到data
		WiFi_RxCounter=0;                                   //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);             //清空WiFi接收缓冲区    
		return 1;                                           //有数据到来
	} else return 0;                                        //无数据到来
}
/*-------------------------------------------------*/
/*函数名：服务器发送数据                           */
/*参  数：databuff：数据缓冲区<2048                */
/*参  数：data_len：数据长度                       */
/*参  数：id：      客户端的连接ID                 */
/*参  数：timeout： 超时时间（10ms的倍数）         */
/*返回值：错误值                                   */
/*        0：无错误                                */
/*        1：等待发送数据超时                      */
/*        2：连接断开了                            */
/*        3：发送数据超时                          */
/*-------------------------------------------------*/
char WiFi_SendData(char id, char *databuff, int data_len, int timeout)
{    
	int i;
	
	WiFi_RxCounter=0;                                 //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //清空WiFi接收缓冲区 
	WiFi_printf("AT+CIPSEND=%d,%d\r\n",id,data_len);  //发送指令	
    while(timeout--){                                 //等待超时与否	
		Delay_Ms(10);                                 //延时10ms
		if(strstr(WiFi_RX_BUF,">"))                   //如果接收到>表示成功
			break;       						      //主动跳出while循环
		u4_printf("%d ",timeout);                     //串口输出现在的超时时间
	}
	if(timeout<=0)return 1;                                   //超时错误，返回1
	else{                                                     //没超时，正确       	
		WiFi_RxCounter=0;                                     //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);               //清空WiFi接收缓冲区 	
		for(i=0;i<data_len;i++)WiFi_printf("%c",databuff[i]); //发送数据	
		while(timeout--){                                     //等待超时与否	
			Delay_Ms(10);                                     //延时10ms
			if(strstr(WiFi_RX_BUF,"SEND OK")){                //如果接受SEND OK，表示发送成功			 
			WiFi_RxCounter=0;                                 //WiFi接收数据量变量清零                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //清空WiFi接收缓冲区 			
				break;                                        //跳出while循环
			} 
			if(strstr(WiFi_RX_BUF,"link is not valid")){      //如果接受link is not valid，表示连接断开			
				WiFi_RxCounter=0;                             //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);       //清空WiFi接收缓冲区 			
				return 2;                                     //返回2
			}
	    }
		if(timeout<=0)return 3;      //超时错误，返回3
		else return 0;	            //正确，返回0
	}	
}
/*-------------------------------------------------*/
/*函数名：连接TCP服务器，并进入透传模式            */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：0：正确  其他：错误                      */
/*-------------------------------------------------*/
char WiFi_Connect_Server(int timeout)
{	
	WiFi_RxCounter=0;                               //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //清空WiFi接收缓冲区   
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ServerIP,ServerPort);//发送连接服务器指令
	while(timeout--){                               //等待超时与否
#ifdef Blue_APP	
    Receive_BlueData();	
#endif
		Delay_Ms(100);                              //延时100ms	
		if(strstr(WiFi_RX_BUF ,"CONNECT"))          //如果接受到CONNECT表示连接成功
			break;                                  //跳出while循环
		if(strstr(WiFi_RX_BUF ,"CLOSED"))           //如果接受到CLOSED表示服务器未开启
			return 1;                               //服务器未开启返回1
		if(strstr(WiFi_RX_BUF ,"ALREADY CONNECTED"))//如果接受到ALREADY CONNECTED已经建立连接
			return 2;                               //已经建立连接返回2
		u4_printf("%d ",timeout);                   //串口输出现在的超时时间  

	}
	u4_printf("\r\n");                        //串口输出信息
	if(timeout<=0)return 3;                   //超时错误，返回3
	else                                      //连接成功，准备进入透传
	{
		u4_printf("Ready to go through\r\n");
//		u4_printf("准备进入透传\r\n");                  //串口显示信息
		WiFi_RxCounter=0;                               //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //清空WiFi接收缓冲区     
		WiFi_printf("AT+CIPSEND\r\n");                  //发送进入透传指令
		while(timeout--){                               //等待超时与否
			Delay_Ms(100);                              //延时100ms	
			if(strstr(WiFi_RX_BUF,"\r\nOK\r\n\r\n>"))   //如果成立表示进入透传成功
				break;                          //跳出while循环
			u4_printf("%d ",timeout);           //串口输出现在的超时时间  
		}
		if(timeout<=0)return 4;                 //透传超时错误，返回4	
	}
	return 0;	                                //成功返回0	
}
/*-------------------------------------------------*/
/*函数名：连接服务器                               */
/*参  数：无                                       */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_ConnectServer(void)
{	
	char res;
	char cnt;
	u4_printf("Prepare to reset the module\r\n");                     //准备复位模块  串口提示数据
	if(WiFi_Reset(50)){                                //复位，100ms超时单位，总计5s超时时间
		u4_printf("Reset failed, ready to restart\r\n");           //返回非0值，进入if，复位失败，准备重启 串口提示数据
		return 1;                                      //返回1
	}else u4_printf("Reset successfully\r\n");                   //串口提示数据  复位成功
	
	u4_printf("Ready to set STA mode\r\n");                  //  准备设置STA模式  串口提示数据
	if(WiFi_SendCmd("AT+CWMODE=1",50)){                //设置STA模式，100ms超时单位，总计5s超时时间
		u4_printf("Failed to set STA mode, ready to restart\r\n");    //返回非0值，进入if，设置STA模式失败，准备重启  串口提示数据
		return 2;                                      //返回2
	}else u4_printf("Set STA mode successfully\r\n");            //串口提示数据   设置STA模式成功
	
	if(wifi_mode==0){                                      //如果联网模式=0：SSID和密码写在程序里 
		u4_printf("Ready to cancel automatic connection\r\n");  //串口提示数据  准备取消自动连接
		if(WiFi_SendCmd("AT+CWAUTOCONN=0",50)){            //取消自动连接，100ms超时单位，总计5s超时时间
			u4_printf("Failed to cancel automatic connection, ready to restart\r\n");   //返回非0值，进入if，串口提示数据  取消自动连接失败，准备重启
			return 3;                                      //返回3
		}else u4_printf("Automatic connection cancelled successfully\r\n");           //串口提示数据  取消自动连接成功
				
		u4_printf("Ready to connect to router\r\n");      //串口提示数据	准备连接路由器
		if(WiFi_JoinAP(30)){                               //连接路由器,1s超时单位，总计30s超时时间
			u4_printf("Failed to connect to router, ready to restart\r\n");     //返回非0值，进入if，串口提示数据  连接路由器失败，准备重启
			return 4;                                      //返回4	
		}else u4_printf("Connect router successfully\r\n");  //串口提示数据		 连接路由器成功	
	}else{                                                 //如果联网模式=1：Smartconfig方式,用APP发送
		if(KEY==0)  //如果此时K2是按下的
		{     
			for(cnt=0;cnt<10;cnt++)
			{
        u4_printf("Ready to set up automatic connection\r\n");  //  准备设置自动连接
			  LED = ~LED; Delay_Ms(100);
			}
			if(WiFi_SendCmd("AT+CWAUTOCONN=1",50)){            //设置自动连接，100ms超时单位，总计5s超时时间
				u4_printf("Failed to set automatic connection, ready to restart\r\n");   //返回非0值，进入if，串口提示数据  设置自动连接失败，准备重启
				return 3;                                      //返回3
			}else u4_printf("Automatic connection set successfully\r\n");           //串口提示数据	 设置自动连接成功
			
			u4_printf("Ready to open Smartconfig\r\n");              //串口提示数据 准备开启
			if(WiFi_SendCmd("AT+CWSTARTSMART",50)){            //开启Smartconfig，100ms超时单位，总计5s超时时间
				u4_printf("Failed to open smartconfig, ready to restart\r\n");//返回非0值，进入if，串口提示数据  开启Smartconfig失败，准备重启
				return 4;                                      //返回4
			}else u4_printf("Smartconfig enabled successfully\r\n");        //串口提示数据  开启Smartconfig成功

			u4_printf("Please use app software to transfer password\r\n");            //串口提示数据 请使用APP软件传输密码
			if(WiFi_Smartconfig(60)){                          //APP软件传输密码，1s超时单位，总计60s超时时间
				u4_printf("Failed to transfer password, ready to restart\r\n");       //返回非0值，进入if，串口提示数据  传输密码失败，准备重启
				return 5;                                      //返回5
			}else u4_printf("Password transferred successfully\r\n");               //串口提示数据  传输密码成功

			u4_printf("Ready to close Smartconfig\r\n");              //串口提示数据  准备关闭
			if(WiFi_SendCmd("AT+CWSTOPSMART",50)){             //关闭Smartconfig，100ms超时单位，总计5s超时时间
				u4_printf("Failed to close smartconfig, ready to restart\r\n");//返回非0值，进入if，串口提示数据  关闭Smartconfig失败，准备重启
				return 6;                                      //返回6
			}else u4_printf("Smartconfig closed successfully\r\n");        //串口提示数据  关闭Smartconfig成功
		}
		else
		{                                                 //反之，此时K2是没有按下  
			u4_printf("Waiting to connect to router\r\n");                   //串口提示数据	  等待连接路由器
			if(WiFi_WaitAP(30)){                               //等待连接路由器,1s超时单位，总计30s超时时间
				u4_printf("Failed to connect to router, ready to restart\r\n");     //返回非0值，进入if，串口提示数据  连接路由器失败，准备重启
				return 7;                                      //返回7	
			}else u4_printf("Connect router successfully\r\n");             //串口提示数据  连接路由器成功					
		}
	}
	
	u4_printf("Ready to get IP address\r\n");                   //串口提示数据  准备获取IP地址
	if(WiFi_GetIP(50)){                                //准备获取IP地址，100ms超时单位，总计5s超时时间
		u4_printf("Failed to get IP address, ready to restart\r\n");     //返回非0值，进入if，串口提示数据  获取IP地址失败，准备重启
		return 10;                                     //返回10
	}else u4_printf("IP address obtained successfully\r\n");             //串口提示数据  获取IP地址成功
	
	u4_printf("Ready to turn on teleport\r\n");                     //串口提示数据  准备开启透传
	if(WiFi_SendCmd("AT+CIPMODE=1",50)){               //开启透传，100ms超时单位，总计5s超时时间
		u4_printf("Failed to open transparent transmission, ready to restart\r\n");       //返回非0值，进入if，串口提示数据  开启透传失败，准备重启
		return 11;                                     //返回11
	}else u4_printf("Successfully closed transparent transmission\r\n");               //串口提示数据  关闭透传成功
	
	u4_printf("Ready to close multi way connection\r\n");                 //串口提示数据  准备关闭多路连接
	if(WiFi_SendCmd("AT+CIPMUX=0",50)){                //关闭多路连接，100ms超时单位，总计5s超时时间
		u4_printf("Failed to close multi-channel connection, ready to restart\r\n");   //返回非0值，进入if，串口提示数据  关闭多路连接失败，准备重启
		return 12;                                     //返回12
	}else u4_printf("Multiple connection closed successfully\r\n");           //串口提示数据  关闭多路连接成功
	
	u4_printf("Ready to connect to server\r\n");                   //串口提示数据  准备连接服务器
	res = WiFi_Connect_Server(100);                    //连接服务器，100ms超时单位，总计10s超时时间
	if(res==1){                						   //返回1，进入if
		u4_printf("The server is not turned on, ready to restart\r\n");       //串口提示数据  服务器未开启，准备重启
		return 13;                                     //返回13
	}else if(res==2){                                  //返回2，进入if
		u4_printf("Connection already exists\r\n");                 //串口提示数据  连接已经存在
	}else if(res==3){								   //返回3，进入if
		u4_printf("Connection to server timed out, ready to restart\r\n");     //串口提示数据  连接服务器超时，准备重启
		return 14;                                     //返回14
	}else if(res==4){								   //返回4，进入if，
		u4_printf("Failed to enter transparent transmission\r\n");                 //串口提示数据  进入透传失败
		return 15;                                     //返回15
	}	
	u4_printf("Successfully connected to the server\r\n");                   //串口提示数据   连接服务器成功
	return 0;                                          //正确返回0	
}




void Wifi_Receive_UseData(void)
{
    char i;
    if(Usart2_RxCompleted==1)    //如果Usart2_RxCompleted等于1，表示接收数据完成
		{
			Usart2_RxCompleted = 0;    //清除标志位
			for(i=2;i<27;i++)
			{
				Receive_String[i-2] = Wifi_Data_buff[i];		
			}
	    if(Receive_String[0]==0XEF && Receive_String[24]==0X1D && Receive_String[1]==0X06)   //如果接收数据量大于0
			{
				for(i=0;i<25;i++)
				{
					USART_SendData(USART2, Receive_String[i]);   //将接收数据原样返回给服务端
					Delay_Ms(1);			
				}
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
				Sensor.PM2_5_Waring = Receive_String[20]<<8;
				Sensor.PM2_5_Waring = (Sensor.PM2_5_Waring + Receive_String[21]);
						
				u4_printf("---wifi_esp8266--->%04d %3.2f'C %3.2f%% %03d \r\n",Sensor.DEV_Num,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
			}
		} 
}


void Wifi_Send_UseData(void)
{
  char i;
	u32 chack_sum = 0;
	
	    Sensor.Year = calendar.w_year;
			Sensor.Month = calendar.w_month;
	    Sensor.Day = calendar.w_date;
	
			Send_String[0] = 0xEF;  //包头
//			if((Sensor.Temperature_Waring < Sensor.Temperature)||(Sensor.Humidity_Waring > Sensor.Humidity)||(Sensor.Smoke_Dev==1))
//			Send_String[1] = 0X05;  //数据类型
//			else
			Send_String[1] = 0X04;  //数据类型
			Send_String[2] = 'M';   //烟雾、 温湿度混合型探测器
			Send_String[3] = 'E';   //防爆
			Send_String[4] = 'I';   //智能型
	
//	    Send_String[5]   = 0X30+Sensor.Year/1000%10;
//			Send_String[6]   = 0X30+Sensor.Year/100%10;
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
 
//			Send_String[15]  = 0X13;//火灾预警

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
			
  	  for(i=0;i<25;i++)
			{
			  USART_SendData(USART2, Send_String[i]);
				Delay_Ms(1); 
			}
//      WiFi_printf("您发送的数据数:%s\r\n",&Wifi_Data_buff[2]);	  //把接收到的数据，返回给服务器
}





















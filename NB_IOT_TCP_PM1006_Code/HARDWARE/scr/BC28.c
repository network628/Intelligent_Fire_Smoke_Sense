///AT+NSOCL=0
#include "usart4.h"
#include "usart1.h"       //包含需要的头文件
#include "BC28.h"
#include "main.h"
#include "string.h"
#include "24c02.h" 		    //包含需要的头文件
char *strx,*extstrx;
char atstr[BUFLEN];
int err;    //全局变量
BC28 BC28_Status;

//char *SERVERIP = "149.235.207.142";           //存放服务器IP或是域名
//int  SERVERPORT = 8001;          //存放服务器的端口号区
char *SERVERIP = "192.168.0.99";           //存放服务器IP或是域名
int  SERVERPORT = 8001;          //存放服务器的端口号区
//#define SERVERIP "59.110.65.82"
//#define SERVERPORT 8001

//UART_BUF buf_uart2;     //NBIOT

void Clear_Buffer(void)//清空串口2缓存
{
	 if(Time3_Count%200==0)
    u4_printf(buf_uart2buf);  //清空前打印信息
	  Check_TCP();
    Delay(300);
    buf_uart2index=0;
    memset(buf_uart2buf,0,BUFLEN);
}

int BC28_Init(void)
{
    int errcount = 0;
    err = 0;    //判断模块卡是否就绪最重要
    u4_printf("start init bc28\r\n");
    Uart2_SendStr("ATE1\r\n");
    Delay(300);
    u4_printf(buf_uart2buf);      //打印收到的串口信息
    u4_printf("get back bc28\r\n");
    strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
    Clear_Buffer();	
    while(strx==NULL)
    {
        u4_printf("\r\n单片机正在连接到模块...\r\n");
        Clear_Buffer();	
        Uart2_SendStr("AT\r\n");
        Delay(300);
        strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
    }
    Uart2_SendStr("AT+CMEE=1\r\n"); //允许错误值
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
    Clear_Buffer();	

    Uart2_SendStr("AT+NBAND?\r\n"); //判断我们的NB模块BAND
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
    if(strx)
    {
        u4_printf("======== BAND========= \r\n %s \r\n",buf_uart2buf);
        Clear_Buffer();
        Delay(300);
    }
    
    Uart2_SendStr("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"ERROR");//只要卡不错误 基本就成功
    if(strx==NULL)
    {
        u4_printf("我的卡号是 : %s \r\n",buf_uart2buf);
        Clear_Buffer();	
        Delay(300);
    }
    else
    {
        err = 1;
        u4_printf("卡错误 : %s \r\n",buf_uart2buf);
        Clear_Buffer();
        Delay(300);
    }

    Uart2_SendStr("AT+CGATT=1\r\n");//激活网络，PDP
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返OK
    Clear_Buffer();	
    if(strx)
    {
        Clear_Buffer();	
        u4_printf("init PDP OK\r\n");
        Delay(300);
    }
    Uart2_SendStr("AT+CGATT?\r\n");//查询激活状态
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"+CGATT:1");//返1 表明激活成功 获取到IP地址了
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        Clear_Buffer();	
        Uart2_SendStr("AT+CGATT?\r\n");//获取激活状态
        Delay(300);
        strx=strstr((const char*)buf_uart2buf,(const char*)"+CGATT:1");//返回1,表明注网成功
        if(errcount>100)     //防止死循环
        {
            err=1;
            errcount = 0;
            break;
        }
    }


    Uart2_SendStr("AT+CSQ\r\n");//查看获取CSQ值
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"+CSQ");//返回CSQ
    if(strx)
    {
        u4_printf("信号质量:%s\r\n",buf_uart2buf);
        Clear_Buffer();
        Delay(300);
    }

    Uart2_SendStr("AT+CEREG?\r\n");
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"+CEREG:0,1");//返回注册状态
    extstrx=strstr((const char*)buf_uart2buf,(const char*)"+CEREG:1,1");//返回注册状态
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL&&extstrx==NULL)//两个返回值都没有
    {
        errcount++;
        Clear_Buffer();
        Uart2_SendStr("AT+CEREG?\r\n");//判断运营商
        Delay(300);
        strx=strstr((const char*)buf_uart2buf,(const char*)"+CEREG:0,1");//返回注册状态
        extstrx=strstr((const char*)buf_uart2buf,(const char*)"+CEREG:1,1");//返回注册状态
        if(errcount>100)     //防止死循环
        {
            err=1;
            errcount = 0;
            break;
        }
    }
		u4_printf("BC28_Init OK  \r\n");  //清空前打印信息
    return err;
}

void BC28_PDPACT(void)//激活场景，为连接服务器做准备
{
    int errcount = 0;
    Uart2_SendStr("AT+CGDCONT=1,\042IP\042,\042HUAWEI.COM\042\r\n");//设置APN
    Delay(300);
    Uart2_SendStr("AT+CGATT=1\r\n");//激活场景
    Delay(300);
    Uart2_SendStr("AT+CGATT?\r\n");//激活场景
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)" +CGATT:1");//注册上网络信息
    Clear_Buffer();	
    while(strx==NULL)
    {
        errcount++;
        Clear_Buffer();
        Uart2_SendStr("AT+CGATT?\r\n");//激活场景
        Delay(300);
        strx=strstr((const char*)buf_uart2buf,(const char*)"+CGATT:1");//一定要终端入网
        if(errcount>100)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }
    Uart2_SendStr("AT+CSCON?\r\n");//判断连接状态，返回1就是成功
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"+CSCON:0,1");//注册上网络信息
    extstrx=strstr((const char*)buf_uart2buf,(const char*)"+CSCON:0,0");//注册上网络信息
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL&&extstrx==NULL)    //连接不能反馈错误
    {
        errcount++;
        Clear_Buffer();
        Uart2_SendStr("AT+CSCON?\r\n");//
        Delay(300);
        strx=strstr((const char*)buf_uart2buf,(const char*)"+CSCON:0,1");//
        extstrx=strstr((const char*)buf_uart2buf,(const char*)"+CSCON:0,0");//
        if(errcount>100)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }
    u4_printf("BC28_PDPACT sever OK  \r\n");  //清空前打印信息
}

void BC28_ConTCP(void)
{
    int errcount = 0;
    Uart2_SendStr("AT+NSOCL=1\r\n");//关闭socekt连接
    Uart2_SendStr("AT+NSOCL=2\r\n");//关闭socekt连接
    Uart2_SendStr("AT+NSOCL=3\r\n");//关闭socekt连接
    Delay(300);
    Clear_Buffer();	
    Uart2_SendStr("AT+NSOCR=STREAM,6,0,1\r\n");//创建一个TCP Socket
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
        if(errcount>10)     //防止死循环
        {
            errcount = 0;
            break;
        }

    }
    Clear_Buffer();

    memset(atstr,0,BUFLEN);
    //AT+NSOCO=1,123.57.41.13,1001
    sprintf(atstr,"AT+NSOCO=1,%s,%d\r\n",SERVERIP,SERVERPORT);
    Uart2_SendStr(atstr);//发送0 socketIP和端口后面跟对应数据长度以及数据
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
        if(errcount>10)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }
		u4_printf("BC28_ConTCP OK  \r\n");  //清空前打印信息
    Clear_Buffer();	
}

void BC28_Senddata(uint8_t *len,uint8_t *data)
{
    int errcount=0;
    memset(atstr,0,BUFLEN);
    //AT+NSOSD=1,4,31323334
    sprintf(atstr,"AT+NSOSD=1,%s,%s\r\n",len,data);
    Uart2_SendStr(atstr);//发送0 socketIP和端口后面跟对应数据长度以及数据
    Delay(300);
    strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2buf,(const char*)"OK");//返回OK
        if(errcount>100)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }
    Clear_Buffer();	
//		u4_printf("---BC28_Senddata---OK  \r\n");  //清空前打印信息
}

	  char sendata[100];
	  char tempstr[100];
void NB_iot_Send_Data(void)
{ 
		memset(sendata,0,100);
		strcat(sendata,"EF");   //包头  1

		memset(tempstr,0,100);
		sprintf(tempstr,"%02x", Send_String[1]);   //数据类型  2
		strcat(sendata,tempstr);


		memset(tempstr,0,100);
		sprintf(tempstr,"%02x%02x%02x", Send_String[2],Send_String[3],Send_String[4]);    
		//烟雾、 温湿度混合型探测器  3   //防爆 4  //智能型 5
		strcat(sendata,tempstr);

		memset(tempstr,0,100);
		sprintf(tempstr,"%02x%02x%02x%02x%02x%02x", Send_String[5],Send_String[6],Send_String[7],Send_String[8],Send_String[9],Send_String[10]);  
		//出厂日期 年  6 7  //出厂日期 月 8 9  //出厂日期 日 10 11
		strcat(sendata,tempstr);

		memset(tempstr,0,100);
		sprintf(tempstr,"%02x%02x%02x%02x",Send_String[11],Send_String[12],Send_String[13],Send_String[14]);
		//设备号
		strcat(sendata,tempstr);

		memset(tempstr,0,100);
		sprintf(tempstr,"%02x",Send_String[15]);  //00：设备类型
		strcat(sendata,tempstr);

		memset(tempstr,0,100);
		sprintf(tempstr,"%02x%02x",Send_String[16],Send_String[17]);     //温度
		strcat(sendata,tempstr);
																 
		memset(tempstr,0,100);
		sprintf(tempstr,"%02x%02x",Send_String[18],Send_String[19]);   		//湿度
		strcat(sendata,tempstr);

		memset(tempstr,0,100);
		sprintf(tempstr,"%02x%02x",Send_String[20],Send_String[21]);   		//烟感 & PM2.5
		strcat(sendata,tempstr);

		memset(tempstr,0,100);
		sprintf(tempstr,"%02x%02x%02x",Send_String[22],Send_String[23],Send_String[24]);  //00：设备状态  //校验码  //包尾（结束符）
		strcat(sendata,tempstr);
 
		BC28_Senddata("25",(u8 *)sendata);  
 
 		Uart2_SendStr("AT+NSORF=1,100\r\n");
//    u4_printf("---NB_iot_Send_Data---OK  \r\n");  //清空前打印信息
}
/////////////////////////////////////////////////////////////
void Check_TCP(void)
{
  strx=strstr((const char*)buf_uart2buf,(const char*)"AT+NSOSD=");   //BC26有数据发出
	if(strx)
	{
//		u4_printf("---NB_Check_TCP1---OK  \r\n");
		strx=strstr((const char*)buf_uart2buf,(const char*)"+CME ERROR:");   //BC26数据发出失败
		if(strx) 
		{
//			u4_printf("---NB_Check_TCP2---OK  \r\n");
			while(1);
		}
	}
}
/////////////////////////////////////////////////////////////
void Ascll2Hex(u8 *s1, u8 *s2, u16 len)
{
    u8 Hex[] = {"0123456789ABCDEF"};
    u8 DataL,DataH;
    u16 i;
    for(i = 0; i < len; i++)
    {
        DataL = s2[i] &0x0F;
        DataH = (s2[i] >> 4)&0x0F;
        s1[2*i] = Hex[DataH];
        s1[2*i + 1] = Hex[DataL];
    }
}

char Ascll_HEX(char str)
{
   char x,data;
	 x = str;
	switch(x)
	{
		case '0': data = 0x00; break;
		case '1': data = 0x01; break;
		case '2': data = 0x02; break;
		case '3': data = 0x03; break;
		case '4': data = 0x04; break;
		case '5': data = 0x05; break;
		case '6': data = 0x06; break;
		case '7': data = 0x07; break;
		case '8': data = 0x08; break;
		case '9': data = 0x09; break;
		case 'A': data = 0x0A; break; //
		case 'B': data = 0x0B; break;
		case 'C': data = 0x0C; break;
		case 'D': data = 0x0D; break; //
		case 'E': data = 0x0E; break; //E
		case 'F': data = 0x0F; break; //F
		case 'a': data = 0x0a; break; //
		case 'b': data = 0x0b; break;
		case 'c': data = 0x0c; break;
		case 'd': data = 0x0d; break; //
		case 'e': data = 0x0e; break;  
		case 'f': data = 0x0f; break; 
		default : break;
	}
	
	return data;
}

/*
+NSONMI:0,4
AT+NSORF=0,4
0,47.104.238.90,1001,4,31323334,0

OK*/
void BC28_RECData(void)
{

		strx=strstr((const char*)buf_uart2buf,(const char*)"EF064D4549000000");
		if(strx)
		{
			Receive_String[0] = (Ascll_HEX(strx[0])<<4) | Ascll_HEX(strx[1]);
			Receive_String[1] = (Ascll_HEX(strx[2])<<4) | Ascll_HEX(strx[3]);
			Receive_String[2] = (Ascll_HEX(strx[4])<<4) | Ascll_HEX(strx[5]);
			Receive_String[3] = (Ascll_HEX(strx[6])<<4) | Ascll_HEX(strx[7]);
			Receive_String[4] = (Ascll_HEX(strx[8])<<4) | Ascll_HEX(strx[9]);
			Receive_String[5] = (Ascll_HEX(strx[10])<<4) | Ascll_HEX(strx[11]);
			Receive_String[6] = (Ascll_HEX(strx[12])<<4) | Ascll_HEX(strx[13]);
			Receive_String[7] = (Ascll_HEX(strx[14])<<4) | Ascll_HEX(strx[15]);
			Receive_String[8] = (Ascll_HEX(strx[16])<<4) | Ascll_HEX(strx[17]);
			Receive_String[9] = (Ascll_HEX(strx[18])<<4) | Ascll_HEX(strx[19]);
			Receive_String[10] = (Ascll_HEX(strx[20])<<4) | Ascll_HEX(strx[21]);
			Receive_String[11] = (Ascll_HEX(strx[22])<<4) | Ascll_HEX(strx[23]);  //设备号解析 11 12 13 14
			Receive_String[12] = (Ascll_HEX(strx[24])<<4) | Ascll_HEX(strx[25]);
			Receive_String[13] = (Ascll_HEX(strx[26])<<4) | Ascll_HEX(strx[27]);
			Receive_String[14] = (Ascll_HEX(strx[28])<<4) | Ascll_HEX(strx[29]);
			Receive_String[16] = (Ascll_HEX(strx[32])<<4) | Ascll_HEX(strx[33]);   //温度解析  16 17
			Receive_String[17] = (Ascll_HEX(strx[34])<<4) | Ascll_HEX(strx[35]);
			Receive_String[18] = (Ascll_HEX(strx[36])<<4) | Ascll_HEX(strx[37]);   // 
			Receive_String[19] = (Ascll_HEX(strx[38])<<4) | Ascll_HEX(strx[39]);
			Receive_String[20] = (Ascll_HEX(strx[40])<<4) | Ascll_HEX(strx[41]);   // 
			Receive_String[21] = (Ascll_HEX(strx[42])<<4) | Ascll_HEX(strx[43]);
			Receive_String[22] = (Ascll_HEX(strx[44])<<4) | Ascll_HEX(strx[45]);
			Receive_String[23] = (Ascll_HEX(strx[46])<<4) | Ascll_HEX(strx[47]);
			Receive_String[24] = (Ascll_HEX(strx[48])<<4) | Ascll_HEX(strx[49]);
			
      memset(strx,0,50);
			
	    if(Receive_String[0]==0XEF && Receive_String[24]==0X1D && Receive_String[1]==0X06)   //如果接收数据量大于0
			{

				
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
				
//				Uart2_SendStr("AT+NSORF=1,100\r\n");
//				BC28_Senddata("7","NB_IOT_SET__OK");  
				u4_printf("---NB_IOT_BC26--->%04d %3.2f'C %3.2f%% %03d \r\n",Sensor.DEV_Num,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
			}
			
 			Uart2_SendStr("AT+NSORF=1,100\r\n");
// 			for(i=0;i<25;i++)
//			{
//			  USART_SendData(USART1, Receive_String[i]);
//				Delay_Ms(1); 
//			}
		}
}
 




/******************** (C) COPYRIGHT  源地工作室 ********************************
 * 文件名  ：SHT2X.c
 * 描述    ：初始化SHT20及一些基本的操作 
 * 作者    ：zhuoyingxingyu
 * 淘宝    ：源地工作室http://vcc-gnd.taobao.com/
 * 论坛地址：极客园地-嵌入式开发论坛http://vcc-gnd.com/
 * 版本更新: 2015-10-20
 * 硬件连接: PB6-I2C1_SCL、PB7-I2C1_SDA
 * 调试方式：J-Link-OB
**********************************************************************************/
 
//头文件
#include "PM1006.h"
#include "usart1.h"       //包含需要的头文件
#include "math.h"
#include "i2c.h"
#include "delay.h"
#include "main.h"         //包含需要的头文件

//#define SLAVE_ADD 0x28 //丛机地址
#define SLAVE_ADD_RD 0x51 //丛机地址RD
#define SLAVE_ADD_WR 0x50 //丛机地址WR
#define IIC_BUFF_MAX 25 //I2C接收和发送的最大字节数
/********************************************
//功能:I2C发送多个字节数据
输入参数：cINSendDat[IIC_BUFF_MAX] 发送数据数组
输入参数：Send_Num 发送字节数
1、发送START条件
1、发送地址+WR 读ACK
2、发送DATA_1 读ACK
.....
3、发送DATA_n 读ACK
4、发送STOP条件
*********************************************/
void I2CSendDatas(unsigned char cINSendDat[IIC_BUFF_MAX], unsigned char Send_Num)
{
	unsigned char i;
	unsigned char SendChar;
	if(Send_Num >= IIC_BUFF_MAX)
	{
		Send_Num = IIC_BUFF_MAX;
	}
	I2C_Start();
	I2C_SendByte(0x50);   /* 设置低起始地址 */      
	I2C_WaitAck();
 
	for ( i = 0; i < Send_Num; i++ )
	{
		SendChar = cINSendDat[i];
		I2C_SendByte(SendChar); //发送1个字节
		Delay_Us(50);
		I2C_Ack();
		Delay_Us(50);
		SDA_L;
	}
	I2C_Stop();
}

/********************************************
//功能:I2C应用Demo程序 发送控制命令
*********************************************/
void I2CDemoSendCmd(void)
{
//	unsigned char i; 11 03 0C 02 1E C0
	unsigned char I2CSendBuff[7]={0x16, 0x07, 0x03, 0xFF, 0xFF, 0x00, 0x12}; //定义发送数据缓冲区
	I2CSendDatas(I2CSendBuff,7);//发送控制命令
	Delay_Ms(20);
}



/********************************************
功能:I2C接收多个字节数据
输入参数：cINRcvDat[IIC_BUFF_MAX] 接收数据数组
输入参数：Rcv_Num 接收字节数
1、发送START条件
1、发送地址+RD 读ACK
2、接收DATA_1 写ACK
.....
3、接收DATA_n 写ACK
4、发送STOP条件
*********************************************/
void I2CRcvDatas(unsigned char cINRcvDat[IIC_BUFF_MAX], unsigned char Rcv_Num)
{
	unsigned char i;
	if(Rcv_Num >= IIC_BUFF_MAX) Rcv_Num = IIC_BUFF_MAX;
	I2C_Start();
	I2C_SendByte(0x51);
	I2C_WaitAck();
 
	Delay_Us(500);
	//=================读取数据======================
	for (i = 0;i < Rcv_Num; i++)
	{
		cINRcvDat[i] = I2C_ReceiveByte(); // 读取一个字节
		Delay_Us(50);
		if(i== Rcv_Num - 1) I2C_NoAck();
		else I2C_Ack();
		Delay_Us(50);
		Delay_Us(50);
	}
	SDA_H;
	I2C_Stop();
}

u32 Time3_Count;
/********************************************
//功能:I2C应用Demo程序 读取PM1006浓度值
*********************************************/
int I2CDemoReadPM(void)
{
//	unsigned char i;
	int PM_Temp;
	char Temp_H, Temp_L;
  unsigned char I2CRecvBuff[22]; //定义接收数据缓冲区
	I2CRcvDatas(I2CRecvBuff,22);//接收模块应答的数据 长度为22个字节

//	for(i=0;i<22;i++)
//	{
//		putchar(I2CRecvBuff[i]); //串口打印测试输出 (有串口输出时使用)
//		delay_ms(1);
//	}
	 Temp_H = I2CRecvBuff[5]<<8;
	 Temp_L = I2CRecvBuff[6];
	 PM_Temp = Temp_H + Temp_L;
	
//	if(Time3_Count%20>15) 
//		{
//	for( i=0;i<22;i++)
//	{
//		u1_printf("%x ",I2CRecvBuff[i]);
//	}
//	u1_printf("\r\n");
//	
////		   u1_printf("%x %x %dug/m3 \r\n",Temp_H, Temp_L, PM_Temp);
//		}
	return PM_Temp;
} 
 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

/******************** (C) COPYRIGHT  Դ�ع����� ********************************
 * �ļ���  ��SHT2X.c
 * ����    ����ʼ��SHT20��һЩ�����Ĳ��� 
 * ����    ��zhuoyingxingyu
 * �Ա�    ��Դ�ع�����http://vcc-gnd.taobao.com/
 * ��̳��ַ������԰��-Ƕ��ʽ������̳http://vcc-gnd.com/
 * �汾����: 2015-10-20
 * Ӳ������: PB6-I2C1_SCL��PB7-I2C1_SDA
 * ���Է�ʽ��J-Link-OB
**********************************************************************************/
 
//ͷ�ļ�
#include "PM1006.h"
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "math.h"
#include "i2c.h"
#include "delay.h"
#include "main.h"         //������Ҫ��ͷ�ļ�

//#define SLAVE_ADD 0x28 //�Ի���ַ
#define SLAVE_ADD_RD 0x51 //�Ի���ַRD
#define SLAVE_ADD_WR 0x50 //�Ի���ַWR
#define IIC_BUFF_MAX 25 //I2C���պͷ��͵�����ֽ���
/********************************************
//����:I2C���Ͷ���ֽ�����
���������cINSendDat[IIC_BUFF_MAX] ������������
���������Send_Num �����ֽ���
1������START����
1�����͵�ַ+WR ��ACK
2������DATA_1 ��ACK
.....
3������DATA_n ��ACK
4������STOP����
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
	I2C_SendByte(0x50);   /* ���õ���ʼ��ַ */      
	I2C_WaitAck();
 
	for ( i = 0; i < Send_Num; i++ )
	{
		SendChar = cINSendDat[i];
		I2C_SendByte(SendChar); //����1���ֽ�
		Delay_Us(50);
		I2C_Ack();
		Delay_Us(50);
		SDA_L;
	}
	I2C_Stop();
}

/********************************************
//����:I2CӦ��Demo���� ���Ϳ�������
*********************************************/
void I2CDemoSendCmd(void)
{
//	unsigned char i; 11 03 0C 02 1E C0
	unsigned char I2CSendBuff[7]={0x16, 0x07, 0x03, 0xFF, 0xFF, 0x00, 0x12}; //���巢�����ݻ�����
	I2CSendDatas(I2CSendBuff,7);//���Ϳ�������
	Delay_Ms(20);
}



/********************************************
����:I2C���ն���ֽ�����
���������cINRcvDat[IIC_BUFF_MAX] ������������
���������Rcv_Num �����ֽ���
1������START����
1�����͵�ַ+RD ��ACK
2������DATA_1 дACK
.....
3������DATA_n дACK
4������STOP����
*********************************************/
void I2CRcvDatas(unsigned char cINRcvDat[IIC_BUFF_MAX], unsigned char Rcv_Num)
{
	unsigned char i;
	if(Rcv_Num >= IIC_BUFF_MAX) Rcv_Num = IIC_BUFF_MAX;
	I2C_Start();
	I2C_SendByte(0x51);
	I2C_WaitAck();
 
	Delay_Us(500);
	//=================��ȡ����======================
	for (i = 0;i < Rcv_Num; i++)
	{
		cINRcvDat[i] = I2C_ReceiveByte(); // ��ȡһ���ֽ�
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
//����:I2CӦ��Demo���� ��ȡPM1006Ũ��ֵ
*********************************************/
int I2CDemoReadPM(void)
{
//	unsigned char i;
	int PM_Temp;
	char Temp_H, Temp_L;
  unsigned char I2CRecvBuff[22]; //����������ݻ�����
	I2CRcvDatas(I2CRecvBuff,22);//����ģ��Ӧ������� ����Ϊ22���ֽ�

//	for(i=0;i<22;i++)
//	{
//		putchar(I2CRecvBuff[i]); //���ڴ�ӡ������� (�д������ʱʹ��)
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

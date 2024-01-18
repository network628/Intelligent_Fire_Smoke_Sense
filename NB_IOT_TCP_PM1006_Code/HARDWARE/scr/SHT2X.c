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
#include "SHT2X.h"
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "math.h"
#include "i2c.h"
#include "main.h"         //������Ҫ��ͷ�ļ�

 float temperatureC;
 float humidityRH;
 
 u8 sndata1[8];
 u8 sndata2[6];
 u32 SN1; 
 u32 SN2; 

 /**
  * @file   Delay_us(����ȷ)
  * @brief  ΢����ʱ time_ms ms
  * @param  time_us ��ʱʱ��
  * @retval NO
  */
static void delay_us( uint16_t time_us )
{
  uint16_t i,j;
  for( i=0;i<time_us;i++ )
  {
		for( j=0;j<11;j++ );//1us
  }
}
 /**
  * @file   Delay_Ms(����ȷ)
  * @brief  ������ʱ time_ms ms
  * @param  time_ms ��ʱʱ��
  * @retval NO
  */
static void delay_ms(uint16_t time_ms)
{
	uint16_t i;
  for( i=0;i<time_ms;i++ )
  {
		delay_us(11000);
  }
	
}
/**
  * @file   SHT2X_IIC_WriteByte
  * @brief  ��SHT20дһ�ֽ�����
  * @param  
	*          - SendByte: ��д������
	*          - WriteAddress: ��д���ַ
  * @retval ����Ϊ:=1�ɹ�д��,=0ʧ��
  */
FunctionalState SHT2X_IIC_WriteByte(uint8_t WriteAddress,uint8_t SendByte)
{		
    if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(WriteAddress);   /* ���õ���ʼ��ַ */      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
    return ENABLE;
}	
 /**
  * @file   SHT2X_IIC_ReadByte
  * @brief  ��SHT20��ȡһ������
  * @param  
	*					- pBuffer: ��Ŷ�������
	*     	  - length: ����������
	*         - ReadAddress: ��������ַ
  * @retval ����Ϊ:=1�ɹ�����,=0ʧ��
  */
FunctionalState SHT2X_IIC_ReadByte( uint8_t ReadAddress, uint16_t length  ,uint8_t* pBuffer)
{		
	  if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); /* ���ø���ʼ��ַ+������ַ */ 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(ReadAddress);   /* ���õ���ʼ��ַ */      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(0x81);
    I2C_WaitAck();
    while(length)
    {
			*pBuffer = I2C_ReceiveByte();
			if(length == 1)
			I2C_NoAck();
			else I2C_Ack(); 
			pBuffer++;
			length--;
    }
    I2C_Stop();
    return ENABLE;
} 
 /**
  * @file   SHT2x_CheckCrc
  * @brief  calculates checksum for n bytes of data and compares it with expected
  * @param  
	*					- data[]: �ȴ����������
	*     	  - startByte: ��ʼ����ı��
	*         - nbrOfBytes: �������
	*         - checksum: ���յ���CRC����
  * @retval ����Ϊ:=1���ɹ�,=0���ʧ��
  */
FunctionalState SHT2x_CheckCrc(u8 data[],u8 startBytes,u8 number, u8 checksum)
{
	u8 bit=0;
	u8 crc = 0;	
  u8 byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = startBytes; byteCtr < startBytes+number; ++byteCtr)
  { crc ^= (data[byteCtr]);
    for (bit = 8; bit > 0; --bit)
    { if (crc & 0x80) crc = (crc << 1) ^ 0x131;
      else crc = (crc << 1);
    }
  }
  if (crc ==checksum) 
	return ENABLE;
  else
	return DISABLE;
}
 /**
  * @file   SHT2x_ReadUserRegister
  * @brief  reads the SHT2x user register 
  * @param  no
  * @retval ���ض�ȡ���Ĳ���ֵ
  */
u8 SHT2x_ReadUserRegister(void)
{
	u8 data[1]={0};
  SHT2X_IIC_ReadByte( USER_REG_R, 1 ,data);
  return data[0];	
}
 /**
  * @file   SHT2x_WriteUserRegister
  * @brief  writes the SHT2x user register (8bit)
  * @param  userdata��Ҫд�Ĳ���
  * @retval ENABLE���ɹ���DISABLE��ʧ��
  */
FunctionalState SHT2x_WriteUserRegister(u8 userdata)
{
	SHT2X_IIC_WriteByte(USER_REG_W ,userdata);
	if(userdata==SHT2x_ReadUserRegister())
	return ENABLE;
	else 
	return DISABLE;
}

 /**
  * @file   SHT2x_Calc_T
  * @brief  �����¶�
  * @param  NO
  * @retval �����¶�ֵ
  */
FunctionalState SHT2x_Calc_T(void)
{
	 u8 length=0;
	 u8 Tdata[3]={0};
	 if(!I2C_Start())return DISABLE;
    I2C_SendByte(I2C_ADR_W); 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(TRIG_T_MEASUREMENT_POLL);      
    I2C_WaitAck();
		delay_us(20);
	  I2C_Stop();
		do
		{
		I2C_Start();
    I2C_SendByte(I2C_ADR_R);
		}
		while(!I2C_WaitAck());
		for(length=0;length<=3;length++)
		{
			Tdata[length]=I2C_ReceiveByte();
			I2C_Ack(); 		
		};
		I2C_NoAck();		
		I2C_Stop();		
		if(((Tdata[0]+Tdata[1]+Tdata[2])>0)&&SHT2x_CheckCrc(Tdata,0,2,Tdata[2]))	
    temperatureC= (-46.85 + (175.72/65536 )*((float)((((u16)Tdata[0]<<8)+(u16)Tdata[1])&0xfffc)));
		else
		return DISABLE;
	  return ENABLE;
}


 /**
  * @file   SHT2x_Calc_RH
  * @brief  ����ʪ��
  * @param  NO
  * @retval ����ʪ��ֵ
  */
FunctionalState SHT2x_Calc_RH(void)
{	
   u8 length=0;
	 u8 RHdata[3]={0};
	 if(!I2C_Start())return DISABLE;
    I2C_SendByte(I2C_ADR_W); 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(TRIG_RH_MEASUREMENT_POLL);      
    I2C_WaitAck();
		delay_us(20);
	  I2C_Stop();
		do
		{
		I2C_Start();
    I2C_SendByte(I2C_ADR_R);
		}
		while(!I2C_WaitAck());
		for(length=0;length<=3;length++)
		{
			RHdata[length]=I2C_ReceiveByte();
			I2C_Ack(); 		
		};
		I2C_NoAck();		
		I2C_Stop();
		if(((RHdata[0]+RHdata[1]+RHdata[2])>0)&&SHT2x_CheckCrc(RHdata,0,2,RHdata[2]))		
	  humidityRH = -6.0 + 125.0/65536 * ((float)((((u16)RHdata[0]<<8)+(u16)RHdata[1])&0xfff0)); 
		else
		return DISABLE;
	  return ENABLE;
}
 /**
  * @file   SHT2x_SoftReset
  * @brief  �����λ
  * @param  NO
  * @retval NO
  */
FunctionalState SHT2x_SoftReset(void)
{
	 if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(SOFT_RESET);       
    I2C_WaitAck();	  
    I2C_Stop(); 
  	delay_ms(1500);
		return ENABLE;	
}

 /**
  * @file   SHT2x_GetSerialNumber
  * @brief  �õ�SHT20�����к�
  * @param  
  * @retval 
  */
u8 SHT2x_GetSerialNumber(u8 *pBuffer1,u8 *pBuffer2)
{   u8 length=8;
	  if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(0xfa);       
    I2C_WaitAck();	
    I2C_SendByte(0x0f);
    I2C_WaitAck();   
		if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x81); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    while(length)
    {
      *pBuffer1 = I2C_ReceiveByte();
      if(length == 1)
				I2C_NoAck();
      else I2C_Ack(); 
      pBuffer1++;
      length--;
    }
     I2C_Stop();
		length=6;
	  if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(0xfc);     
    I2C_WaitAck();	
    I2C_SendByte(0xc9);
    I2C_WaitAck();   
		if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x81); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    while(length)
    {
      *pBuffer2 = I2C_ReceiveByte();
      if(length == 1)
				I2C_NoAck();
      else I2C_Ack(); 
      pBuffer2++;
      length--;
    }
     I2C_Stop();	
    return ENABLE;
}
 /**
  * @file   SHT2X_Init
  * @brief  SHT20��ʼ��
  * @param  NO
  * @retval NO
  */
u32 SHT2X_Init(void)
{
	  u32 Num;
		SHT2x_GetSerialNumber(sndata1,sndata2);
		if((sndata1[0]+sndata1[1]+sndata1[3]+sndata1[4]+sndata1[5]+sndata1[6]+sndata1[7])>0)
		{		
		if(
		SHT2x_CheckCrc(sndata1,0,1,sndata1[1])&&
		SHT2x_CheckCrc(sndata1,2,1,sndata1[3])&&
		SHT2x_CheckCrc(sndata1,4,1,sndata1[5])&&
		SHT2x_CheckCrc(sndata1,6,1,sndata1[7])&&
		SHT2x_CheckCrc(sndata2,0,2,sndata2[2])&&
		SHT2x_CheckCrc(sndata2,3,2,sndata2[5])
		)
		{
//		printf("SHT2X CRC ok\r\n");
		SN1=((sndata2[3]<<24)+(sndata2[4]<<16)+(sndata1[0]<<8)+sndata1[2]);
		SN2=((sndata1[4]<<24)+(sndata1[6]<<16)+(sndata2[0]<<8)+sndata2[1]);
//		u4_printf("SHT2X SN:0x%x%x\r\n",SN1,SN2);
		Num = (SN1<<8)+SN2;
//		u4_printf("SHT2X SN:%d\r\n",Num);
		}
//		else
//		printf("SHT2X CRC error\r\n");	
		}
		else
		{
		SHT2x_GetSerialNumber(sndata1,sndata2);
//		printf("SHT2Xͨ�Ŵ���\r\n");
		}

//		if(SHT2x_WriteUserRegister(0x3a))
//		printf("���óɹ�\r\n");
//		else 
//		printf("���ô���\r\n");
		return Num;
}
 /**
  * @file   SHT2X_TEST
  * @brief  SHT20����
  * @param  NO
  * @retval NO
  */


void SHT2X_TEST(void)
{
	
		if(SHT2x_Calc_T())
		{ 
      Sensor.Temperature = temperatureC;        //SHT21 �¶�
//			 u4_printf("�¶ȣ�%f\r\n",temperatureC);
 
		}
    /*ʪ�Ȳ���*/
		if(SHT2x_Calc_RH())
		{
 	    Sensor.Humidity    = humidityRH;         //SHT21 ʪ��
//			u4_printf("ʪ�ȣ�%f%%\r\n",humidityRH);
		}
	

}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

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
#include "SHT2X.h"
#include "usart1.h"       //包含需要的头文件
#include "math.h"
#include "i2c.h"
#include "main.h"         //包含需要的头文件

 float temperatureC;
 float humidityRH;
 
 u8 sndata1[8];
 u8 sndata2[6];
 u32 SN1; 
 u32 SN2; 

 /**
  * @file   Delay_us(不精确)
  * @brief  微妙延时 time_ms ms
  * @param  time_us 延时时间
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
  * @file   Delay_Ms(不精确)
  * @brief  毫秒延时 time_ms ms
  * @param  time_ms 延时时间
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
  * @brief  向SHT20写一字节数据
  * @param  
	*          - SendByte: 待写入数据
	*          - WriteAddress: 待写入地址
  * @retval 返回为:=1成功写入,=0失败
  */
FunctionalState SHT2X_IIC_WriteByte(uint8_t WriteAddress,uint8_t SendByte)
{		
    if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(WriteAddress);   /* 设置低起始地址 */      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
    return ENABLE;
}	
 /**
  * @file   SHT2X_IIC_ReadByte
  * @brief  从SHT20读取一串数据
  * @param  
	*					- pBuffer: 存放读出数据
	*     	  - length: 待读出长度
	*         - ReadAddress: 待读出地址
  * @retval 返回为:=1成功读入,=0失败
  */
FunctionalState SHT2X_IIC_ReadByte( uint8_t ReadAddress, uint16_t length  ,uint8_t* pBuffer)
{		
	  if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); /* 设置高起始地址+器件地址 */ 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(ReadAddress);   /* 设置低起始地址 */      
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
	*					- data[]: 等待检验的数据
	*     	  - startByte: 开始数组的标号
	*         - nbrOfBytes: 检验个数
	*         - checksum: 接收到的CRC数据
  * @retval 返回为:=1检测成功,=0检测失败
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
  * @retval 返回读取到的参数值
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
  * @param  userdata：要写的参数
  * @retval ENABLE：成功，DISABLE：失败
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
  * @brief  计算温度
  * @param  NO
  * @retval 返回温度值
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
  * @brief  计算湿度
  * @param  NO
  * @retval 返回湿度值
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
  * @brief  软件复位
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
  * @brief  得到SHT20的序列号
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
  * @brief  SHT20初始化
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
//		printf("SHT2X通信错误\r\n");
		}

//		if(SHT2x_WriteUserRegister(0x3a))
//		printf("设置成功\r\n");
//		else 
//		printf("设置错误\r\n");
		return Num;
}
 /**
  * @file   SHT2X_TEST
  * @brief  SHT20测试
  * @param  NO
  * @retval NO
  */


void SHT2X_TEST(void)
{
	
		if(SHT2x_Calc_T())
		{ 
      Sensor.Temperature = temperatureC;        //SHT21 温度
//			 u4_printf("温度：%f\r\n",temperatureC);
 
		}
    /*湿度测试*/
		if(SHT2x_Calc_RH())
		{
 	    Sensor.Humidity    = humidityRH;         //SHT21 湿度
//			u4_printf("湿度：%f%%\r\n",humidityRH);
		}
	

}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

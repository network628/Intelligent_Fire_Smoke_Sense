
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ�ִ���1���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/
#include "main.h"         //������Ҫ��ͷ�ļ�
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "usart1.h"     //������Ҫ��ͷ�ļ�
#include "delay.h"        //������Ҫ��ͷ�ļ�

#if  USART1_RX_ENABLE                   //���ʹ�ܽ��չ���
char Usart1_RxCompleted = 0;            //����һ������ 0����ʾ����δ��� 1����ʾ������� 
unsigned int Usart1_RxCounter = 0;      //����һ����������¼����1�ܹ������˶����ֽڵ�����
char Usart1_RxBuff[USART1_RXBUFF_SIZE]; //����һ�����飬���ڱ��洮��1���յ�������   	
#endif

/*-------------------------------------------------*/
/*����������ʼ������1���͹���                      */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Usart1_Init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //����һ������GPIO���ܵı���
	USART_InitTypeDef USART_InitStructure;   //����һ�����ô��ڹ��ܵı���
#if USART1_RX_ENABLE                         //���ʹ�ܽ��չ���
	NVIC_InitTypeDef NVIC_InitStructure;     //���ʹ�ܽ��չ��ܣ�����һ�������жϵı���
#endif

#if USART1_RX_ENABLE                                 //���ʹ�ܽ��չ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3
#endif	
      
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);  //ʹ�ܴ���1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //ʹ��GPIOAʱ��
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              //׼������PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO����50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //����������������ڴ���1�ķ���
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA9
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //׼������PA10 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�������룬���ڴ���1�Ľ���
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA10
	
	USART_InitStructure.USART_BaudRate = bound;                                    //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
#if USART1_RX_ENABLE               												   //���ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //�շ�ģʽ
#else                                                                              //�����ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //ֻ��ģʽ
#endif        
    USART_Init(USART1, &USART_InitStructure);                                      //���ô���1	

#if USART1_RX_ENABLE  	         					        //���ʹ�ܽ���ģʽ
	USART_ClearFlag(USART1, USART_FLAG_RXNE);	            //������ձ�־λ
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          //���������ж�
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;       //���ô���1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //���ô���1�ж�
#endif  

	USART_Cmd(USART1, ENABLE);                              //ʹ�ܴ���1
}

/*-------------------------------------------------*/
/*������������1 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/

__align(8) char Usart1_TxBuff[USART1_TXBUFF_SIZE];  

void u1_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(Usart1_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)Usart1_TxBuff);		
	while((USART1->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART1->DR = Usart1_TxBuff[i];
		while((USART1->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*������������2�����жϺ���                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART1_IRQHandler(void)   
{                      
	  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����жϣ�������չ������
    {
 		  Usart1_RxBuff[Usart1_RxCounter] = USART_ReceiveData(USART1);//����ģ�������
			Usart1_RxCounter ++;  
		}
} 



void rs485_UartWrite(u8 *buf ,u8 len) 										//����
{
	u8 i=0;        
 
//	while((USART1->SR&0X40)==0);
	for(i = 0;i < len;i ++)
	{			
//		USART1->DR = buf[i];
//		while((USART1->SR&0X40)==0);	
		 USART_SendData(USART1, buf[i]);
		 Delay_Ms(1);
	}	
}
//////////////////////////////////////////////////////////////////////

//void Modbus_Get_Data(void)
//{
// 
//	if(strstr(Usart1_RxBuff,"hello"))
//	{                 
////    rs485_UartWrite((u8 *)Usart1_RxBuff ,10);
//		u4_printf("\r\nUsart1_RxBuff:%s\r\n",Usart1_RxBuff);   
//		Usart1_RxCounter=0;                                   //WiFi������������������                        
//		memset(Usart1_RxBuff,0,USART1_RXBUFF_SIZE);             //���WiFi���ջ�����    
//		                                       
//	}                                         
//}

unsigned int GetCRC16(unsigned char *ptr,  unsigned char len)
{ 
    u16 index;
    u8 crch = 0xFF;  //��CRC�ֽ�
    u8 crcl = 0xFF;  //��CRC�ֽ�
    u8  TabH[] = {  //CRC��λ�ֽ�ֵ��
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,  
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
        0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,  
        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,  
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,  
        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
        0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40  
    } ;  
    u8 TabL[] = {  //CRC��λ�ֽ�ֵ��
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,  
        0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,  
        0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,  
        0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,  
        0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,  
        0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,  
        0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,  
        0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,  
        0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,  
        0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,  
        0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,  
        0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,  
        0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,  
        0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,  
        0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,  
        0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,  
        0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,  
        0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,  
        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,  
        0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,  
        0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,  
        0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,  
        0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,  
        0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,  
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,  
        0x43, 0x83, 0x41, 0x81, 0x80, 0x40  
    } ;
 
    while (len--)  //����ָ������CRC
    {
        index = crch ^ *ptr++;
        crch = crcl ^ TabH[index];
        crcl = TabL[index];
    }
    
    return ((crch<<8) | crcl);  
} 
//////////////////////////////////////////////////////////////////////

//���㷢�͵����ݳ��ȣ����ҽ����ݷŵ�*buf������                     
void UartRead(u8 *buf)  
{
	 u8 i;
	if(Usart1_RxBuff[0]==0x01)
	{
		for(i=0;i<Usart1_RxCounter;i++)  //�������յ������ݵ�����ָ����
		{
			*buf=Usart1_RxBuff[i];  //�����ݸ��Ƶ�buf��
			buf++;
		}
		memset(Usart1_RxBuff,0,USART1_RXBUFF_SIZE); 
	}
	else
	{
		Usart1_RxCounter=0;       //���ռ���������
		memset(Usart1_RxBuff,0,USART1_RXBUFF_SIZE); 
	}
} 

unsigned char regGroup[5];  //Modbus�Ĵ����飬��ַΪ0x00~0x04
 u8 buf[60];
 u8 len;
void Modbus_Get_Data(void)
{
  unsigned char i=0,cnt;
	unsigned int crc;
	unsigned char crch,crcl;
	float Temperature=34.96;
	float Humidity=78.36;
	int PM2_5=645;
	
	 UartRead(buf);   //�����յ������������������		

	Temperature = Sensor.Temperature;
	Humidity = Sensor.Humidity;
	PM2_5 = Sensor.PM2_5;
	if(buf[0]==0x01)  //01 03 00 01 00 06 94 08
	{         
		  len = Usart1_RxCounter;
//		rs485_UartWrite(buf,Usart1_RxCounter);
	    crc=GetCRC16(buf,len-2);       //����CRCУ��ֵ����ȥCRCУ��ֵ
			crch=crc>>8;    				//crc��λ
			crcl=crc&0xFF;					//crc��λ
			if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //�ж�CRCУ���Ƿ���ȷ
			{
				Temperature*=100;
				Humidity*=100;
				switch (buf[1])  //��������ִ�в���
				{
					case 0x03:     //������
						if((buf[2]==0x00)&&(buf[3]<=0x05)) //�Ĵ�����ַ֧��0x0000~0x0005
						{
							
							if(buf[3]<=0x04) 
							{
								i=buf[3];//��ȡ�Ĵ�����ַ
								cnt=buf[5];  //��ȡ����ȡ�ļĴ�������
								buf[2]=cnt*2;  //��ȡ���ݵ��ֽ�����Ϊ�Ĵ���*2����modbus����ļĴ���Ϊ16λ
								len=3;
								if(((i==0x01)||(i==0x02)||(i==0x03)) && (cnt==0x01))  //��ȡ�����Ĵ���ֵ
								{
									  if(i==0x01)
										while(cnt--)
										{
											buf[len++]=((int)Temperature>>8)&0x00ff;				//�Ĵ������ֽڲ�0
		//									buf[len++]=regGroup[i++];		//���ֽ� 01234
											buf[len++]=((int)Temperature)&0x00ff;	;		//���ֽ� 01234
										}
										else if(i==0x02)
											while(cnt--)
										{
											buf[len++]=((int)Humidity>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=((int)Humidity)&0x00ff;	;		//���ֽ� 01234
										}
										else if(i==0x03)
											while(cnt--)
										{
											buf[len++]=(PM2_5>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=(PM2_5)&0x00ff;	;		//���ֽ� 01234
										}
							  }
							  if((i==0x01) && (cnt==0x03))  //��ȡ����Ĵ���ֵ
								{
//								 	while(cnt--)
									{
//										if(cnt==0x03)
										{
											buf[len++]=((int)Temperature>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=((int)Temperature)&0x00ff;	;		//���ֽ� 01234
										}
//										else if(cnt==0x02)
										{
											buf[len++]=((int)Humidity>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=((int)Humidity)&0x00ff;	;		//���ֽ� 01234
										}
//										else if(cnt==0x01)
										{
											buf[len++]=(PM2_5>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=(PM2_5)&0x00ff;	;		//���ֽ� 01234
										}
									}
								}
						}
							break;
					}
					else if((buf[2]==0x40)&&(buf[3]<=0x05)) //�Ĵ�����ַ֧��0x9c40~0x9c45
					{
							
							if(buf[3]<=0x05) 
							{
								i=buf[3];//��ȡ�Ĵ�����ַ
								cnt=buf[5];  //��ȡ����ȡ�ļĴ�������
								buf[2]=cnt*2;  //��ȡ���ݵ��ֽ�����Ϊ�Ĵ���*2����modbus����ļĴ���Ϊ16λ
								len=3;
								if(((i==0x02)||(i==0x03)||(i==0x04)) && (cnt==0x01))  //��ȡ�����Ĵ���ֵ
								{
									  if(i==0x02)
										while(cnt--)
										{
											buf[len++]=((int)Temperature>>8)&0x00ff;				//�Ĵ������ֽڲ�0
		//									buf[len++]=regGroup[i++];		//���ֽ� 01234
											buf[len++]=((int)Temperature)&0x00ff;	;		//���ֽ� 01234
										}
										else if(i==0x03)
											while(cnt--)
										{
											buf[len++]=((int)Humidity>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=((int)Humidity)&0x00ff;	;		//���ֽ� 01234
										}
										else if(i==0x04)
											while(cnt--)
										{
											buf[len++]=(PM2_5>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=(PM2_5)&0x00ff;	;		//���ֽ� 01234
										}
							  }
							  if((i==0x02) && (cnt==0x03))  //��ȡ����Ĵ���ֵ
								{
//								 	while(cnt--)
									{
//										if(cnt==0x03)
										{
											buf[len++]=((int)Temperature>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=((int)Temperature)&0x00ff;	;		//���ֽ� 01234
										}
//										else if(cnt==0x02)
										{
											buf[len++]=((int)Humidity>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=((int)Humidity)&0x00ff;	;		//���ֽ� 01234
										}
//										else if(cnt==0x01)
										{
											buf[len++]=(PM2_5>>8)&0x00ff;				//�Ĵ������ֽڲ�0
											buf[len++]=(PM2_5)&0x00ff;	;		//���ֽ� 01234
										}
									}
								}
						}
							break;
					}
					else  //�Ĵ�����ַ����֧��ʱ�����ش�����
					{   
						buf[1]=0x83;  //���������λ��1
						buf[2]=0x02;  //�����쳣��Ϊ02-��Ч��ַ
						len=3;
						break;
					}
////////////////////////////////////////////////////////////////////////////////////////////////////////					
					case 0x06:           //д�뵥���Ĵ���
						if((buf[2]==0x00)&&(buf[3]<=0x05))   //�Ĵ�����ַ֧��0x0000-0x0005
						{
							if(buf[3]<=0x04)
							{
								i=buf[3];				//��ȡ�Ĵ�����ַ
								regGroup[i]=buf[5];		//����Ĵ�������
//								led3=0;
							}
							len -=2;                 //����-2�����¼���CRC������ԭ֡
							break;
						}
						else  
						{							//�Ĵ�����ַ����֧�֣����ش�����
							buf[1]=0x86;           //���������λ��1
							buf[2]=0x02;           //�����쳣��Ϊ02-��Ч��ַ
							len=3;
							break;
					}
					default:    //������֧�ֵĹ�����
						    buf[1]=0x80;     //���������λ��1
							buf[2]=0x01;     //�����쳣��Ϊ01����Ч����
							len=3;
							break;
				}
			    crc=GetCRC16(buf,len);           //����CRCУ��ֵ
				buf[len++]=crc>>8;           //CRC���ֽ�
				buf[len++]=crc&0xff;        //CRC���ֽ�
				rs485_UartWrite(buf,len);  //������Ӧ֡
			}
			
		memset(buf,0,Usart1_RxCounter);
    Usart1_RxCounter=0;       //���ռ���������		
	}
//  Delay_Ms(500);
}

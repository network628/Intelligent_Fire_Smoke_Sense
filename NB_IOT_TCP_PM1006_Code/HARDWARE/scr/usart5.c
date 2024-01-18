#include "main.h"         //������Ҫ��ͷ�ļ�
#include "usart5.h"	  
#include "delay.h"
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "24c02.h"      //������Ҫ��ͷ�ļ�
#include "SHT2X.h"
 
#if EN_UART5_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART5_RX_BUF[USART5_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART5_RX_STA=0;       //����״̬���	  
  
void UART5_IRQHandler(void)
{
	u8 res;	
	if(UART5->SR&(1<<5))//���յ�����
	{	 
		res=UART5->DR; 
		if((USART5_RX_STA&0x8000)==0)//����δ���
		{
			if(USART5_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART5_RX_STA=0;//���մ���,���¿�ʼ
				else USART5_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(res==0x0d)USART5_RX_STA|=0x4000;
				else
				{
					if(res == 0x16)  USART5_RX_STA=0;
					USART5_RX_BUF[USART5_RX_STA&0X3FFF]=res;
					USART5_RX_STA++;
					if(USART5_RX_STA>(USART5_REC_LEN-1))USART5_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	}
 
} 
#endif		

//��ʼ��IO ����1
//bound:������
//CHECK OK
//091209
void Uart5_init(u32 bound)
{  	
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//ʹ�� GPIOD ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//ʹ�� GPIOD ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD2      BT_BR  rx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD   BT_BR
 

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;//PC12  BT_BC  tx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ�� 

  //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(UART5, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ���1 
	
#if EN_UART5_RX		  //���ʹ���˽���
 
  //Usart5 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
#endif
}


/*-------------------------------------------------*/
/*������������1 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/

__align(8) char Usart5_TxBuff[USART5_TXBUFF_SIZE];  

void u5_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(Usart5_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)Usart5_TxBuff);		
	while((UART5->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		UART5->DR = Usart5_TxBuff[i];
		while((UART5->SR&0X40)==0);	
	}	
}
 
void Uart5_Receive_Data(void)  //����PM2.5������
{
  
//	if(strstr((char *)USART5_RX_BUF,"DATA")) 
  if(USART5_RX_BUF[0]==0x16 && USART5_RX_BUF[1]==0x11 && USART5_RX_BUF[2]==0x0b)
//	if(USART5_RX_BUF[0]==0x16)
	{
		Sensor.PM2_5 = (USART5_RX_BUF[5]<<8) + USART5_RX_BUF[6];
//		u1_printf("Sensor.PM2_5 = %04d\r\n",Sensor.PM2_5);
	}
//  u1_printf("USART5_RX_BUF = %x-%x-%x\r\n",USART5_RX_BUF[0],USART5_RX_BUF[1],USART5_RX_BUF[2]);
	memset(USART5_RX_BUF,0,256);	
	USART5_RX_STA = 0;
}

void Uart5_Send_Data(void)
{
		  char i;
      char PM25_Str[] = {0x11, 0x02, 0x0B, 0x01, 0xE1};
			for(i=0;i<5;i++)
			{
			  USART_SendData(UART5, PM25_Str[i]);
				Delay_Ms(1); 
			}
}



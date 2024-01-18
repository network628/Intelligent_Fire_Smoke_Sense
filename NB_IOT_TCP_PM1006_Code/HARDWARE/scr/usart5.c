#include "main.h"         //包含需要的头文件
#include "usart5.h"	  
#include "delay.h"
#include "stm32f10x.h"  //包含需要的头文件
#include "usart1.h"       //包含需要的头文件
#include "24c02.h"      //包含需要的头文件
#include "SHT2X.h"
 
#if EN_UART5_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART5_RX_BUF[USART5_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART5_RX_STA=0;       //接收状态标记	  
  
void UART5_IRQHandler(void)
{
	u8 res;	
	if(UART5->SR&(1<<5))//接收到数据
	{	 
		res=UART5->DR; 
		if((USART5_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART5_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART5_RX_STA=0;//接收错误,重新开始
				else USART5_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)USART5_RX_STA|=0x4000;
				else
				{
					if(res == 0x16)  USART5_RX_STA=0;
					USART5_RX_BUF[USART5_RX_STA&0X3FFF]=res;
					USART5_RX_STA++;
					if(USART5_RX_STA>(USART5_REC_LEN-1))USART5_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	}
 
} 
#endif		

//初始化IO 串口1
//bound:波特率
//CHECK OK
//091209
void Uart5_init(u32 bound)
{  	
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//使能 GPIOD 时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//使能 GPIOD 时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD2      BT_BR  rx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD   BT_BR
 

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;//PC12  BT_BC  tx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 

  //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(UART5, &USART_InitStructure); //初始化串口1
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART5, ENABLE);                    //使能串口1 
	
#if EN_UART5_RX		  //如果使能了接收
 
  //Usart5 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
#endif
}


/*-------------------------------------------------*/
/*函数名：串口1 printf函数                         */
/*参  数：char* fmt,...  格式化输出字符串和参数    */
/*返回值：无                                       */
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
 
void Uart5_Receive_Data(void)  //新型PM2.5传感器
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



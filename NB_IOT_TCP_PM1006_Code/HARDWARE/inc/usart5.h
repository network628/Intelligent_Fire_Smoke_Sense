#ifndef __USART5_H
#define __USART5_H
//#include "sys.h" 
#include "stm32f10x.h"  //包含需要的头文件
#include "stdio.h"	
#include "stdarg.h"		//包含需要的头文件 
#include "string.h"     //包含需要的头文件
//////////////////////////////////////////////////////////////////////////////////	 
//#define BT_BR PDout(2)// PD2 主机有数据发送到模块  用来唤醒模块
//#define BT_BC  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)//模块有数据发送到主机
////////////////////////////////////////////////////////////////////////////////// 	
#define USART5_REC_LEN    200  	//定义最大接收字节数 200
#define EN_UART5_RX 			1		//使能（1）/禁止（0）串口4接收
	  	
extern u8  USART5_RX_BUF[USART5_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART5_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义

#define USART5_TXBUFF_SIZE   256    //定义串口1 发送缓冲区大小 256字节

void Uart5_init(u32 bound);
void u5_printf(char*,...) ;         //串口5 printf函数
 
void Uart5_Send_Data(void);
void Uart5_Receive_Data(void);  //新型PM2.5传感器

#endif	   















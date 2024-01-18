#ifndef __USART4_H
#define __USART4_H
//#include "sys.h" 
#include "stm32f10x.h"  //包含需要的头文件
#include "stdio.h"	
#include "stdarg.h"		//包含需要的头文件 
#include "string.h"     //包含需要的头文件
//////////////////////////////////////////////////////////////////////////////////	 
#define BT_BR PDout(2)// PD2 主机有数据发送到模块  用来唤醒模块
#define BT_BC  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)//模块有数据发送到主机
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN    200  	//定义最大接收字节数 200
#define EN_UART4_RX 			1		//使能（1）/禁止（0）串口4接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义

#define USART4_TXBUFF_SIZE   256    //定义串口1 发送缓冲区大小 256字节

void Uart4_init(u32 bound);
void u4_printf(char*,...) ;         //串口1 printf函数
void Blue_app(void);
void Send_BlueData(void);
void Receive_BlueData(void);

#endif	   















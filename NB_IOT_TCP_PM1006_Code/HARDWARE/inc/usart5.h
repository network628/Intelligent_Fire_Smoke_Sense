#ifndef __USART5_H
#define __USART5_H
//#include "sys.h" 
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "stdio.h"	
#include "stdarg.h"		//������Ҫ��ͷ�ļ� 
#include "string.h"     //������Ҫ��ͷ�ļ�
//////////////////////////////////////////////////////////////////////////////////	 
//#define BT_BR PDout(2)// PD2 ���������ݷ��͵�ģ��  ��������ģ��
//#define BT_BC  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)//ģ�������ݷ��͵�����
////////////////////////////////////////////////////////////////////////////////// 	
#define USART5_REC_LEN    200  	//�����������ֽ��� 200
#define EN_UART5_RX 			1		//ʹ�ܣ�1��/��ֹ��0������4����
	  	
extern u8  USART5_RX_BUF[USART5_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART5_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��

#define USART5_TXBUFF_SIZE   256    //���崮��1 ���ͻ�������С 256�ֽ�

void Uart5_init(u32 bound);
void u5_printf(char*,...) ;         //����5 printf����
 
void Uart5_Send_Data(void);
void Uart5_Receive_Data(void);  //����PM2.5������

#endif	   















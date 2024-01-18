#ifndef __USART4_H
#define __USART4_H
//#include "sys.h" 
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "stdio.h"	
#include "stdarg.h"		//������Ҫ��ͷ�ļ� 
#include "string.h"     //������Ҫ��ͷ�ļ�
//////////////////////////////////////////////////////////////////////////////////	 
#define BT_BR PDout(2)// PD2 ���������ݷ��͵�ģ��  ��������ģ��
#define BT_BC  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)//ģ�������ݷ��͵�����
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN    200  	//�����������ֽ��� 200
#define EN_UART4_RX 			1		//ʹ�ܣ�1��/��ֹ��0������4����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��

#define USART4_TXBUFF_SIZE   256    //���崮��1 ���ͻ�������С 256�ֽ�

void Uart4_init(u32 bound);
void u4_printf(char*,...) ;         //����1 printf����
void Blue_app(void);
void Send_BlueData(void);
void Receive_BlueData(void);

#endif	   















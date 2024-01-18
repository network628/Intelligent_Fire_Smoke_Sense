/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��LED���ܵ�Դ�ļ�                */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "led.h"        //������Ҫ��ͷ�ļ�

void LED_GPIO_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��PB�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				      //LED0-->PB8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					        //�����趨������ʼ��GPIOB.8
 GPIO_ResetBits(GPIOB,GPIO_Pin_8);						            //PB8  �����
}

void ALARM_GPIO_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_1; // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		   //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					       //�����趨������ʼ��GPIOC.3
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);						           //PC3  �����
	GPIO_SetBits(GPIOC,GPIO_Pin_1);	                       // SMOKE_ON  1��
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_13;             //BEEP �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
 



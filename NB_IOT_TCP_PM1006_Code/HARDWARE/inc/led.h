/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��LED���ܵ�ͷ�ļ�                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LED_H
#define __LED_H

#include "main.h"         //������Ҫ��ͷ�ļ�

#define LED   PBout(8)	      // LED1

#define BEEP        PCout(3)   //ALARM ��������
#define SMOKE_VCC  PCout(1)  //ALARM ��λ����
#define SMOKE_IN     GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_2)  //BEEP �������
#define KEY          GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_13)

void LED_GPIO_Init(void);//��ʼ��
void ALARM_GPIO_Init(void);

#endif

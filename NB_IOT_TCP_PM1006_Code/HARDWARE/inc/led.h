/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              实现LED功能的头文件                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LED_H
#define __LED_H

#include "main.h"         //包含需要的头文件

#define LED   PBout(8)	      // LED1

#define BEEP        PCout(3)   //ALARM 声音控制
#define SMOKE_VCC  PCout(1)  //ALARM 复位控制
#define SMOKE_IN     GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_2)  //BEEP 报警检测
#define KEY          GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_13)

void LED_GPIO_Init(void);//初始化
void ALARM_GPIO_Init(void);

#endif

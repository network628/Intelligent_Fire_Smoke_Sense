/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              实现LED功能的源文件                */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "led.h"        //包含需要的头文件

void LED_GPIO_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能PB端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				      //LED0-->PB8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					        //根据设定参数初始化GPIOB.8
 GPIO_ResetBits(GPIOB,GPIO_Pin_8);						            //PB8  输出高
}

void ALARM_GPIO_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_1; // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		   //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					       //根据设定参数初始化GPIOC.3
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);						           //PC3  输出高
	GPIO_SetBits(GPIOC,GPIO_Pin_1);	                       // SMOKE_ON  1打开
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_13;             //BEEP 报警检测
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
 



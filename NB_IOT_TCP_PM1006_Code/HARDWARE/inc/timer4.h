#ifndef __TIMER4_H
#define __TIMER4_H
#include <stdint.h> 

extern uint8_t  TIM4CH4_CAPTURE_STA;		//输入捕获状态		    				
extern uint16_t	TIM4CH4_CAPTURE_VAL;	//输入捕获值	

void TIM4_Cap_Init(uint16_t arr,uint16_t psc);
uint16_t Get_PM1006_Value(void);
#endif

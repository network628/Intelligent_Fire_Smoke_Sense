#ifndef __PM1006_H
#define __PM1006_H			 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//extern unsigned char I2CRecvBuff[22]; //����������ݻ����� 
 
void I2CDemoSendCmd(void); 
int I2CDemoReadPM(void);

#endif 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
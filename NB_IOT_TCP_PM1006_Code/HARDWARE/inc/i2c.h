#ifndef __I2C_H
#define __I2C_H			 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/

#define EEPROM_I2C_SCL_PIN      					 	GPIO_Pin_6   
#define EEPROM_I2C_SCL_GPIO_PORT   	       	GPIOB  
#define EEPROM_I2C_SCL_GPIO_RCC      			  RCC_APB2Periph_GPIOB  
 
#define EEPROM_I2C_SDA_PIN      					 	GPIO_Pin_7   
#define EEPROM_I2C_SDA_GPIO_PORT   	       	GPIOB  
#define EEPROM_I2C_SDA_GPIO_RCC      			  RCC_APB2Periph_GPIOB  

#define SCL_H         	 GPIO_SetBits(EEPROM_I2C_SCL_GPIO_PORT , EEPROM_I2C_SCL_PIN)   /*GPIOB->BSRR = GPIO_Pin_6*/
#define SCL_L            GPIO_ResetBits(EEPROM_I2C_SCL_GPIO_PORT , EEPROM_I2C_SCL_PIN) /*GPIOB->BRR  = GPIO_Pin_6 */
   
#define SDA_H         	 GPIO_SetBits(EEPROM_I2C_SDA_GPIO_PORT , EEPROM_I2C_SDA_PIN)   /*GPIOB->BSRR = GPIO_Pin_7*/
#define SDA_L         	 GPIO_ResetBits(EEPROM_I2C_SDA_GPIO_PORT , EEPROM_I2C_SDA_PIN) /*GPIOB->BRR  = GPIO_Pin_7*/

#define SCL_read       	GPIO_ReadInputDataBit(EEPROM_I2C_SCL_GPIO_PORT , EEPROM_I2C_SCL_PIN)/* GPIOB->IDR  & GPIO_Pin_6   */
#define SDA_read       	GPIO_ReadInputDataBit(EEPROM_I2C_SDA_GPIO_PORT , EEPROM_I2C_SDA_PIN)/*GPIOB->IDR  & GPIO_Pin_7	  */

#define I2C_PageSize  8  

#define ADDR_24C08		0xA0

/* Private function prototypes -----------------------------------------------*/
void I2C_delay(void);
FunctionalState I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
FunctionalState I2C_WaitAck(void) ;
void I2C_SendByte(uint8_t SendByte) ;
uint8_t I2C_ReceiveByte(void)  ;
void I2C_Configuration(void);


#endif 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

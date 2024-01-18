#ifndef __SHT2X_H
#define __SHT2X_H			 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#define CHECKSUM_ERROR 0x04
 
 

typedef enum{
  TRIG_T_MEASUREMENT_HM    = 0xE3, // command trig. temp meas. hold master
  TRIG_RH_MEASUREMENT_HM   = 0xE5, // command trig. humidity meas. hold master
  TRIG_T_MEASUREMENT_POLL  = 0xF3, // command trig. temp meas. no hold master
  TRIG_RH_MEASUREMENT_POLL = 0xF5, // command trig. humidity meas. no hold master
  USER_REG_W               = 0xE6, // command writing user register
  USER_REG_R               = 0xE7, // command reading user register
  SOFT_RESET               = 0xFE  // command soft reset
}SHT2xCommand;

typedef enum {
  SHT2x_RES_12_14BIT       = 0x00, // RH=12bit, T=14bit
  SHT2x_RES_8_12BIT        = 0x01, // RH= 8bit, T=12bit
  SHT2x_RES_10_13BIT       = 0x80, // RH=10bit, T=13bit
  SHT2x_RES_11_11BIT       = 0x81, // RH=11bit, T=11bit
  SHT2x_RES_MASK           = 0x81  // Mask for res. bits (7,0) in user reg.
} SHT2xResolution;

typedef enum {
  SHT2x_EOB_ON             = 0x40, // end of battery
  SHT2x_EOB_MASK           = 0x40, // Mask for EOB bit(6) in user reg.
} SHT2xEob;

typedef enum {
  SHT2x_HEATER_ON          = 0x04, // heater on
  SHT2x_HEATER_OFF         = 0x00, // heater off
  SHT2x_HEATER_MASK        = 0x04, // Mask for Heater bit(2) in user reg.
} SHT2xHeater;


typedef enum{
  HUMIDITY,
  TEMP
}SHT2xMeasureType;

typedef enum{
  I2C_ADR_W                = 128,   // sensor I2C address + write bit
  I2C_ADR_R                = 129    // sensor I2C address + read bit
}I2cHeader;

FunctionalState SHT2X_IIC_WriteByte(uint8_t WriteAddress,uint8_t SendByte);
FunctionalState SHT2X_IIC_ReadByte( uint8_t ReadAddress, uint16_t length  ,uint8_t* pBuffer);
FunctionalState SHT2x_CheckCrc(u8 data[],u8 startBytes,u8 number, u8 checksum);
u8 SHT2x_ReadUserRegister(void);
FunctionalState SHT2x_WriteUserRegister(u8 userdata);
FunctionalState SHT2x_SoftReset(void);
FunctionalState SHT2x_Calc_T(void);
FunctionalState SHT2x_Calc_RH(void);
u8 SHT2x_GetSerialNumber(u8 *pBuffer1,u8 *pBuffer2);
void SHT2X_TEST(void);
u32 SHT2X_Init(void);

#endif 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

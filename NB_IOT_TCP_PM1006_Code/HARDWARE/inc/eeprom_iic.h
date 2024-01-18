/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            实现IIc总线功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/
//b6    b7
//b10   b11

#ifndef __EEPROM_IIC_H
#define __EEPROM_IIC_H
  
#define SDA_IN()        {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00008000;GPIOB->BSRR=(1<<3);} //SDA设置为上拉输入模式
#define SDA_OUT()       {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00003000;} 	
//#define SDA_IN()        {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X80000000;GPIOB->BSRR=(1<<7);} //SDA设置为上拉输入模式
//#define SDA_OUT()       {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X30000000;}                    //SDA设置为推免输出模式
#define READ_SDA        GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)       //读取SDA 
#define IIC_SDA_OUT(x)  GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)x) //设置SDA电平 

#define	IIC_SCL_H    GPIO_SetBits(GPIOB, GPIO_Pin_10)    //SCL拉高
#define	IIC_SDA_H    GPIO_SetBits(GPIOB, GPIO_Pin_11)    //SDA拉高

#define	IIC_SCL_L    GPIO_ResetBits(GPIOB, GPIO_Pin_10)  //SCL拉低
#define	IIC_SDA_L    GPIO_ResetBits(GPIOB, GPIO_Pin_11)  //SDA拉低

void EEPROM_IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
char IIC_Wait_Ack(void);
void IIC_Send_Byte(unsigned char);
unsigned char IIC_Read_Byte(unsigned char);

#endif

















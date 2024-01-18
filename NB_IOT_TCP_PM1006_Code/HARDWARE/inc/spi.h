/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��spi2���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __SPI_H
#define __SPI_H	 

void SPI_Configuration(void);
void SPI_GPIO_Configuration(void);
void SPI_WriteByte(unsigned char TxData);
unsigned char SPI_ReadByte(void);
void SPI_CrisEnter(void);
void SPI_CrisExit(void);
void SPI_CS_Select(void);
void SPI_CS_Deselect(void);
void W5500_Reset(void);
#endif 


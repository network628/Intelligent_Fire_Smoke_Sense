/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��spi2���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "delay.h"      //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*����������ʼ��SPI�ӿ�                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;                      //����һ������GPIO�ı���
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //ʹ��GPIOA�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //ʹ��GPIOB�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);   //ʹ��GPIOC�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);    //ʹ��SPI1ʱ�� 
	
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;//׼������PB13 PB14 PB15
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                      //����50Mhz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                        //�����������ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStruct);                                 //����PB13 PB14 PB15

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;        //׼������PB12  CS
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; //����50Mhz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  //�������ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStruct);            //����PB12  
	GPIO_SetBits(GPIOA,GPIO_Pin_4);               //PB12�øߵ�ƽ
}

/*-------------------------------------------------*/
/*����������ʼ��SPI����                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_GPIO_Configuration();	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStruct);	
	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
}
/*-------------------------------------------------*/
/*��������д1�ֽ����ݵ�SPI����                     */
/*��  ����TxData:д�����ߵ�����                    */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_WriteByte(unsigned char TxData)
{				 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������		  
	SPI1->DR=TxData;	 	  				//����һ��byte 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte
	SPI1->DR;		
}

/*-------------------------------------------------*/
/*����������SPI���߶�ȡ1�ֽ�����                   */
/*��  ������                                       */
/*����ֵ������������                               */
/*-------------------------------------------------*/
unsigned char SPI_ReadByte(void)
{			 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������			  
	SPI1->DR=0xFF;	 	  					//����һ�������ݲ����������ݵ�ʱ�� 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte  
	return SPI1->DR;   
}

/*-------------------------------------------------*/
/*�������������ٽ���                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CrisEnter(void)
{
	__set_PRIMASK(1);    //��ֹȫ���ж�
}
/*-------------------------------------------------*/
/*���������˳��ٽ���                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CrisExit(void)
{
	__set_PRIMASK(0);   //��ȫ���ж�
}
/*-------------------------------------------------*/
/*�������� Ƭѡ�ź�����͵�ƽ                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CS_Select(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

/*-------------------------------------------------*/
/*�������� Ƭѡ�ź�����ߵ�ƽ                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CS_Deselect(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}


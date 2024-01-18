/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              实现spi2功能的源文件               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "delay.h"      //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：初始化SPI接口                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;                      //定义一个设置GPIO的变量
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //使能GPIOA端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //使能GPIOB端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);   //使能GPIOC端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);    //使能SPI1时钟 
	
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;//准备设置PB13 PB14 PB15
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                      //速率50Mhz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                        //复用推免输出模式
	GPIO_Init(GPIOA, &GPIO_InitStruct);                                 //设置PB13 PB14 PB15

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;        //准备设置PB12  CS
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; //速率50Mhz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  //推免输出模式
	GPIO_Init(GPIOA, &GPIO_InitStruct);            //设置PB12  
	GPIO_SetBits(GPIOA,GPIO_Pin_4);               //PB12置高电平
}

/*-------------------------------------------------*/
/*函数名：初始化SPI功能                            */
/*参  数：无                                       */
/*返回值：无                                       */
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
/*函数名：写1字节数据到SPI总线                     */
/*参  数：TxData:写到总线的数据                    */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI_WriteByte(unsigned char TxData)
{				 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//等待发送区空		  
	SPI1->DR=TxData;	 	  				//发送一个byte 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //等待接收完一个byte
	SPI1->DR;		
}

/*-------------------------------------------------*/
/*函数名：从SPI总线读取1字节数据                   */
/*参  数：无                                       */
/*返回值：读到的数据                               */
/*-------------------------------------------------*/
unsigned char SPI_ReadByte(void)
{			 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//等待发送区空			  
	SPI1->DR=0xFF;	 	  					//发送一个空数据产生输入数据的时钟 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //等待接收完一个byte  
	return SPI1->DR;   
}

/*-------------------------------------------------*/
/*函数名：进入临界区                               */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI_CrisEnter(void)
{
	__set_PRIMASK(1);    //禁止全局中断
}
/*-------------------------------------------------*/
/*函数名：退出临界区                               */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI_CrisExit(void)
{
	__set_PRIMASK(0);   //开全局中断
}
/*-------------------------------------------------*/
/*函数名： 片选信号输出低电平                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI_CS_Select(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

/*-------------------------------------------------*/
/*函数名： 片选信号输出高电平                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI_CS_Deselect(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}


#include "usart2.h"        //包含需要的头文件
#include "stm32f10x.h"    //包含需要的头文件
#include "delay.h"        //包含需要的头文件

#define Uart2_SendStr       u2_printf           //串口2控制 WiFi
#define buf_uart2index    Usart2_RxCounter    //串口2控制 WiFi
#define buf_uart2buf       Usart2_RxBuff       //串口2控制 WiFi
#define BUFLEN      USART2_RXBUFF_SIZE  //串口2控制 WiFi
#define Delay   Delay_Ms

//#define BUFLEN 256      //数组缓存大小
//typedef struct _UART_BUF
//{
//    char buf [BUFLEN+1];               
//    unsigned int index ;
//}UART_BUF;

extern char *SERVERIP;           //存放服务器IP或是域名
extern int  SERVERPORT;          //存放服务器的端口号区

void Clear_Buffer(void);//清空缓存	
int BC28_Init(void);
void BC28_PDPACT(void);
void BC28_ConTCP(void);
void BC28_RECData(void);
void BC28_Senddata(uint8_t *len,uint8_t *data);
void NB_iot_Send_Data(void);
void Check_TCP(void);
//void test_value(void);	
	
typedef struct
{
   uint8_t CSQ;    
   uint8_t Socketnum;   //编号
   uint8_t reclen[10];   //获取到数据的长度
   uint8_t res;      
   uint8_t recdatalen[10];
   uint8_t recdata[100];
} BC28;


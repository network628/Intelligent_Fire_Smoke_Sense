#include "usart2.h"        //������Ҫ��ͷ�ļ�
#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "delay.h"        //������Ҫ��ͷ�ļ�

#define Uart2_SendStr       u2_printf           //����2���� WiFi
#define buf_uart2index    Usart2_RxCounter    //����2���� WiFi
#define buf_uart2buf       Usart2_RxBuff       //����2���� WiFi
#define BUFLEN      USART2_RXBUFF_SIZE  //����2���� WiFi
#define Delay   Delay_Ms

//#define BUFLEN 256      //���黺���С
//typedef struct _UART_BUF
//{
//    char buf [BUFLEN+1];               
//    unsigned int index ;
//}UART_BUF;

extern char *SERVERIP;           //��ŷ�����IP��������
extern int  SERVERPORT;          //��ŷ������Ķ˿ں���

void Clear_Buffer(void);//��ջ���	
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
   uint8_t Socketnum;   //���
   uint8_t reclen[10];   //��ȡ�����ݵĳ���
   uint8_t res;      
   uint8_t recdatalen[10];
   uint8_t recdata[100];
} BC28;


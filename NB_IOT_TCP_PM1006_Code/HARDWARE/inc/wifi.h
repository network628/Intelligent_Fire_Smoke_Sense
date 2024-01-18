/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ����Wifi���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "usart2.h"	       //������Ҫ��ͷ�ļ�

extern char WiFi_Connect_flag;  //�ⲿ����������ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������

extern char *ServerIP;           //��ŷ�����IP��������
extern int  ServerPort;          //��ŷ������Ķ˿ں���

#define RESET_IO(x)    GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)x)  //PA4����WiFi�ĸ�λ

#define WiFi_printf       u2_printf           //����2���� WiFi
#define WiFi_RxCounter    Usart2_RxCounter    //����2���� WiFi
#define WiFi_RX_BUF       Usart2_RxBuff       //����2���� WiFi
#define WiFi_RXBUFF_SIZE  USART2_RXBUFF_SIZE  //����2���� WiFi

#define SSID   "QD-DATA"                     //·����SSID����
#define PASS   "44448888"                 //·��������

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_GetIP(int timeout);
char WiFi_Get_LinkSta(void);
char WiFi_Get_Data(char *data, char *len, char *id);
char WiFi_SendData(char id, char *databuff, int data_len, int timeout);
//char WiFi_Connect_Server(int timeout);
char WiFi_ConnectServer(void);

void Wifi_Receive_UseData(void);
void Wifi_Send_UseData(void);

#endif



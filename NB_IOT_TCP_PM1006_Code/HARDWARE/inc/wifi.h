/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              操作Wifi功能的头文件               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "usart2.h"	       //包含需要的头文件

extern char WiFi_Connect_flag;  //外部变量声明，同服务器连接状态  0：还没有连接服务器  1：连接上服务器了

extern char *ServerIP;           //存放服务器IP或是域名
extern int  ServerPort;          //存放服务器的端口号区

#define RESET_IO(x)    GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)x)  //PA4控制WiFi的复位

#define WiFi_printf       u2_printf           //串口2控制 WiFi
#define WiFi_RxCounter    Usart2_RxCounter    //串口2控制 WiFi
#define WiFi_RX_BUF       Usart2_RxBuff       //串口2控制 WiFi
#define WiFi_RXBUFF_SIZE  USART2_RXBUFF_SIZE  //串口2控制 WiFi

#define SSID   "QD-DATA"                     //路由器SSID名称
#define PASS   "44448888"                 //路由器密码

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



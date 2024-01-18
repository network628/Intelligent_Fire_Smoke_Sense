/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�ͷ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __MQTT_H
#define __MQTT_H

#define  BUFF_UNIT           512    //����������
#define  R_NUM               8     //���ջ���������
#define  T_NUM               8     //���ͻ���������  
#define  C_NUM               8     //�����������

#define  MQTT_TxData(x)       W5500_TxData(x)                               

#define  PRODUCTID            "170297"                                      //��ƷID
#define  PRODUCTID_LEN        strlen(PRODUCTID)                             //��ƷID����
#define  DEVICEID             "41898794"                                    //�豸ID  
#define  DEVICEID_LEN         strlen(DEVICEID)                              //�豸ID����
#define  AUTHENTICATION       "kfbskd"                                      //��Ȩ��Ϣ  
#define  AUTHENTICATION_LEN    strlen(AUTHENTICATION)                       //��Ȩ��Ϣ����
#define  S_TOPIC_NAME         "APP_Topic"                                    //��Ҫ���ĵ�����  
#define  P_TOPIC_NAME         "KFB_Topic"                                   //��Ҫ����������  

extern unsigned char  MQTT_RxDataBuf[R_NUM][BUFF_UNIT];        //�ⲿ�������������ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_RxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_RxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_RxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_TxDataBuf[T_NUM][BUFF_UNIT];        //�ⲿ�������������ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_TxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_TxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_TxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_CMDBuf[C_NUM][BUFF_UNIT];           //�ⲿ�����������������ݵĽ��ջ�����
extern unsigned char *MQTT_CMDInPtr;                           //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_CMDOutPtr;                          //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_CMDEndPtr;                          //�ⲿ����������ָ�򻺳���������λ��

extern char ClientID[128];     //�ⲿ������������ſͻ���ID�Ļ�����
extern int  ClientID_len;      //�ⲿ������������ſͻ���ID�ĳ���
extern char Username[128];     //�ⲿ��������������û����Ļ�����
extern int  Username_len;	   //�ⲿ��������������û����ĳ���
extern char Passward[128];     //�ⲿ�����������������Ļ�����
extern int  Passward_len;	   //�ⲿ�����������������ĳ���
extern unsigned char ServerIP[4];       //�ⲿ������������ŷ�����IP
extern unsigned char ServerName[128];   //�ⲿ������������ŷ���������
extern int  ServerPort;                 //�ⲿ������������ŷ������Ķ˿ں�

extern char Connect_flag;      //�ⲿ����������ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
extern char ReConnect_flag;    //�ⲿ��������������������״̬    0�����ӻ�����  1�����ӶϿ�������
extern char ConnectPack_flag;  //�ⲿ����������CONNECT����״̬   1��CONNECT���ĳɹ�
extern char SubcribePack_flag; //�ⲿ�������������ı���״̬      1�����ı��ĳɹ�

void MQTT_Buff_Init(void);
void MQTT_Buff_ReInit(void);
void OneNetIoT_Parameter_Init(void);
void MQTT_ConectPack(void);
void MQTT_Subscribe(char *, int);
void MQTT_PingREQ(void);
void MQTT_PublishQs0(char *, char *, int);
void MQTT_DealPushdata_Qs0(unsigned char *);	
void TxDataBuf_Deal(unsigned char *, int);
void CMDBuf_Deal(unsigned char *, int);

#endif

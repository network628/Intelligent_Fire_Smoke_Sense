/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ����602Wifi���ܵ�Դ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/
#include "usart4.h"	 
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "main.h"       //������Ҫ��ͷ�ļ�
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "delay.h"	    //������Ҫ��ͷ�ļ�
#include "usart1.h"	    //������Ҫ��ͷ�ļ�
#include "led.h"        //������Ҫ��ͷ�ļ�
#include "key.h"        //������Ҫ��ͷ�ļ�
#include "24c02.h" 		    //������Ҫ��ͷ�ļ�
#include "SHT2X.h"
#include "rtc.h"
/*-----------------------------------------------------------------------------*/
/*                              �����Լ������绷���޸�                         */
/*-----------------------------------------------------------------------------*/
char *ServerIP = "49.235.207.142";           //��ŷ�����IP��������
//char *ServerIP = "192.168.0.99";           //��ŷ�����IP��������
int  ServerPort = 8001;                   //��ŷ������Ķ˿ں���
/*-----------------------------------------------------------------------------*/

char  Wifi_Data_buff[2048];     //���ݻ�����

char wifi_mode = 1;     //����ģʽ 0��SSID������д�ڳ�����   1��Smartconfig��ʽ��APP����
char WiFi_Connect_flag;      //ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������

/*-------------------------------------------------*/
/*����������ʼ��WiFi�ĸ�λIO                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void WiFi_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);   //ʹ��PA�˿�ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //ENABLEPBCLK	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				      //LED0-->PB8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					        //�����趨������ʼ��GPIOB.8
	GPIO_SetBits(GPIOB,GPIO_Pin_3);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;             //key
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}
/*-------------------------------------------------*/
/*��������WiFi��������ָ��                         */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter=0;                           //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ����� 
	WiFi_printf("%s\r\n",cmd);                  //����ָ��
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                          //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"OK"))            //������յ�OK��ʾָ��ɹ�
			break;       						//��������whileѭ��
		u4_printf("%d ",timeout);               //����������ڵĳ�ʱʱ��
	}
	u4_printf("\r\n");                          //���������Ϣ
	if(timeout<=0)return 1;                     //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         				//��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi��λ                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Reset(int timeout)
{
	RESET_IO(0);                                    //��λIO���͵�ƽ
	Delay_Ms(500);                                  //��ʱ500ms
	RESET_IO(1);                                    //��λIO���ߵ�ƽ	
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"ready"))             //������յ�ready��ʾ��λ�ɹ�
			break;       						    //��������whileѭ��
		u4_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u4_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�ready������1
	else return 0;		         				    //��֮����ʾ��ȷ��˵���յ�ready��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi����·����ָ��                       */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ����� 
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS); //����ָ��	
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                             //��ʱ1s
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP\r\n\r\nOK")) //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						    //��������whileѭ��
		u4_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u4_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*��������WiFi_Smartconfig                         */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{	
	WiFi_RxCounter=0;                           //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ�����     
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                         //��ʱ1s
		if(strstr(WiFi_RX_BUF,"connected"))     //������ڽ��ܵ�connected��ʾ�ɹ�
			break;                              //����whileѭ��  
		u4_printf("%d ",timeout);               //����������ڵĳ�ʱʱ��  
	}	
	u4_printf("\r\n");                          //���������Ϣ
	if(timeout<=0)return 1;                     //��ʱ���󣬷���1
	return 0;                                   //��ȷ����0
}
/*-------------------------------------------------*/
/*���������ȴ�����·����                           */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                             //��ʱ1s
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP"))       //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						    //��������whileѭ��
		u4_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u4_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*���������ȴ�����wifi����ȡIP��ַ                 */
/*��  ����ip������IP������                         */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_GetIP(int timeout)
{
	char *presult1,*presult2;
	char ip[4];
	
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ����� 
	WiFi_printf("AT+CIFSR\r\n");                    //����ָ��	
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"OK"))                //������յ�OK��ʾ�ɹ�
			break;       						    //��������whileѭ��
		u4_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u4_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else{
		presult1 = strstr(WiFi_RX_BUF,"\"");
		if( presult1 != NULL ){
			presult2 = strstr(presult1+1,"\"");
			if( presult2 != NULL ){
				memcpy(ip,presult1+1,presult2-presult1-1);
				u4_printf("ESP8266_IP��%s\r\n",ip);     //������ʾIP��ַ
				return 0;    //��ȷ����0
			}else return 2;  //δ�յ�Ԥ������
		}else return 3;      //δ�յ�Ԥ������	
	}
}
/*-------------------------------------------------*/
/*����������ȡ����״̬                             */
/*��  ������                                       */
/*����ֵ������״̬                                 */
/*        0����״̬                                */
/*        1���пͻ��˽���                          */
/*        2���пͻ��˶Ͽ�                          */
/*-------------------------------------------------*/
char WiFi_Get_LinkSta(void)
{
	char id_temp[10]={0};    //�����������ID
	char sta_temp[10]={0};   //�����������״̬
	
	if(strstr(WiFi_RX_BUF,"CONNECT")){                 //������ܵ�CONNECT��ʾ�пͻ�������	
		sscanf(WiFi_RX_BUF,"%[^,],%[^,]",id_temp,sta_temp);
		u4_printf("With client access,ID=%s\r\n",id_temp);  //�пͻ��˽��� ������ʾ��Ϣ  
		WiFi_RxCounter=0;                              //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);        //���WiFi���ջ�����     
		return 1;                                      //�пͻ��˽���
	}else if(strstr(WiFi_RX_BUF,"CLOSED")){            //������ܵ�CLOSED��ʾ�����ӶϿ�	
		sscanf(WiFi_RX_BUF,"%[^,],%[^,]",id_temp,sta_temp);
		u4_printf("A client is disconnected,ID=%s\r\n",id_temp);        //�пͻ��˶Ͽ���������ʾ��Ϣ
		WiFi_RxCounter=0;                                    //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);              //���WiFi���ջ�����     
		return 2;                                            //�пͻ��˶Ͽ�
	}else return 0;                                          //��״̬�ı�	
}
/*-------------------------------------------------*/
/*����������ȡ�ͻ�������                           */
/*        ����س����з�\r\n\r\n��Ϊ���ݵĽ�����   */
/*��  ����data�����ݻ�����                         */
/*��  ����len�� ������                             */
/*��  ����id��  �������ݵĿͻ��˵�����ID           */
/*����ֵ������״̬                                 */
/*        0��������                                */
/*        1��������                                */
/*-------------------------------------------------*/
char WiFi_Get_Data(char *data, char *len, char *id)
{
	char temp[10]={0};      //������
	char *presult;

	if(strstr(WiFi_RX_BUF,"\r\n\r\n")){                     //�������ŵĻس�������Ϊ���ݵĽ�����
		sscanf(WiFi_RX_BUF,"%[^,],%[^,],%[^:]",temp,id,len);//��ȡ�������ݣ���Ҫ��id�����ݳ���	
		presult = strstr(WiFi_RX_BUF,":");                  //����ð�š�ð�ź��������
		if( presult != NULL )                               //�ҵ�ð��
			sprintf((char *)data,"%s",(presult+1));         //ð�ź�����ݣ����Ƶ�data
		WiFi_RxCounter=0;                                   //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);             //���WiFi���ջ�����    
		return 1;                                           //�����ݵ���
	} else return 0;                                        //�����ݵ���
}
/*-------------------------------------------------*/
/*����������������������                           */
/*��  ����databuff�����ݻ�����<2048                */
/*��  ����data_len�����ݳ���                       */
/*��  ����id��      �ͻ��˵�����ID                 */
/*��  ����timeout�� ��ʱʱ�䣨10ms�ı�����         */
/*����ֵ������ֵ                                   */
/*        0���޴���                                */
/*        1���ȴ��������ݳ�ʱ                      */
/*        2�����ӶϿ���                            */
/*        3���������ݳ�ʱ                          */
/*-------------------------------------------------*/
char WiFi_SendData(char id, char *databuff, int data_len, int timeout)
{    
	int i;
	
	WiFi_RxCounter=0;                                 //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //���WiFi���ջ����� 
	WiFi_printf("AT+CIPSEND=%d,%d\r\n",id,data_len);  //����ָ��	
    while(timeout--){                                 //�ȴ���ʱ���	
		Delay_Ms(10);                                 //��ʱ10ms
		if(strstr(WiFi_RX_BUF,">"))                   //������յ�>��ʾ�ɹ�
			break;       						      //��������whileѭ��
		u4_printf("%d ",timeout);                     //����������ڵĳ�ʱʱ��
	}
	if(timeout<=0)return 1;                                   //��ʱ���󣬷���1
	else{                                                     //û��ʱ����ȷ       	
		WiFi_RxCounter=0;                                     //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);               //���WiFi���ջ����� 	
		for(i=0;i<data_len;i++)WiFi_printf("%c",databuff[i]); //��������	
		while(timeout--){                                     //�ȴ���ʱ���	
			Delay_Ms(10);                                     //��ʱ10ms
			if(strstr(WiFi_RX_BUF,"SEND OK")){                //�������SEND OK����ʾ���ͳɹ�			 
			WiFi_RxCounter=0;                                 //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //���WiFi���ջ����� 			
				break;                                        //����whileѭ��
			} 
			if(strstr(WiFi_RX_BUF,"link is not valid")){      //�������link is not valid����ʾ���ӶϿ�			
				WiFi_RxCounter=0;                             //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);       //���WiFi���ջ����� 			
				return 2;                                     //����2
			}
	    }
		if(timeout<=0)return 3;      //��ʱ���󣬷���3
		else return 0;	            //��ȷ������0
	}	
}
/*-------------------------------------------------*/
/*������������TCP��������������͸��ģʽ            */
/*��  ����timeout�� ��ʱʱ�䣨100ms�ı�����        */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
char WiFi_Connect_Server(int timeout)
{	
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ�����   
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ServerIP,ServerPort);//�������ӷ�����ָ��
	while(timeout--){                               //�ȴ���ʱ���
#ifdef Blue_APP	
    Receive_BlueData();	
#endif
		Delay_Ms(100);                              //��ʱ100ms	
		if(strstr(WiFi_RX_BUF ,"CONNECT"))          //������ܵ�CONNECT��ʾ���ӳɹ�
			break;                                  //����whileѭ��
		if(strstr(WiFi_RX_BUF ,"CLOSED"))           //������ܵ�CLOSED��ʾ������δ����
			return 1;                               //������δ��������1
		if(strstr(WiFi_RX_BUF ,"ALREADY CONNECTED"))//������ܵ�ALREADY CONNECTED�Ѿ���������
			return 2;                               //�Ѿ��������ӷ���2
		u4_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��  

	}
	u4_printf("\r\n");                        //���������Ϣ
	if(timeout<=0)return 3;                   //��ʱ���󣬷���3
	else                                      //���ӳɹ���׼������͸��
	{
		u4_printf("Ready to go through\r\n");
//		u4_printf("׼������͸��\r\n");                  //������ʾ��Ϣ
		WiFi_RxCounter=0;                               //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ�����     
		WiFi_printf("AT+CIPSEND\r\n");                  //���ͽ���͸��ָ��
		while(timeout--){                               //�ȴ���ʱ���
			Delay_Ms(100);                              //��ʱ100ms	
			if(strstr(WiFi_RX_BUF,"\r\nOK\r\n\r\n>"))   //���������ʾ����͸���ɹ�
				break;                          //����whileѭ��
			u4_printf("%d ",timeout);           //����������ڵĳ�ʱʱ��  
		}
		if(timeout<=0)return 4;                 //͸����ʱ���󣬷���4	
	}
	return 0;	                                //�ɹ�����0	
}
/*-------------------------------------------------*/
/*�����������ӷ�����                               */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_ConnectServer(void)
{	
	char res;
	char cnt;
	u4_printf("Prepare to reset the module\r\n");                     //׼����λģ��  ������ʾ����
	if(WiFi_Reset(50)){                                //��λ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u4_printf("Reset failed, ready to restart\r\n");           //���ط�0ֵ������if����λʧ�ܣ�׼������ ������ʾ����
		return 1;                                      //����1
	}else u4_printf("Reset successfully\r\n");                   //������ʾ����  ��λ�ɹ�
	
	u4_printf("Ready to set STA mode\r\n");                  //  ׼������STAģʽ  ������ʾ����
	if(WiFi_SendCmd("AT+CWMODE=1",50)){                //����STAģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u4_printf("Failed to set STA mode, ready to restart\r\n");    //���ط�0ֵ������if������STAģʽʧ�ܣ�׼������  ������ʾ����
		return 2;                                      //����2
	}else u4_printf("Set STA mode successfully\r\n");            //������ʾ����   ����STAģʽ�ɹ�
	
	if(wifi_mode==0){                                      //�������ģʽ=0��SSID������д�ڳ����� 
		u4_printf("Ready to cancel automatic connection\r\n");  //������ʾ����  ׼��ȡ���Զ�����
		if(WiFi_SendCmd("AT+CWAUTOCONN=0",50)){            //ȡ���Զ����ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
			u4_printf("Failed to cancel automatic connection, ready to restart\r\n");   //���ط�0ֵ������if��������ʾ����  ȡ���Զ�����ʧ�ܣ�׼������
			return 3;                                      //����3
		}else u4_printf("Automatic connection cancelled successfully\r\n");           //������ʾ����  ȡ���Զ����ӳɹ�
				
		u4_printf("Ready to connect to router\r\n");      //������ʾ����	׼������·����
		if(WiFi_JoinAP(30)){                               //����·����,1s��ʱ��λ���ܼ�30s��ʱʱ��
			u4_printf("Failed to connect to router, ready to restart\r\n");     //���ط�0ֵ������if��������ʾ����  ����·����ʧ�ܣ�׼������
			return 4;                                      //����4	
		}else u4_printf("Connect router successfully\r\n");  //������ʾ����		 ����·�����ɹ�	
	}else{                                                 //�������ģʽ=1��Smartconfig��ʽ,��APP����
		if(KEY==0)  //�����ʱK2�ǰ��µ�
		{     
			for(cnt=0;cnt<10;cnt++)
			{
        u4_printf("Ready to set up automatic connection\r\n");  //  ׼�������Զ�����
			  LED = ~LED; Delay_Ms(100);
			}
			if(WiFi_SendCmd("AT+CWAUTOCONN=1",50)){            //�����Զ����ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
				u4_printf("Failed to set automatic connection, ready to restart\r\n");   //���ط�0ֵ������if��������ʾ����  �����Զ�����ʧ�ܣ�׼������
				return 3;                                      //����3
			}else u4_printf("Automatic connection set successfully\r\n");           //������ʾ����	 �����Զ����ӳɹ�
			
			u4_printf("Ready to open Smartconfig\r\n");              //������ʾ���� ׼������
			if(WiFi_SendCmd("AT+CWSTARTSMART",50)){            //����Smartconfig��100ms��ʱ��λ���ܼ�5s��ʱʱ��
				u4_printf("Failed to open smartconfig, ready to restart\r\n");//���ط�0ֵ������if��������ʾ����  ����Smartconfigʧ�ܣ�׼������
				return 4;                                      //����4
			}else u4_printf("Smartconfig enabled successfully\r\n");        //������ʾ����  ����Smartconfig�ɹ�

			u4_printf("Please use app software to transfer password\r\n");            //������ʾ���� ��ʹ��APP�����������
			if(WiFi_Smartconfig(60)){                          //APP����������룬1s��ʱ��λ���ܼ�60s��ʱʱ��
				u4_printf("Failed to transfer password, ready to restart\r\n");       //���ط�0ֵ������if��������ʾ����  ��������ʧ�ܣ�׼������
				return 5;                                      //����5
			}else u4_printf("Password transferred successfully\r\n");               //������ʾ����  ��������ɹ�

			u4_printf("Ready to close Smartconfig\r\n");              //������ʾ����  ׼���ر�
			if(WiFi_SendCmd("AT+CWSTOPSMART",50)){             //�ر�Smartconfig��100ms��ʱ��λ���ܼ�5s��ʱʱ��
				u4_printf("Failed to close smartconfig, ready to restart\r\n");//���ط�0ֵ������if��������ʾ����  �ر�Smartconfigʧ�ܣ�׼������
				return 6;                                      //����6
			}else u4_printf("Smartconfig closed successfully\r\n");        //������ʾ����  �ر�Smartconfig�ɹ�
		}
		else
		{                                                 //��֮����ʱK2��û�а���  
			u4_printf("Waiting to connect to router\r\n");                   //������ʾ����	  �ȴ�����·����
			if(WiFi_WaitAP(30)){                               //�ȴ�����·����,1s��ʱ��λ���ܼ�30s��ʱʱ��
				u4_printf("Failed to connect to router, ready to restart\r\n");     //���ط�0ֵ������if��������ʾ����  ����·����ʧ�ܣ�׼������
				return 7;                                      //����7	
			}else u4_printf("Connect router successfully\r\n");             //������ʾ����  ����·�����ɹ�					
		}
	}
	
	u4_printf("Ready to get IP address\r\n");                   //������ʾ����  ׼����ȡIP��ַ
	if(WiFi_GetIP(50)){                                //׼����ȡIP��ַ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u4_printf("Failed to get IP address, ready to restart\r\n");     //���ط�0ֵ������if��������ʾ����  ��ȡIP��ַʧ�ܣ�׼������
		return 10;                                     //����10
	}else u4_printf("IP address obtained successfully\r\n");             //������ʾ����  ��ȡIP��ַ�ɹ�
	
	u4_printf("Ready to turn on teleport\r\n");                     //������ʾ����  ׼������͸��
	if(WiFi_SendCmd("AT+CIPMODE=1",50)){               //����͸����100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u4_printf("Failed to open transparent transmission, ready to restart\r\n");       //���ط�0ֵ������if��������ʾ����  ����͸��ʧ�ܣ�׼������
		return 11;                                     //����11
	}else u4_printf("Successfully closed transparent transmission\r\n");               //������ʾ����  �ر�͸���ɹ�
	
	u4_printf("Ready to close multi way connection\r\n");                 //������ʾ����  ׼���رն�·����
	if(WiFi_SendCmd("AT+CIPMUX=0",50)){                //�رն�·���ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u4_printf("Failed to close multi-channel connection, ready to restart\r\n");   //���ط�0ֵ������if��������ʾ����  �رն�·����ʧ�ܣ�׼������
		return 12;                                     //����12
	}else u4_printf("Multiple connection closed successfully\r\n");           //������ʾ����  �رն�·���ӳɹ�
	
	u4_printf("Ready to connect to server\r\n");                   //������ʾ����  ׼�����ӷ�����
	res = WiFi_Connect_Server(100);                    //���ӷ�������100ms��ʱ��λ���ܼ�10s��ʱʱ��
	if(res==1){                						   //����1������if
		u4_printf("The server is not turned on, ready to restart\r\n");       //������ʾ����  ������δ������׼������
		return 13;                                     //����13
	}else if(res==2){                                  //����2������if
		u4_printf("Connection already exists\r\n");                 //������ʾ����  �����Ѿ�����
	}else if(res==3){								   //����3������if
		u4_printf("Connection to server timed out, ready to restart\r\n");     //������ʾ����  ���ӷ�������ʱ��׼������
		return 14;                                     //����14
	}else if(res==4){								   //����4������if��
		u4_printf("Failed to enter transparent transmission\r\n");                 //������ʾ����  ����͸��ʧ��
		return 15;                                     //����15
	}	
	u4_printf("Successfully connected to the server\r\n");                   //������ʾ����   ���ӷ������ɹ�
	return 0;                                          //��ȷ����0	
}




void Wifi_Receive_UseData(void)
{
    char i;
    if(Usart2_RxCompleted==1)    //���Usart2_RxCompleted����1����ʾ�����������
		{
			Usart2_RxCompleted = 0;    //�����־λ
			for(i=2;i<27;i++)
			{
				Receive_String[i-2] = Wifi_Data_buff[i];		
			}
	    if(Receive_String[0]==0XEF && Receive_String[24]==0X1D && Receive_String[1]==0X06)   //�����������������0
			{
				for(i=0;i<25;i++)
				{
					USART_SendData(USART2, Receive_String[i]);   //����������ԭ�����ظ������
					Delay_Ms(1);			
				}
		/********************д��EEPROM**************************/
				Sensor.DEV_Num = (Receive_String[11]-0x30)*1000 + (Receive_String[12]-0x30)*100 + (Receive_String[13]-0x30)*10 + (Receive_String[14]-0x30);
				AT24C02_WriteOneByte(Addr_Base-0, Sensor.DEV_Num/255);   //�豸��ǧ��λ
				AT24C02_WriteOneByte(Addr_Base-1, Sensor.DEV_Num%255);   //�豸�Ÿ���λ 65535
				AT24C02_WriteOneByte(Addr_Base-2, Receive_String[16]);   //�¶�Ԥ��ʮλ
				AT24C02_WriteOneByte(Addr_Base-3, Receive_String[17]);   //�¶�Ԥ����λ
				AT24C02_WriteOneByte(Addr_Base-4, Receive_String[18]);   //ʪ��Ԥ��ʮλ
				AT24C02_WriteOneByte(Addr_Base-5, Receive_String[19]);   //ʪ��Ԥ����λ
				AT24C02_WriteOneByte(Addr_Base-6, Receive_String[20]);		
				AT24C02_WriteOneByte(Addr_Base-7, Receive_String[21]);   //PM2.5Ԥ�� 
						
				Sensor.Temperature_Waring = (Receive_String[16]<<8);
				Sensor.Temperature_Waring = (Sensor.Temperature_Waring + Receive_String[17]) /10;
				Sensor.Humidity_Waring = Receive_String[18]<<8;
				Sensor.Humidity_Waring = (Sensor.Humidity_Waring + Receive_String[19]) / 10;
				Sensor.PM2_5_Waring = Receive_String[20]<<8;
				Sensor.PM2_5_Waring = (Sensor.PM2_5_Waring + Receive_String[21]);
						
				u4_printf("---wifi_esp8266--->%04d %3.2f'C %3.2f%% %03d \r\n",Sensor.DEV_Num,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
			}
		} 
}


void Wifi_Send_UseData(void)
{
  char i;
	u32 chack_sum = 0;
	
	    Sensor.Year = calendar.w_year;
			Sensor.Month = calendar.w_month;
	    Sensor.Day = calendar.w_date;
	
			Send_String[0] = 0xEF;  //��ͷ
//			if((Sensor.Temperature_Waring < Sensor.Temperature)||(Sensor.Humidity_Waring > Sensor.Humidity)||(Sensor.Smoke_Dev==1))
//			Send_String[1] = 0X05;  //��������
//			else
			Send_String[1] = 0X04;  //��������
			Send_String[2] = 'M';   //���� ��ʪ�Ȼ����̽����
			Send_String[3] = 'E';   //����
			Send_String[4] = 'I';   //������
	
//	    Send_String[5]   = 0X30+Sensor.Year/1000%10;
//			Send_String[6]   = 0X30+Sensor.Year/100%10;
			Send_String[5]   = 0X30+Sensor.Year/10%10;
			Send_String[6]   = 0X30+Sensor.Year%10;   //�������� ��
			Send_String[7]   = 0X30+Sensor.Month/10;
			Send_String[8]  = 0X30+Sensor.Month%10;  //�������� ��
			Send_String[9]  = 0X30+Sensor.Day/10;
			Send_String[10]  = 0X30+Sensor.Day%10;  //�������� ��	
			Send_String[11]  = 0X30+Sensor.DEV_Num/1000%10;
			Send_String[12]  = 0X30+Sensor.DEV_Num/100%10;
			Send_String[13]  = 0X30+Sensor.DEV_Num/10%10;
			Send_String[14]  = 0X30+Sensor.DEV_Num%10;
			
			if(Sensor.Temperature_Waring < Sensor.Temperature+10)
			{
			  Send_String[15]  = 0X0B;//���±���
			}		
      else if(Sensor.Temperature_Waring < Sensor.Temperature)
			{
 	      Send_String[15]  = 0X0D;//����Ԥ��
			}				
			else if(Sensor.Humidity_Waring > Sensor.Humidity)
			{
			  Send_String[15]  = 0X12;//ʪ�ȱ���
			}		
			else if(Sensor.Humidity_Waring > Sensor.Humidity-10)
			{
			  Send_String[15]  = 0X11;//ʪ��Ԥ��
			}
			else if(Sensor.PM2_5_Waring<Sensor.PM2_5) 
			{
			  Send_String[15]  = 0X10;//����Ũ�ȱ���
			}
			else if(Sensor.PM2_5_Waring<Sensor.PM2_5+50) 
			{
			  Send_String[15]  = 0X0F;//����Ũ��Ԥ��
			}
			else if(Sensor.Smoke_Dev==1)
			{
			  Send_String[15]  = 0X14;//���ֱ���
			}
			else
			Send_String[15]  = 0X00;//���� ����������  �豸����
 
//			Send_String[15]  = 0X13;//����Ԥ��

			Send_String[16]  = (Sensor.Temperature*10)/255;
			Send_String[17]  = (int)(Sensor.Temperature*10)%255;
			
			Send_String[18]  = (Sensor.Humidity*10)/255;
			Send_String[19]  = (int)(Sensor.Humidity*10)%255;
   #ifdef PM1006
			Send_String[20] = Sensor.PM2_5/255;
			Send_String[21] = Sensor.PM2_5%255;
	 #endif
	 #ifdef	Dian_Smoke
			Send_String[20]  = 0;
			Send_String[21]  = Sensor.Smoke_Dev;
	 #endif	

      if(SHT2x_Calc_T() && (SHT2x_Calc_RH()))  //if
	     Send_String[22]  = 0X00;  //�豸״̬
			else
			 Send_String[22]  = 0X01;  //�豸״̬	
			
			for(i=1;i<23;i++)
			{
				chack_sum += Send_String[i]; 
			}
			Send_String[23]  = chack_sum % 255;
			//У����
			Send_String[24]  = 0X1D;  //��β����������
			
  	  for(i=0;i<25;i++)
			{
			  USART_SendData(USART2, Send_String[i]);
				Delay_Ms(1); 
			}
//      WiFi_printf("�����͵�������:%s\r\n",&Wifi_Data_buff[2]);	  //�ѽ��յ������ݣ����ظ�������
}





















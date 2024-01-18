/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "mqtt.h"         //������Ҫ��ͷ�ļ�
#include "string.h"       //������Ҫ��ͷ�ļ�
#include "stdio.h"        //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�

unsigned char  MQTT_RxDataBuf[R_NUM][BUFF_UNIT];            //���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_RxDataInPtr;                            //ָ����ջ�����������ݵ�λ��
unsigned char *MQTT_RxDataOutPtr;                           //ָ����ջ�������ȡ���ݵ�λ��
unsigned char *MQTT_RxDataEndPtr;                           //ָ����ջ�����������λ��

unsigned char  MQTT_TxDataBuf[T_NUM][BUFF_UNIT];            //���ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_TxDataInPtr;                            //ָ���ͻ�����������ݵ�λ��
unsigned char *MQTT_TxDataOutPtr;                           //ָ���ͻ�������ȡ���ݵ�λ��
unsigned char *MQTT_TxDataEndPtr;                           //ָ���ͻ�����������λ��

unsigned char  MQTT_CMDBuf[C_NUM][BUFF_UNIT];               //�������ݵĽ��ջ�����
unsigned char *MQTT_CMDInPtr;                               //ָ���������������ݵ�λ��
unsigned char *MQTT_CMDOutPtr;                              //ָ�����������ȡ���ݵ�λ��
unsigned char *MQTT_CMDEndPtr;                              //ָ���������������λ��

char ClientID[128];                                          //��ſͻ���ID�Ļ�����
int  ClientID_len;                                           //��ſͻ���ID�ĳ���

char Username[128];                                          //����û����Ļ�����
int  Username_len;											 //����û����ĳ���

char Passward[128];                                          //�������Ļ�����
int  Passward_len;											 //�������ĳ���

unsigned char ServerIP[4];                                   //��ŷ�����IP
unsigned char ServerName[128];                               //��ŷ���������
int  ServerPort;                                             //��ŷ������Ķ˿ں�

int   Fixed_len;                       					     //�̶���ͷ����
int   Variable_len;                     					 //�ɱ䱨ͷ����
int   Payload_len;                       					 //��Ч���ɳ���
unsigned char  temp_buff[BUFF_UNIT];						 //��ʱ������������������

char Connect_flag;        //ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
char ConnectPack_flag;    //CONNECT����״̬   1��CONNECT���ĳɹ�
char SubcribePack_flag;   //���ı���״̬      1�����ı��ĳɹ�

/*----------------------------------------------------------*/
/*����������ʼ������,����,�������ݵ� ������ �Լ���״̬����  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_Init(void)
{	
	MQTT_RxDataInPtr=MQTT_RxDataBuf[0];               //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_RxDataOutPtr=MQTT_RxDataInPtr;               //ָ���ͻ�������ȡ���ݵ�ָ���λ
    MQTT_RxDataEndPtr=MQTT_RxDataBuf[R_NUM-1];        //ָ���ͻ�����������ָ���λ
	
	MQTT_TxDataInPtr=MQTT_TxDataBuf[0];               //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_TxDataOutPtr=MQTT_TxDataInPtr;               //ָ���ͻ�������ȡ���ݵ�ָ���λ
	MQTT_TxDataEndPtr=MQTT_TxDataBuf[T_NUM-1];        //ָ���ͻ�����������ָ���λ
	
	MQTT_CMDInPtr=MQTT_CMDBuf[0];                     //ָ���������������ݵ�ָ���λ
	MQTT_CMDOutPtr=MQTT_CMDInPtr;                     //ָ�����������ȡ���ݵ�ָ���λ
	MQTT_CMDEndPtr=MQTT_CMDBuf[C_NUM-1];              //ָ���������������ָ���λ
	
	Connect_flag = 0;                                 //������������
	ConnectPack_flag = SubcribePack_flag = 0;         //������������
}
/*----------------------------------------------------------*/
/*�����������³�ʼ������,����,������� �Լ���״̬����     */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_ReInit(void)
{			
	unsigned char *MQTT_TxDatatempPtr;                 //ָ���ͻ�����λ�õ���ʱָ��
	
	if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){         //if�����Ļ���˵�����ͻ�������������
		MQTT_TxDataOutPtr = MQTT_TxDataInPtr;          //OUTָ��ָ��INָ��
		if(MQTT_TxDataOutPtr==MQTT_TxDataBuf[0]){      //���������OUTָ���ڻ���������������if
			MQTT_TxDataOutPtr =MQTT_TxDataBuf[T_NUM-4];//�ض�λOUTָ��
		}else if(MQTT_TxDataOutPtr==MQTT_TxDataBuf[1]){//���������OUTָ���ڻ�����������һ����Ԫ������if
		    MQTT_TxDataOutPtr =MQTT_TxDataBuf[T_NUM-3];//�ض�λOUTָ��
		}else if(MQTT_TxDataOutPtr==MQTT_TxDataBuf[2]){//���������OUTָ���ڻ�����������������Ԫ������if
		    MQTT_TxDataOutPtr =MQTT_TxDataBuf[T_NUM-2];//�ض�λOUTָ��
		}else{
			MQTT_TxDataOutPtr -= BUFF_UNIT;            //OUTָ������һ����Ԫ
			MQTT_TxDataOutPtr -= BUFF_UNIT;            //OUTָ������һ����Ԫ
			MQTT_TxDataOutPtr -= BUFF_UNIT;            //OUTָ������һ����Ԫ
		}			
		MQTT_TxDatatempPtr = MQTT_TxDataInPtr;         //����ǰINָ���λ���ݴ���tempָ����
		MQTT_TxDataInPtr = MQTT_TxDataOutPtr;          //INָ��ָ��ǰOUTָ��
		MQTT_ConectPack();                             //���ͻ�����������ӱ���
		MQTT_Subscribe(S_TOPIC_NAME,0);	               //���ͻ�������Ӷ���topic���ȼ�0									
		MQTT_TxDataInPtr = MQTT_TxDatatempPtr;         //INָ��ͨ��tempָ�룬����ԭ����λ��		
	}else{                                             //��֮��˵�����ͻ�����û������
		MQTT_ConectPack();                             //���ͻ�����������ӱ���
		MQTT_Subscribe(S_TOPIC_NAME,0);	               //���ͻ�������Ӷ���topic���ȼ�0		                                
	}
	ConnectPack_flag = SubcribePack_flag = 0;          //������������
}
/*----------------------------------------------------------*/
/*��������OneNet�Ƴ�ʼ���������õ��ͻ���ID���û���������    */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void OneNetIoT_Parameter_Init(void)
{	
	memset(ClientID,128,0);                              //�ͻ���ID�Ļ�����ȫ������
	sprintf(ClientID,"%s",DEVICEID);                     //�����ͻ���ID�������뻺����
	ClientID_len = strlen(ClientID);                     //����ͻ���ID�ĳ���
	
	memset(Username,128,0);                              //�û����Ļ�����ȫ������
	sprintf(Username,"%s",PRODUCTID);                    //�����û����������뻺����
	Username_len = strlen(Username);                     //�����û����ĳ���
	
	memset(Passward,128,0);                              //�û����Ļ�����ȫ������
	sprintf(Passward,"%s",AUTHENTICATION);               //�������룬�����뻺����
	Passward_len = strlen(Passward);                     //��������ĳ���
	
	ServerIP[0] = 183;                                   //������IP��ַ
	ServerIP[1] = 230;                                   //������IP��ַ
	ServerIP[2] = 40;                                    //������IP��ַ
	ServerIP[3] = 39;                                    //������IP��ַ
	ServerPort  = 6002;                                  //�������˿ں�6002
	
	u1_printf("�ͻ���ID��%s\r\n",ClientID);               //�������������Ϣ
	u1_printf("�� �� ����%s\r\n",Username);               //�������������Ϣ
	u1_printf("��    �룺%s\r\n",Passward);               //�������������Ϣ
}

/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	Fixed_len = 2;                                                        //���ӱ����У��̶���ͷ����=2
	Variable_len = 10;                                                    //���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //�ܱ��ĳ���       
	
	temp_buff[0]=0x10;                              //��1���ֽ� ���̶�0x01	
	temp_buff[1]=Variable_len + Payload_len;        //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���
	temp_buff[2]=0x00;          				    //��3���ֽ� ���̶�0x00	            
	temp_buff[3]=0x04;                              //��4���ֽ� ���̶�0x04
	temp_buff[4]=0x4D;								//��5���ֽ� ���̶�0x4D
	temp_buff[5]=0x51;								//��6���ֽ� ���̶�0x51
	temp_buff[6]=0x54;								//��7���ֽ� ���̶�0x54
	temp_buff[7]=0x54;								//��8���ֽ� ���̶�0x54
	temp_buff[8]=0x04;								//��9���ֽ� ���̶�0x04
	temp_buff[9]=0xC2;								//��10���ֽڣ�ʹ���û���������У�飬��ʹ���������������Ự
	temp_buff[10]=0x00; 						    //��11���ֽڣ�����ʱ����ֽ� 0x00
	temp_buff[11]=0x64;								//��12���ֽڣ�����ʱ����ֽ� 0x64   100s
	
	/*     CLIENT_ID      */
	temp_buff[12] = ClientID_len/256;                			  			//�ͻ���ID���ȸ��ֽ�
	temp_buff[13] = ClientID_len%256;               			  			//�ͻ���ID���ȵ��ֽ�
	memcpy(&temp_buff[14],ClientID,ClientID_len);                 			//���ƹ����ͻ���ID�ִ�	
	/*     �û���        */
	temp_buff[14+ClientID_len] = Username_len/256; 				  		    //�û������ȸ��ֽ�
	temp_buff[15+ClientID_len] = Username_len%256; 				 		    //�û������ȵ��ֽ�
	memcpy(&temp_buff[16+ClientID_len],Username,Username_len);              //���ƹ����û����ִ�	
	/*      ����        */
	temp_buff[16+ClientID_len+Username_len] = Passward_len/256;			    //���볤�ȸ��ֽ�
	temp_buff[17+ClientID_len+Username_len] = Passward_len%256;			    //���볤�ȵ��ֽ�
	memcpy(&temp_buff[18+ClientID_len+Username_len],Passward,Passward_len); //���ƹ��������ִ�

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);      //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic����                            */
/*��  ����QoS�����ĵȼ�                                     */
/*��  ����topic_name������topic��������                     */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	Fixed_len = 2;                              //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;                           //SUBSCRIBE�����У��ɱ䱨ͷ����=2	
	Payload_len = 2 + strlen(topic_name) + 1;   //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�
	
	temp_buff[0]=0x82;                                    //��1���ֽ� ���̶�0x82                      
	temp_buff[1]=Variable_len + Payload_len;              //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	temp_buff[2]=0x00;                                    //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3]=0x01;		                              //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4]=strlen(topic_name)/256;                  //��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5]=strlen(topic_name)%256;		          //��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));  //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�		
	temp_buff[6+strlen(topic_name)]=QoS;                  //���1���ֽڣ����ĵȼ�
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������PING���ģ�������                                  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	temp_buff[0]=0xC0;              //��1���ֽ� ���̶�0xC0                      
	temp_buff[1]=0x00;              //��2���ֽ� ���̶�0x00 

	TxDataBuf_Deal(temp_buff, 2);   //�������ݵ�������
}
/*----------------------------------------------------------*/
/*���������ȼ�0 ������Ϣ����                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */
/*��  ����data_len�����ݳ���                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	Fixed_len = 2;                             //�̶���ͷ���ȣ�2�ֽ�
	Variable_len = 2 + strlen(topic);          //�ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;                    //��Ч���ɳ��ȣ�����data_len
	
	temp_buff[0]=0x30;                         //��1���ֽ� ���̶�0xC0                
	temp_buff[1]=Variable_len + Payload_len;   //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	temp_buff[2]=strlen(topic)/256;            //��3���ֽ� ��topic���ȸ��ֽ�
	temp_buff[3]=strlen(topic)%256;		       //��4���ֽ� ��topic���ȵ��ֽ�
	memcpy(&temp_buff[4],topic,strlen(topic)); //��5���ֽڿ�ʼ ������topic�ַ���	
	memcpy(&temp_buff[4+strlen(topic)],data,data_len);   //����data����
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*����������������������ĵȼ�0������                       */
/*��  ����redata�����յ�����                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int  re_len;               	           //����һ����������Ž��յ������ܳ���
	int  pack_num;                         //����һ�����������������һ�����ʱ���������͵ĸ���
    int  temp_len;                         //����һ���������ݴ�����
    int  totle_len;                        //����һ������������Ѿ�ͳ�Ƶ����͵���������
	int  topic_len;              	       //����һ���������������������ĳ���
	int  cmd_len;                          //����һ����������������а������������ݵĳ���
	int  cmd_loca;                         //����һ����������������а������������ʼλ��
	int  i;                                //����һ������������forѭ��
	unsigned char temp[BUFF_UNIT];	       //��ʱ������
	unsigned char *data;                   //redata������ʱ�򣬵�һ���ֽ�������������data����ָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�ĵط�
		
	re_len = redata[0];                                   			//��ȡ���յ������ܳ���
	data = &redata[1];                                              //dataָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�� 
	pack_num = temp_len = totle_len = 0;                			//������������
	do{
		pack_num++;                                     			//��ʼѭ��ͳ�����͵ĸ�����ÿ��ѭ�����͵ĸ���+1
		temp_len = data[1+totle_len]+2;                 			//���㱾��ͳ�Ƶ����͵����ݳ���
		totle_len += temp_len;                          			//�ۼ�ͳ�Ƶ��ܵ����͵����ݳ���
		re_len -= temp_len ;                            			//���յ������ܳ��� ��ȥ ����ͳ�Ƶ����͵��ܳ���      
	}while(re_len!=0);                                  			//������յ������ܳ��ȵ���0�ˣ�˵��ͳ�������
	if(pack_num!=1)u1_printf("���ν�����%d����������\r\n",pack_num);//���������Ϣ
	temp_len = totle_len = 0;                		            	//������������
	for(i=0;i<pack_num;i++){                                        //�Ѿ�ͳ�Ƶ��˽��յ����͸�������ʼforѭ����ȡ��ÿ�����͵����� 
		topic_len = data[2+totle_len]*256+data[3+totle_len] + 2;    //���㱾����������������ռ�õ�������
		cmd_len = data[1+totle_len]-topic_len;                      //���㱾��������������������ռ�õ�������
		cmd_loca = totle_len + 2 +  topic_len;                      //���㱾�������������������ݿ�ʼ��λ��
		memcpy(temp,&data[cmd_loca],cmd_len);                       //�������ݿ�������		                 
		CMDBuf_Deal(temp, cmd_len);                                 //�������������
		temp_len = data[1+totle_len]+2;                             //��¼�������͵ĳ���
		totle_len += temp_len;                                      //�ۼ��Ѿ�ͳ�Ƶ����͵����ݳ���
	}	
}
/*----------------------------------------------------------*/
/*�������������ͻ�����                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_TxDataInPtr[1],data,size);      //�������ݵ����ͻ�����
	MQTT_TxDataInPtr[0] = size;                  //��¼���ݳ���
	MQTT_TxDataInPtr+=BUFF_UNIT;                 //ָ������
	if(MQTT_TxDataInPtr==MQTT_TxDataEndPtr)      //���ָ�뵽������β����
		MQTT_TxDataInPtr = MQTT_TxDataBuf[0];    //ָ���λ����������ͷ
}
/*----------------------------------------------------------*/
/*�������������������                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[1],data,size);      //�������ݵ��������
	MQTT_CMDInPtr[0] = size;                  //��¼���ݳ���
	MQTT_CMDInPtr[size+1] = '\0';             //�����ַ���������
	MQTT_CMDInPtr+=BUFF_UNIT;                 //ָ������
	if(MQTT_CMDInPtr==MQTT_CMDEndPtr)         //���ָ�뵽������β����
		MQTT_CMDInPtr = MQTT_CMDBuf[0];       //ָ���λ����������ͷ
}

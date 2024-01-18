#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "delay.h"        //������Ҫ��ͷ�ļ�
#include "wizchip_conf.h" //������Ҫ��ͷ�ļ�
#include "socket.h"       //������Ҫ��ͷ�ļ�
#include "dhcp.h"         //������Ҫ��ͷ�ļ�
#include "spi.h"          //������Ҫ��ͷ�ļ�
#include "dns.h"          //������Ҫ��ͷ�ļ�
#include "W5500_APP.h"    //������Ҫ��ͷ�ļ�
#include "24c02.h" 		    //������Ҫ��ͷ�ļ�
#include "usart4.h"
#include "SHT2X.h"
#include "main.h"         //������Ҫ��ͷ�ļ�

 
unsigned char  gRec_BUF[DATA_Rec_SIZE];    //���ݽ��ջ�����
unsigned char  gSend_BUF[100];    //���ݷ��ͻ�����
//unsigned char  gDNSBUF[DATA_Rec_SIZE];     //���ݻ�����
char Connect_flag = 0;          //���ӱ�־  0:δ�ɹ�  1���ɹ�
int  tcp_state;                 //���ڱ���TCP���ص�״̬
int  temp_state;                //������ʱ����TCP���ص�״̬
char my_dhcp_retry = 0;         //DHCP��ǰ�������ԵĴ���
char SOCK_flag = 0;             //�˿����ӱ�־ 0��û�����ӷ�����  1���Ѿ����ӷ��������ȴ���������Ӧ
wiz_NetInfo gWIZNETINFO =       //MAC��ַ�Լ�дһ������Ҫ��·�������������豸һ������
{        
0x00, 0x08, 0xdc,0x00, 0xab, 0xcd, 
}; 
/*---------------------------------------------------------------*/
/*         ������IP��ַ�Ͷ˿ںţ������Լ�������޸�              */
/*---------------------------------------------------------------*/
unsigned char DstIP[]={172,16,0,106};     //������IP��ַ
unsigned int  DstPort=40191; 

//unsigned char DstIP[]={47,96,28,113};     //������IP��ַ
//unsigned int  DstPort=40191; 
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
	int  ret,ret_s,ret_r;    //���ڱ��溯������ֵ
	int sended;  //ԭ�����ؽ��յ������ݳ���
//  char ch_cnt=27; 
	unsigned char Send_String[25];
	unsigned char Receive_String[25];

void w5500_app(void)
{
 
  switch(DHCP_run())           //�ж�DHCPִ�����ĸ�����
	{
		case DHCP_IP_ASSIGN:     //��״̬��ʾ��·���������������ip��
		case DHCP_IP_CHANGED:    //��״̬��ʾ��·�����ı��˷�����������ip
								 my_ip_assign();   //����IP������ȡ��������¼��������
								 break;            //����
		
		case DHCP_IP_LEASED:     //��״̬��ʾ��·����������Ŀ�����ip����ʽ�����ˣ���ʾ��������ͨ����
								 
								 ret=getSn_SR(SOCK_TCPS);       //��ȡTCP����״̬
		               
								 switch(ret)                    //�ж�socket0��״̬
								 {
									  case SOCK_INIT:          //��״̬��ʾ��SOCK��ʼ���ɹ���׼������  
										 u4_printf("Ready to connect to server\r\n");       //��ʾ��Ϣ  ׼�����ӷ�����
										 ret = connect(SOCK_TCPS,DstIP,DstPort);  //���ӷ�����
										 if(ret == SOCKERR_NOPEN)                 //�жϽ�����������SOCKERR_NOPEN����ʾ������δ����
										 {
											 u4_printf("The server is not turned on and will be reconnected in 3S\r\n");  //��ʾ��Ϣ  ������δ������3s��׼����������
											 Delay_Ms(3000);				                //3s��ʱ���ٴ��������ӷ�����													 
										 } 
										 break;	//����
										 
										case SOCK_ESTABLISHED:   //��״̬��ʾ�����ӽ����ɹ�
											   if((Connect_flag==0)&&(getSn_IR(SOCK_TCPS)==Sn_IR_CON))   //�ж�һ���˽�ɹ����
												 {
													 Connect_flag = 1;
													 u4_printf("Connection established\r\n");      //������ʾ��Ϣ  �����ѽ���
												 }
										     if(Time3_Count%40==0)           
													 Send_UseData();
													 Receive_UseData();
												 break;                                           //����
																 
									  case SOCK_CLOSE_WAIT:      //��״̬��ʾ���ȴ��ر�����
																 u4_printf("Wait for the connection to be closed\r\n");   //������ʾ��Ϣ  �ȴ��ر�����
																 if((ret=disconnect(SOCK_TCPS)) != SOCK_OK)  //�ر����ӣ����жϹرճɹ����
																 {
																	 u4_printf("Connection closing failed, ready to restart\r\n");   //��ʾ��Ϣ    ���ӹر�ʧ�ܣ�׼������
																	 Delay_Ms(500);                              //��ʱ500ms
																	 NVIC_SystemReset();	                     //����
																 }
										             Connect_flag = 0; 
																 u4_printf("Connection closed successfully\r\n");         //��ʾ�رճɹ�  ���ӹرճɹ�
																 break;  //����
																 
									  case SOCK_CLOSED:          //��״̬��ʾ�����ڹرս׶Σ��������ӷ�����
																 u4_printf("Ready to open local port\r\n");      //��ʾ��Ϣ  ׼���򿪱��ض˿�
																 ret = socket(SOCK_TCPS,Sn_MR_TCP,DstPort,Sn_MR_ND); //��socket0��һ���˿�
																 if(ret != SOCK_TCPS)                             //�жϣ��������ֵ������SOCK_TCPS����ʾ�򿪴���
																 {
																	  u4_printf("Port error, ready to restart\r\n");          //��ʾ��Ϣ �˿ڴ���׼������
																	  Delay_Ms(500);                             //��ʱ500ms
																	  NVIC_SystemReset();	                     //����
																 }
														     Connect_flag = 0; 
																 u4_printf("Opening local port succeeded\r\n");   //��ʾ��Ϣ   �򿪱��ض˿ڳɹ�
																 break;                                          //����
																 
									 default:             //����״̬
																 break;      //���� 
								 }
								 break;  //����
		
		case DHCP_FAILED:   //��״̬��ʾDHCP��ȡIPʧ��     									 
							my_dhcp_retry++;                        //ʧ�ܴ���+1
							if(my_dhcp_retry > MY_MAX_DHCP_RETRY){  //���ʧ�ܴ�������������������if							
								u4_printf("DHCP failed, ready to restart \r\n");//������ʾ��Ϣ  DHCPʧ�ܣ�׼������
								NVIC_SystemReset();		            //����
							}
							break;                                  //����
	}	
}

/*-------------------------------------------------*/
/*����������ȡ��IPʱ�Ļص�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void my_ip_assign(void)
{
   getIPfromDHCP(gWIZNETINFO.ip);     //�ѻ�ȡ����ip��������¼����������
   getGWfromDHCP(gWIZNETINFO.gw);     //�ѻ�ȡ�������ز�������¼����������
   getSNfromDHCP(gWIZNETINFO.sn);     //�ѻ�ȡ�������������������¼����������
   getDNSfromDHCP(gWIZNETINFO.dns);   //�ѻ�ȡ����DNS��������������¼����������
   gWIZNETINFO.dhcp = NETINFO_DHCP;   //���ʹ�õ���DHCP��ʽ
   network_init();                    //��ʼ������  
   u4_printf("DHCP lease term : %d S\r\n", getDHCPLeasetime());  //����
}
/*-------------------------------------------------*/
/*����������ȡIP��ʧ�ܺ���                         */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void my_ip_conflict(void)
{
	u4_printf("Failed to get IP address, ready to restart\r\n");   //��ʾ��ȡIPʧ��  ��ȡIPʧ�ܣ�׼������
	NVIC_SystemReset();                      //����
}
/*-------------------------------------------------*/
/*����������ʼ�����纯��                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void network_init(void)
{
	char tmpstr[6] = {0};
	wiz_NetInfo netinfo;
	
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);//�����������
	ctlnetwork(CN_GET_NETINFO, (void*)&netinfo);	//��ȡ�������
	ctlwizchip(CW_GET_ID,(void*)tmpstr);	        //��ȡоƬID

	//��ӡ�������
	if(netinfo.dhcp == NETINFO_DHCP) u4_printf("\r\n=== %s NET CONF : DHCP ===\r\n",(char*)tmpstr);
	else u4_printf("\r\n=== %s NET CONF : Static ===\r\n",(char*)tmpstr);	
  u4_printf("===========================\r\n");
	u4_printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",netinfo.mac[0],netinfo.mac[1],netinfo.mac[2],netinfo.mac[3],netinfo.mac[4],netinfo.mac[5]);			
	u4_printf("IP address: %d.%d.%d.%d\r\n", netinfo.ip[0],netinfo.ip[1],netinfo.ip[2],netinfo.ip[3]);
	u4_printf("default gateway: %d.%d.%d.%d\r\n", netinfo.gw[0],netinfo.gw[1],netinfo.gw[2],netinfo.gw[3]);
	u4_printf("Subnet mask: %d.%d.%d.%d\r\n", netinfo.sn[0],netinfo.sn[1],netinfo.sn[2],netinfo.sn[3]);
	u4_printf("DNS server: %d.%d.%d.%d\r\n", netinfo.dns[0],netinfo.dns[1],netinfo.dns[2],netinfo.dns[3]);
	u4_printf("===========================\r\n");
}
/*-------------------------------------------------*/
/*����������ʼ��W5500                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W5500_init(void)
{
	//W5500�շ��ڴ�������շ������������ܵĿռ���16K����0-7��ÿ���˿ڵ��շ����������Ƿ��� 2K
    char memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}}; 
	char tmp;
		
	SPI_Configuration();                                    //��ʼ��SPI�ӿ�
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);	//ע���ٽ�������
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  //ע��SPIƬѡ�źź���
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);	//ע���д����
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){   //���if��������ʾ�շ��ڴ����ʧ��
		 u4_printf("Failed to initialize transceiver partition, ready to restart\r\n");      //��ʾ��Ϣ  ��ʼ���շ�����ʧ��,׼������
  		 NVIC_SystemReset();                                //����
	}	
    do{                                                     //�������״̬
		 if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){ //���if��������ʾδ֪����		 
			u4_printf("Unknown error, ready to restart\r\n");            //��ʾ��Ϣ  δ֪����׼������
			NVIC_SystemReset();                             //����
		 }
		 if(tmp == PHY_LINK_OFF){
			 u4_printf("The network cable is not connected\r\n");//�����⵽������û���ӣ���ʾ��������
			 Delay_Ms(100);              //��ʱ
		 }
	}while(tmp == PHY_LINK_OFF);                            //ѭ��ִ�У�ֱ������������

	setSHAR(gWIZNETINFO.mac);                                   //����MAC��ַ
	DHCP_init(SOCK_DHCP, gRec_BUF);                             //��ʼ��DHCP
	reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);//ע��DHCP�ص����� 

	my_dhcp_retry = 0;	                                        //DHCP���Դ���=0
	tcp_state = 0;                                              //TCP״̬=0
	temp_state = -1;                                            //��һ��TCP״̬=-1
}


void Receive_UseData(void)
{
   char i;

	 if(getSn_IR(SOCK_TCPS) & Sn_IR_RECV)
    {
      setSn_IR(SOCK_TCPS, Sn_IR_RECV);														
    }
    if((ret=getSn_RX_RSR(SOCK_TCPS))>0)
		{
      memset(gRec_BUF,0,25);
			ret_r = recv(SOCK_TCPS,gRec_BUF,DATA_Rec_SIZE);    //��������Client������
	    if(ret_r == 25 && gRec_BUF[0]==0XEF && gRec_BUF[24]==0X1D && gRec_BUF[1]==0X06)   //�����������������0
			{
				for(i=0;i<ret_r;i++)
				{
					Receive_String[i] = gRec_BUF[i];
//					USART_SendData(USART1, Receive_String[i]);
//					Delay_Ms(1); 
				}
				send(SOCK_TCPS,Receive_String,ret_r);              //������ԭ�����ظ��ͻ���
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
				Sensor.PM2_5_Waring = Receive_String[21];
						
				u4_printf("---W5500--->%04d %3.2f'C %3.2f%% %03d \r\n",Sensor.DEV_Num,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
			}
		} 
}


void Send_UseData(void)
{
  char i;
	u32 chack_sum = 0;
		  Sensor.Year  = 2019;
	    Sensor.Month =  6;
	    Sensor.Day   =  5;
			Send_String[0] = 0xEF;  //��ͷ
//			if((Sensor.Temperature_Waring < Sensor.Temperature)||(Sensor.Humidity_Waring > Sensor.Humidity)||(Sensor.Smoke_Dev==1))
//			Send_String[1] = 0X05;  //��������
//			else
			Send_String[1] = 0X04;  //�豸����
			Send_String[2] = 'M';   //���� ��ʪ�Ȼ����̽����
			Send_String[3] = 'E';   //����
			Send_String[4] = 'I';   //������
 
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
			
			if(Sensor.Temperature && Sensor.Humidity)  //if
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
			ret_s = 25;
//		  sended = sendto(SOCK_UDPS,Send_String,ret_s,DstIP,DstPort);
			send(SOCK_TCPS,Send_String,ret_s);              //������ԭ�����ظ��ͻ���
//			for(i=0;i<23;i++)
//			{
//			  USART_SendData(USART1, Send_String[i]);
//				Delay_Ms(1); 
//			}
			
//     u4_printf("%04d %3.2f'C %3.2f%% %03d %3.2f'C %3.2f%% %03d\r\n",Sensor.DEV_Num,Sensor.Temperature,Sensor.Humidity,Sensor.Smoke_Dev,Sensor.Temperature_Waring,Sensor.Humidity_Waring,Sensor.PM2_5_Waring);
		
		if(sended != ret_s)  //������͵����������� ������ ���յ�����������
		{
//			u4_printf("�����ж�ʧ���\r\n");   //��ʾ��Ϣ
		}

//		AT24C02_ReadPage(0, out_String);
//		u4_printf("%s\r\n",out_String);
	
}














#ifndef  _W5500_APP_H_
#define  _W5500_APP_H_

#define SOCK_DHCP		    0      //DHCP功能用的端口，W5500有8个，0-7都行
#define SOCK_TCPS	        1      //TCP连接用的端口， W5500有8个，0-7都行
#define SOCK_UDPS			2      //开启UDP的端口（W5500端口0-7共计8个）
#define MY_MAX_DHCP_RETRY	3      //DHCP重试次数
#define DATA_Rec_SIZE       2048   //缓冲区大小



void W5500_init(void);
void my_ip_assign(void);                     //获取到IP时的回调函数  
void my_ip_conflict(void);                   //获取IP的失败函数
void network_init(void);                     //初始化网络函数
void w5500_app(void);
void Send_UseData(void);
void Receive_UseData(void);



#endif   // _W5500_H_

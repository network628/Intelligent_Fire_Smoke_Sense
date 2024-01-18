/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*          实现各种中断服务函数的源文件           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //包含需要的头文件
#include "stm32f10x_it.h"  //包含需要的头文件
#include "main.h"          //包含需要的头文件
#include "delay.h"         //包含需要的头文件
#include "usart1.h"        //包含需要的头文件
#include "led.h"           //包含需要的头文件
#include "usart2.h"        //包含需要的头文件
#include "timer4.h"        //包含需要的头文件
#include "wifi.h"	       //包含需要的头文件



/*-------------------------------------------------*/
/*函数名：串口2接收中断函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)   
{    
  #ifdef GPRS_BC26
	  if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)
    {
			  Usart2_RxBuff[Usart2_RxCounter]=USART_ReceiveData(USART2); //保存到缓冲区	
				Usart2_RxCounter ++;   //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }

    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
    {
        Usart2_RxBuff[Usart2_RxCounter]=USART_ReceiveData(USART2); //保存到缓冲区	
				Usart2_RxCounter ++;   //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
        USART_ClearFlag(USART2,USART_FLAG_ORE);
    }
	#endif
	
  #ifdef Wifi_ESP8266 	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //如果USART_IT_RXNE标志置位，表示有数据到了，进入if分支
		if(WiFi_Connect_flag==0){                                //如果Connect_flag等于0，当前还没有连接服务器，处于指令配置状态
			if(USART2->DR){                                 //处于指令配置状态时，非零值才保存到缓冲区	
				Usart2_RxBuff[Usart2_RxCounter]=USART2->DR; //保存到缓冲区	
				Usart2_RxCounter ++;                        //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
			}		
		}else{		                                        //反之Connect_flag等于1，连接上服务器了	
			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;   //把接收到的数据保存到Usart2_RxBuff中				
			if(Usart2_RxCounter == 0){    					//如果Usart2_RxCounter等于0，表示是接收的第1个数据，进入if分支				
				TIM_Cmd(TIM3,ENABLE); 
			}else{                        					//else分支，表示果Usart2_RxCounter不等于0，不是接收的第一个数据
				TIM_SetCounter(TIM3,0);  
			}	
			Usart2_RxCounter ++;         				    //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
		}		
	}
	#endif
}

u32 Time3_Count;
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
		if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
		{                //如果TIM_IT_Update置位，表示TIM4溢出中断，进入if	
			Usart2_RxCompleted = 1;                                       //串口2接收完成标志位置位
			memcpy(&Wifi_Data_buff[2],Usart2_RxBuff,Usart2_RxCounter);         //拷贝数据
			Wifi_Data_buff[0] = WiFi_RxCounter/256;                            //记录接收的数据量		
			Wifi_Data_buff[1] = WiFi_RxCounter%256;                            //记录接收的数据量
			Wifi_Data_buff[WiFi_RxCounter+2] = '\0';                           //加入结束符
			WiFi_RxCounter=0;                                             //清零计数值
			TIM_Cmd(TIM3, DISABLE);                        				  //关闭TIM4定时器
			TIM_SetCounter(TIM3, 0);                        			  //清零定时器4计数器
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     			  //清除TIM4溢出中断标志 
	  }

}

//定时器5中断服务程序	 
void TIM4_IRQHandler(void)
{ 
// 	if((TIM4CH4_CAPTURE_STA&0X80)==0)//还未成功捕获	
//	{
//		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
//		{	    
//			if(TIM4CH4_CAPTURE_STA&0X40)//已经捕获到高电平了
//			{
//				if((TIM4CH4_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
//				{
//					TIM4CH4_CAPTURE_STA|=0X80;//标记成功捕获了一次
//					TIM4CH4_CAPTURE_VAL=0XFFFF;
//				}else TIM4CH4_CAPTURE_STA++;
//			}	 
//		}
//	if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)//捕获1发生捕获事件
//		{	
//			if(TIM4CH4_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
//			{	  			
//				TIM4CH4_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
//				TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4);
//		   		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling); //CC4P=1 设置为下降沿捕获
//			}else  								//还未开始,第一次捕获上升沿
//			{
//				TIM4CH4_CAPTURE_STA=0;			//清空
//				TIM4CH4_CAPTURE_VAL=0;
//	 			TIM_SetCounter(TIM4,0);
//				TIM4CH4_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
//		    TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising );		//CC4P=0 设置为上升沿捕获
//			}		    
//		}			     	    					   
// 	}
//  TIM_ClearITPendingBit(TIM4, TIM_IT_CC4|TIM_IT_Update); //清除中断标志位
}
/*-------------------------------------------------*/
/*函数名：不可屏蔽中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：硬件出错后进入的中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：内存管理中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：预取指失败，存储器访问失败中断处理函数   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：未定义的指令或非法状态处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：软中断，SWI 指令调用的处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：调试监控器处理函数                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：可挂起的系统服务处理函数                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
}

/*-------------------------------------------------*/
/*函数名：SysTic系统嘀嗒定时器处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{
 
}

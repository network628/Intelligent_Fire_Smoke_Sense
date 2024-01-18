/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*          ʵ�ָ����жϷ�������Դ�ļ�           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //������Ҫ��ͷ�ļ�
#include "stm32f10x_it.h"  //������Ҫ��ͷ�ļ�
#include "main.h"          //������Ҫ��ͷ�ļ�
#include "delay.h"         //������Ҫ��ͷ�ļ�
#include "usart1.h"        //������Ҫ��ͷ�ļ�
#include "led.h"           //������Ҫ��ͷ�ļ�
#include "usart2.h"        //������Ҫ��ͷ�ļ�
#include "timer4.h"        //������Ҫ��ͷ�ļ�
#include "wifi.h"	       //������Ҫ��ͷ�ļ�



/*-------------------------------------------------*/
/*������������2�����жϺ���                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)   
{    
  #ifdef GPRS_BC26
	  if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)
    {
			  Usart2_RxBuff[Usart2_RxCounter]=USART_ReceiveData(USART2); //���浽������	
				Usart2_RxCounter ++;   //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }

    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
    {
        Usart2_RxBuff[Usart2_RxCounter]=USART_ReceiveData(USART2); //���浽������	
				Usart2_RxCounter ++;   //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
        USART_ClearFlag(USART2,USART_FLAG_ORE);
    }
	#endif
	
  #ifdef Wifi_ESP8266 	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //���USART_IT_RXNE��־��λ����ʾ�����ݵ��ˣ�����if��֧
		if(WiFi_Connect_flag==0){                                //���Connect_flag����0����ǰ��û�����ӷ�����������ָ������״̬
			if(USART2->DR){                                 //����ָ������״̬ʱ������ֵ�ű��浽������	
				Usart2_RxBuff[Usart2_RxCounter]=USART2->DR; //���浽������	
				Usart2_RxCounter ++;                        //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
			}		
		}else{		                                        //��֮Connect_flag����1�������Ϸ�������	
			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;   //�ѽ��յ������ݱ��浽Usart2_RxBuff��				
			if(Usart2_RxCounter == 0){    					//���Usart2_RxCounter����0����ʾ�ǽ��յĵ�1�����ݣ�����if��֧				
				TIM_Cmd(TIM3,ENABLE); 
			}else{                        					//else��֧����ʾ��Usart2_RxCounter������0�����ǽ��յĵ�һ������
				TIM_SetCounter(TIM3,0);  
			}	
			Usart2_RxCounter ++;         				    //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
		}		
	}
	#endif
}

u32 Time3_Count;
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
		if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
		{                //���TIM_IT_Update��λ����ʾTIM4����жϣ�����if	
			Usart2_RxCompleted = 1;                                       //����2������ɱ�־λ��λ
			memcpy(&Wifi_Data_buff[2],Usart2_RxBuff,Usart2_RxCounter);         //��������
			Wifi_Data_buff[0] = WiFi_RxCounter/256;                            //��¼���յ�������		
			Wifi_Data_buff[1] = WiFi_RxCounter%256;                            //��¼���յ�������
			Wifi_Data_buff[WiFi_RxCounter+2] = '\0';                           //���������
			WiFi_RxCounter=0;                                             //�������ֵ
			TIM_Cmd(TIM3, DISABLE);                        				  //�ر�TIM4��ʱ��
			TIM_SetCounter(TIM3, 0);                        			  //���㶨ʱ��4������
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     			  //���TIM4����жϱ�־ 
	  }

}

//��ʱ��5�жϷ������	 
void TIM4_IRQHandler(void)
{ 
// 	if((TIM4CH4_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
//	{
//		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
//		{	    
//			if(TIM4CH4_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
//			{
//				if((TIM4CH4_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
//				{
//					TIM4CH4_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
//					TIM4CH4_CAPTURE_VAL=0XFFFF;
//				}else TIM4CH4_CAPTURE_STA++;
//			}	 
//		}
//	if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)//����1���������¼�
//		{	
//			if(TIM4CH4_CAPTURE_STA&0X40)		//����һ���½��� 		
//			{	  			
//				TIM4CH4_CAPTURE_STA|=0X80;		//��ǳɹ�����һ��������
//				TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4);
//		   		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling); //CC4P=1 ����Ϊ�½��ز���
//			}else  								//��δ��ʼ,��һ�β���������
//			{
//				TIM4CH4_CAPTURE_STA=0;			//���
//				TIM4CH4_CAPTURE_VAL=0;
//	 			TIM_SetCounter(TIM4,0);
//				TIM4CH4_CAPTURE_STA|=0X40;		//��ǲ�����������
//		    TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising );		//CC4P=0 ����Ϊ�����ز���
//			}		    
//		}			     	    					   
// 	}
//  TIM_ClearITPendingBit(TIM4, TIM_IT_CC4|TIM_IT_Update); //����жϱ�־λ
}
/*-------------------------------------------------*/
/*�����������������жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ӳ������������жϴ�����             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*���������ڴ�����жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ԥȡָʧ�ܣ��洢������ʧ���жϴ�����   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������δ�����ָ���Ƿ�״̬������           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*�����������жϣ�SWI ָ����õĴ�����           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*�����������Լ����������                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*���������ɹ����ϵͳ��������                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
}

/*-------------------------------------------------*/
/*��������SysTicϵͳ��શ�ʱ��������             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{
 
}

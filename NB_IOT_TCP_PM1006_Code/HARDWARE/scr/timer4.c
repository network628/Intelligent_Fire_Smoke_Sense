#include "timer4.h"
#include "stm32f10x_tim.h"
 
////////////////////////////////////////////////////////////////////////////////// 	  
//��ʱ��4ͨ��4���벶������

//TIM_ICInitTypeDef  TIM4_ICInitStructure;

//void TIM4_Cap_Init(u16 arr,u16 psc)
//{	 
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ��TIM5ʱ��
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOAʱ��
//	
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;  //PA0 ���֮ǰ����  
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0 ��������  
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_9);						 //PA0 ����
//	
//	//��ʼ����ʱ��4 TIM4	 
//	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
//  
//	//��ʼ��TIM4���벶�����
//	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC4S=01 	ѡ������� IC4ӳ�䵽TI4��
//	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//�½��ز���
//	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
//	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
//	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
//	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
//	
//	//�жϷ����ʼ��
//	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
//	
//	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC4IE�����ж�	
//	TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��5
//}

//u8  TIM4CH4_CAPTURE_STA=0;	//���벶��״̬		    				
//u16	TIM4CH4_CAPTURE_VAL;	//���벶��ֵ



//u16 Get_PM1006_Value(void)
//{
//	 	static u32 temp=0; 
//		if(TIM4CH4_CAPTURE_STA&0X80)//�ɹ�������һ��������
//		{
//			temp=TIM4CH4_CAPTURE_STA&0X3F;
//			temp*=65536;   //���ʱ���ܺ�
//			temp+=TIM4CH4_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��
//			temp/=1000;    //ת����MS
////			temp=temp*3-4;
//			temp=temp*3+40;
//			TIM4CH4_CAPTURE_STA=0;//������һ�β���
//		}
//		return temp;
//}


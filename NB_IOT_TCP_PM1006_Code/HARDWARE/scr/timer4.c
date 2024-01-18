#include "timer4.h"
#include "stm32f10x_tim.h"
 
////////////////////////////////////////////////////////////////////////////////// 	  
//定时器4通道4输入捕获配置

//TIM_ICInitTypeDef  TIM4_ICInitStructure;

//void TIM4_Cap_Init(u16 arr,u16 psc)
//{	 
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM5时钟
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOA时钟
//	
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;  //PA0 清除之前设置  
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0 上拉输入  
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_9);						 //PA0 下拉
//	
//	//初始化定时器4 TIM4	 
//	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
//  
//	//初始化TIM4输入捕获参数
//	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC4S=01 	选择输入端 IC4映射到TI4上
//	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//下降沿捕获
//	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
//	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
//	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
//	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
//	
//	//中断分组初始化
//	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
//	
//	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断	
//	TIM_Cmd(TIM4,ENABLE ); 	//使能定时器5
//}

//u8  TIM4CH4_CAPTURE_STA=0;	//输入捕获状态		    				
//u16	TIM4CH4_CAPTURE_VAL;	//输入捕获值



//u16 Get_PM1006_Value(void)
//{
//	 	static u32 temp=0; 
//		if(TIM4CH4_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
//		{
//			temp=TIM4CH4_CAPTURE_STA&0X3F;
//			temp*=65536;   //溢出时间总和
//			temp+=TIM4CH4_CAPTURE_VAL;//得到总的高电平时间
//			temp/=1000;    //转换成MS
////			temp=temp*3-4;
//			temp=temp*3+40;
//			TIM4CH4_CAPTURE_STA=0;//开启下一次捕获
//		}
//		return temp;
//}


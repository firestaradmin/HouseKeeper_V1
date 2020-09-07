#include <stm32f10x.h>  
#include "stm32f10x_it.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "as608.h"
#include "EXTI.h"
#include "myTIM.h"
#include "servo.h"
#include "usmart.h"


u8 FP_id = 0;
u8 FP_Trig_Flag = 0;
u8 servo_Stauts = 0;	//门状态 1：开   0：关

int main()
{
	u8 ensure;
	SystemInit();//晶振时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	DelayInit();//延时函数初始化
	uart_init(115200);//串口1初始化
	usart2_init(usart2_baund);//初始化串口2,用于与指纹模块通讯
	usart3_init(115200);//WIFI模块
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	FP_WAK_Init();//指纹中断初始化
	TIM2_Int_Init(100, 7200 - 1); //T = 10ms
	user_led_init();
	//usmart_dev.init(72);
	servo_init();
	

	ensure=PS_ReadSysPara(&AS608Para);  //读AS608模块参数 
	if(ensure==0x00)
	{
		printf("\r\nRemainNum:%d    Level:%d", AS608Para.PS_max-ValidN,AS608Para.PS_level);
	}
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	//Add_FR();	//录指纹
	while(1)
	{
		
//		DelayMs(100);
//		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//		DelayMs(100);

		if(FP_WAK_Trig)
		{
			press_FR();
			FP_WAK_Trig = 0;
		}

		if(FP_Trig_Flag)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			servo_control(1);
			servo_Stauts = 1;
			FP_Trig_Flag = 0;
		}
		
	}
}


void user_led_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_DeInit(GPIOC);
	
	GPIO_InitTypeDef GPIOType = {
			GPIO_Pin_13,
			GPIO_Speed_50MHz,
			GPIO_Mode_Out_OD
	};
	
	GPIO_Init(GPIOC , &GPIOType);
}



//void WIFI_EN_GPIO_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	/* RST的IO口设置 */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB,GPIO_Pin_8);
//	

//}


void WIFI_send_id(u8 id)
{
	u8 buf[3] = {'[',id,']'};
	
	for(int i = 0; i < 3; i++){
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART3,buf[i]); 
	}
}






#include "EXTI.h"

u8 FP_WAK_Trig = 0;

void FP_WAK_Init(void)
{
	
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
//	//初始化读状态引脚GPIOA
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//输入下拉模式
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO	
	
	
	
	/* EXTI的时钟要设置AFIO寄存器 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE) ;
	
	EXTI_InitTypeDef EXTI_InitStruct;
	/* 初始化EXTI外设 */
	/* 选择作为EXTI线的GPIO引脚 */
	GPIO_EXTILineConfig( GPIO_PortSourceGPIOA , GPIO_PinSource0) ;
	/* 配置中断or事件线 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line0 ;
	/* 配置模式：中断or事件 */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt ;
	/* 配置边沿触发 上升or下降 */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising ;
	/* 使能EXTI线 */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE ;
	EXTI_Init(&EXTI_InitStruct) ;
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;  //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
	
	
	
	
}


void EXTI0_IRQHandler(void)
{

	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
	{
		FP_WAK_Trig = 1;
		//OLED_Show16X16CN_AND_8X16ASC(0,40,"TP_KEY is triged",OLED_DISPLAYCHAR_NORMAL);
		//OLED_Refresh_OneTime();
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
	
}



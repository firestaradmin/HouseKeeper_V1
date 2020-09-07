#include <stm32f10x.h> 


void RCC_cfg(void);
void NVIC_cfg(void);
void GPIOC_Init(void);


void RCC_cfg(void)
{
       //定义错误状态变量
       ErrorStatus HSEStartUpStatus;
       //将RCC寄存器重新设置为默认值
       RCC_DeInit();
       //打开外部高速时钟晶振
       RCC_HSEConfig(RCC_HSE_ON);
       //等待外部高速时钟晶振工作
       HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
       if(HSEStartUpStatus == SUCCESS)
       {
			//设置AHB时钟(HCLK)为系统时钟
			RCC_HCLKConfig(RCC_SYSCLK_Div1);
			//设置高速AHB时钟(APB2)为HCLK时钟
			RCC_PCLK2Config(RCC_HCLK_Div1);
			//设置低速AHB时钟(APB1)为HCLK的2分频
			RCC_PCLK1Config(RCC_HCLK_Div2);
			//设置FLASH代码延时
			FLASH_SetLatency(FLASH_Latency_2);
			//使能预取指缓存
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
			//设置PLL时钟，为HSE的9倍频 8MHz * 9 = 72MHz
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
			//使能PLL
			RCC_PLLCmd(ENABLE);
			//等待PLL准备就绪
			while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
			//设置PLL为系统时钟源
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
			//判断PLL是否是系统时钟
			while(RCC_GetSYSCLKSource() != 0x08);
       }
}

void GPIOC_Init(void)
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

void GPIOB_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_DeInit(GPIOB);
	
	GPIO_InitTypeDef GPIOType = {
			GPIO_Pin_0,
			GPIO_Speed_50MHz,
			GPIO_Mode_Out_PP
	};
	
	GPIO_Init(GPIOB , &GPIOType);
	

	
}
void NVIC_cfg(void)
{

	//选择中断分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //0-3

}

 


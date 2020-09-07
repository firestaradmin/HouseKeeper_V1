/***************************乐声电子科技有限公司****************************
**  工程名称：YS-V0.4语音识别开发板程序
**	CPU: STM32f103RCT6
**	晶振：8MHZ
**	波特率：9600 bit/S
**	配套产品信息：YS-V0.4语音识别开发板
**                http://yuesheng001.taobao.com
**  作者：zdings
**  联系：751956552@qq.com
**  修改日期：2012.4.11
**  说明：本程序 具备语音识别、串口通信、开发板基本功能演示。
***************************乐声电子科技有限公司******************************/
#include "reg_rw.h"

/*************端口信息********************
 * 接线说明
 * RST      PB6
 * CS   	PB8
 * WR/SPIS  PB9
 * P2/SDCK  PB3
 * P1/SDO   PB4
 * P0/SDI   PB5
 * IRQ      PC1
 * A0				PB7
 * RD       PA0
*****************************************/



/************************************************************************************
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
*********************************************************************************/
uint8 nAsrStatus=0;	

void LD3320_Init(void);



void LD3320_EXTI_Cfg(void);
void LD3320_Spi_cfg(void);
void LD3320_GPIO_Cfg(void);
void LED_gpio_cfg(void);

/***********************************************************
* 名    称： LD3320_main(void)
* 功    能： 主函数LD3320程序入口
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 

void  LD3320_main(void)
{
	uint8 nAsrRes=0;
	LD3320_Init();	  
	nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR
	while(1)
	{
// 	switch(nAsrStatus)
//		{
//			case LD_ASR_RUNING:
//			case LD_ASR_ERROR:		
//												break;
//			case LD_ASR_NONE:
//											nAsrStatus=LD_ASR_RUNING;
//											if (RunASR()==0)	//	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算
//											{		
//												nAsrStatus = LD_ASR_ERROR;
//											}
//											break;
//
//			case LD_ASR_FOUNDOK:
//											 nAsrRes = LD_GetResult( );	//	一次ASR识别流程结束，去取ASR识别结果										 					
//												 switch(nAsrRes)		   /*对结果执行相关操作,客户修改*/
//												  {
//													  case CODE_LSD:			/*命令“流水灯”*/
//															PrintCom(USART1,"“流水灯”命令识别成功\r\n"); /*text.....*/
//																						 break;
//														case CODE_SS:	 /*命令“闪烁”*/
//															PrintCom(USART1,"“闪烁”命令识别成功\r\n"); /*text.....*/
//																						 break;
//														case CODE_AJCF:		/*命令“按键触发”*/
//												
//															PrintCom(USART1,"“按键触发”命令识别成功\r\n"); /*text.....*/
//																						break;
//														case CODE_QM:		/*命令“全灭”*/
//												
//															PrintCom(USART1,"“全灭”命令识别成功\r\n"); /*text.....*/
//																						break;
//														default:break;
//													}	
//												nAsrStatus = LD_ASR_NONE;
//											break;
//			
//			case LD_ASR_FOUNDZERO:
//			default:
//								nAsrStatus = LD_ASR_NONE;
//								break;
			}//switch
	}// while


/***********************************************************
* 名    称：LD3320_Init(void)
* 功    能：模块驱动端口初始配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_Init(void)
{
	LD3320_GPIO_Cfg();	
	LD3320_Spi_cfg();	 
	LD_reset();
}

/***********************************************************
* 名    称： void Delay_( int i)
* 功    能： 短延时
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void Delay_( int i)
 {     
    while( i--)
       {	

        }
 }	
/***********************************************************
* 名    称：	LD3320_delay(unsigned long uldata)
* 功    能：	长延时函数
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
 void  LD3320_delay(u16 ms)
{
//	unsigned int j  =  0;
//	unsigned int g  =  0;
//	for (j=0;j<10;j++)
//	{
//		for (g=0;g<uldata;g++)
//		{
//			Delay_(1080);
//		}
//	}
   delay_us(120*ms);
}

/***********************************************************
* 名    称：	RunASR(void)
* 功    能：	运行ASR
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;
	for (i=0; i<5; i++)			//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD_AsrStart();			//初始化ASR
		delay_ms(100);
		if (LD_AsrAddFixed()==0)	//添加关键词语到LD3320芯片中
		{
			LD_reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay_ms(100);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		delay_ms(10);
		if (LD_AsrRun() == 0)
		{
			LD_reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay_ms(100);			//	并从初始化开始重新ASR识别流程
			continue;
		}
    LD_ReadReg(0xbf);
		asrflag=1;
		break;					//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
	}

	return asrflag;
}

/***********************************************************
* 名    称：LD3320_GPIO_Cfg(void)
* 功    能：初始化需要用到的IO口
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_GPIO_Cfg(void)
{	
	  GPIO_InitTypeDef GPIO_InitStructure;
	 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC,ENABLE);
		/*PC0:MODE, PC1:RSTB, PC2:WR*/
		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_0|GPIO_Pin_2;//;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
    
		GPIO_SetBits(GPIOC,GPIO_Pin_0);	/*MODE默认拉高  选择SPI*/
    GPIO_ResetBits(GPIOC,GPIO_Pin_2);	/*WR默认拉低*/
	 
}
/***********************************************************
* 名    称：LD3320_Spi_cfg(void)
* 功    能：配置SPI功能和端口初始化
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_Spi_cfg(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);	   //使能SPI2外设时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

	/*P0/P1/P2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
  GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			//CS(NSS)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  
	LD_CS_H();

	/* SPI2 配置 */
  SPI_Cmd(SPI2, DISABLE);	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;				                //主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					          //8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						              //时钟极性 空闲状态时，SCK保持高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //时钟相位 数据采样从第一个时钟边沿开始
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //软件产生NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;   //波特率控制 SYSCLK/128
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC多项式寄存器初始值为7
	SPI_Init(SPI2, &SPI_InitStructure);
	/* 使能SPI2 */
	SPI_Cmd(SPI2, ENABLE);

}
/***********************************************************
* 名    称： LD3320_EXTI_Cfg(void) 
* 功    能： 外部中断功能配置和相关端口配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_EXTI_Cfg(void)
{

}


/***********************************************************
* 名    称：  EXTI1_IRQHandler(void)
* 功    能： 外部中断函数
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1)!= RESET ) 
	{
 		ProcessInt0(); 
		EXTI_ClearFlag(EXTI_Line1);
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}


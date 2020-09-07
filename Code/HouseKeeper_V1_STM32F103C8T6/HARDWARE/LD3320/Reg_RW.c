/************************************************************************************/
//	版权所有：Copyright (c) 2005 - 2010 ICRoute INC.
/************************************************************************************/

#include "sys.h"
#include "Reg_RW.h"


void LD_WriteReg( unsigned char address, unsigned char dataout )
{
	LD_CS_L();
	// 这里添加硬件SPI口的操作代码：
	SPI_SendByte(0x04); // 发送 0x04
	SPI_SendByte(address); // 发送 address
	SPI_SendByte(dataout); // 发送 dataout
	LD_CS_H();
}
unsigned char LD_ReadReg( unsigned char address )
{
	LD_CS_L();
	// 这里添加硬件SPI口的操作代码：
	SPI_SendByte(0x05); // 发送 0x05
	SPI_SendByte(address); // 发送 address
	LD_CS_H();
	return (SPI_SendByte(0)); // 读出数据, 并返回
	
}
	

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
unsigned char SPI_TR( unsigned char TxData )
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据					    
}

void LD3320_SPI_init(void)
{

	/* 定义SPI结构体变量 */
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	
	/* SPI的IO口和SPI外设打开时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/* CS的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//初始high电平

	
	/* SPI的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);


	/* SPI的基本配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	  // 设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // 设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		  // 串行同步时钟的空闲状态为HIGH电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	  // 串行同步时钟的第1个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		  // NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;// 定义波特率预分频的值:波特率预分频值为16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;// 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;		  // CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  			  // 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI1, ENABLE); 	  // 使能SPI外设

}



void SIM_SPI_init()
{
		/* 定义SPI结构体变量 */
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	
	/* SPI的IO口打开时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


	/* CS的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//初始high电平
	
	/* CLK的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);					//初始high电平
	
	/* MISO的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);					//初始high电平
	
	/* MOSI的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);					//初始high电平
}
	
 

 

unsigned char SPI_SendByte(unsigned char dt)

{

	u8 i;

	u8 temp=0;

	for(i=8;i>0;i--)

	{

		if(dt&0x80)MOSI_H;

		else MOSI_L;

		dt<<=1;

		SCLK_H;

		__nop();

		__nop();

		__nop();

		temp<<=1;

		if(MISO)temp++;

		__nop();

		__nop();

		__nop();

		SCLK_L;

	}

	return temp;

}


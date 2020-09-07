/************************************************************************************/
//	版权所有：Copyright (c) 2005 - 2010 ICRoute INC.
/************************************************************************************/


// Reg_RW.h  读写LD3320芯片的寄存器，共有四种方法。
#ifndef REG_RW_H
#define REG_RW_H
#include "sys.h"

//sbit	LD_MODE	= P1^0;
#define LD_RST_H() GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define LD_RST_L() GPIO_ResetBits(GPIOA, GPIO_Pin_11)

#define LD_CS_H()	GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define LD_CS_L()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)

void LD_WriteReg( unsigned char address, unsigned char dataout );
unsigned char LD_ReadReg( unsigned char address );

unsigned char SPI_TR( unsigned char TxData );
void LD3320_SPI_init(void);




unsigned char SPI_SendByte(unsigned char dt);
void SIM_SPI_init();

#define MOSI_H GPIO_SetBits(GPIOA, GPIO_Pin_7)

#define MOSI_L GPIO_ResetBits(GPIOA, GPIO_Pin_7)

#define SCLK_H GPIO_SetBits(GPIOA, GPIO_Pin_5)

#define SCLK_L GPIO_ResetBits(GPIOA, GPIO_Pin_5)

#define MISO GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)




#endif


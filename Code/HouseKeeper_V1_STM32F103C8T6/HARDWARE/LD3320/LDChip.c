/***************************乐声电子科技有限公司****************************
**  文件名称：语音模块底层驱动
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
#include "ldchip.h"
#define delay_ms DelayMs
//#include "sdcard.h"
//#include "ff.h"
//#include "file.h"
/** @defgroup SD  source
* @{
*/
//FATFS    FsFat; //文件系统句柄
//FILETYP  ConfigFile;//SD卡存储的配置文件句柄
//BYTE file_stat;
static unsigned char stringBuf[10];    //for MFID from SD，仅限本文件使用
//u8*const KAIDENG="/开灯.mp3";		//GBK24的存放位置
//u8*const GUANDENG="/关灯.mp3";		//GBK24的存放位置
//u8*const DANWEI="/单位.mp3";		//GBK24的存放位置
//u8*const XINGMING="/姓名.mp3";		//GBK24的存放位置
//u8*const TEST_PAT="/erge.bin";		//GBK24的存放位置
//u8*const GUANDENG="/断桥残雪.mp3";
uint32  stringSize;  

/**
* @}
*/
extern uint8  nAsrStatus;
uint32 nMp3StartPos=0;
uint32 nMp3Size=0;
uint32 nMp3Pos=0;
uint32 nCurMp3Pos=0;
uint8  nLD_Mode = LD_MODE_IDLE;		//	用来记录当前是在进行ASR识别还是在播放MP3
uint8 bMp3Play=0;							//	用来记录播放MP3的状态
uint8 ucRegVal;
uint8 ucHighInt;
uint8 ucLowInt;
uint8 ucStatus;
uint8 ucSPVol=15; // MAX=15 MIN=0		//	Speaker喇叭输出的音量
uint8 buffer[10];
unsigned long fpPoint=0;

/***********************************************************
* 名    称：void LD_reset(void)
* 功    能：LD芯片硬件初始化
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD_reset(void)
{
   //u8 aa,bb,cc;
	LD_RST_H();
	delay_ms(1);
	LD_RST_L();
	delay_ms(1);
	LD_RST_H();
	delay_ms(1);
	LD_CS_L();
	delay_ms(1);
	LD_CS_H();		
	delay_ms(1);
}

/***********************************************************
* 名    称： void LD_Init_Common(void)
* 功    能： 初始化命令
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD_Init_Common(void)
{

	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	delay_ms(5);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	delay_ms(5);
	LD_WriteReg(0xCF, 0x43);   
	delay_ms(5);
	LD_WriteReg(0xCB, 0x02);
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);       
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19); 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}
	delay_ms(5);
	
	LD_WriteReg(0xCD, 0x04);
	LD_WriteReg(0x17, 0x4c); 
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}

/***********************************************************
* 名    称：void LD_Init_ASR(void)
* 功    能：初始化ASR
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 	
void LD_Init_ASR(void)
{
	nLD_Mode=LD_MODE_ASR_RUN;
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);	
	LD3320_delay( 100 );
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0x00);          
	LD_WriteReg(0x42, 0x08);
	LD_WriteReg(0x44, 0x00);    
	LD_WriteReg(0x46, 0x08); 
	LD3320_delay( 100 );
  LD_WriteReg(0x35, 0x4C); 
  LD3320_delay( 10 );
  LD_WriteReg(0xB3, 0x28); 
  LD_WriteReg(0xB4, 0x28); 
//  LD_WriteReg(0xB5, 0x64); 
  delay_ms( 10 );
}

/***********************************************************
* 名    称：void ProcessInt0(void)
* 功    能：识别处理函数
* 入口参数：  
* 出口参数：
* 说    明：可以利用查询方式，或中断方式进行调用
* 调用方法： 
**********************************************************/ 
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;
	u8 err;
	if(nLD_Mode == LD_MODE_ASR_RUN)
	{
		uint8 Ab2,Bf;
		ucRegVal = LD_ReadReg(0x2B);
		Ab2 = LD_ReadReg(0xc5);
		Bf = LD_ReadReg(0xc7);
		//     LCD_ShowxNum(100,150,ucRegVal,2,24,0);
		//     LCD_ShowxNum(150,150,Ab2,4,24,0);
		//     LCD_ShowxNum(200,150,Bf,4,24,0);
		printf("ucRegVal:%d\r\n",ucRegVal);
		printf("Ab2:%d\r\n",Ab2);
		printf("Bf:%d\r\n",Bf);
		// 语音识别产生的中断
		// （有声音输入，不论识别成功或失败都有中断）
		LD_WriteReg(0x29,0) ;
		LD_WriteReg(0x02,0) ;
		if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
		{	 
				nAsrResCount = LD_ReadReg(0xba);
				//LCD_ShowString(100,200,100,24,24,"enter");
				printf("enter\r\n");
				//OSFlagPost(Flags,0x0001,OS_FLAG_SET,&err);/* 识别成功 1*/
				if(nAsrResCount>0 && nAsrResCount<=4) 
			{
				//LCD_ShowString(200,200,100,24,24,"success");
				printf("success\r\n");
				//LCD_ShowxNum(300,150,nAsrResCount,4,24,0);
				printf("nAsrResCount:%d\r\n",nAsrResCount);
				nAsrStatus=LD_ASR_FOUNDOK; 	
				LD_WriteReg(0xb2,  0);
			}
			else
			{
				//                 LD_reset();
				//LCD_ShowString(200,230,100,24,24,"enter fail");
				printf("enter fail\r\n");
				nAsrStatus=LD_ASR_FOUNDZERO;
			}	
		}
		if( LD_ReadReg(0xbf)==0x35 ) 
		{
			//OSFlagPost(Flags,0x0002,OS_FLAG_SET,&err);/* 识别失败 2*/
			//LCD_ShowString(100,230,100,24,24,"fail");
			printf("fail\r\n");
			//        LD_reset();
			delay_ms(20);
			ucRegVal = LD_ReadReg(0x2B);

			nAsrStatus=LD_ASR_FOUNDZERO;	//执行没有识别
		}

		//     LD_WriteReg(0x2b, 0);
		//		  LD_WriteReg(0x1C,0);/*写0:ADC不可用*/

		//     LD_WriteReg(0x29,0) ;
		//     LD_WriteReg(0x02,0) ;
		//     LD_WriteReg(0x2B,  0);
		//     LD_WriteReg(0xBA, 0);	
		//     LD_WriteReg(0xBC,0);	
		//     LD_WriteReg(0x08,1);	 /*清除FIFO_DATA*/
		//     LD_WriteReg(0x08,0);	/*清除FIFO_DATA后 再次写0*/
		//      LD_WriteReg(0x1C,0x0b);
		//      LD_AsrRun();
		//LD3320_delay(1);
		//RunASR();
	}
	if(nLD_Mode == LD_MODE_MP3 )
	{
		ucRegVal = LD_ReadReg(0x2B);
		ucHighInt = LD_ReadReg(0x29); 
		ucLowInt=LD_ReadReg(0x02); 
		LD_WriteReg(0x29,0) ;
		LD_WriteReg(0x02,0) ;
		if( LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
		{
			// A. 声音数据已全部播放完。

			LD_WriteReg(0x2B,0);	
			LD_WriteReg(0xBA, 0);	
			LD_WriteReg(0xBC,0x0);	
			bMp3Play=0;					// 声音数据全部播放完后，修改bMp3Play的变量
			nMp3Pos = 0;
			LD_WriteReg(0x08,1);
			delay_ms(1);
			LD_WriteReg(0x08,0);
			LD_WriteReg(0x33, 0);
			//        f_close(&ConfigFile);
			//OSFlagPost(Flags,0x0002,OS_FLAG_SET,&err);/* 播放完成 2*/
			return ;
		}
		//     
		if(nMp3Pos>=nMp3Size)
		{
			// B. 声音数据已发送完毕。

			LD_WriteReg(0xBC,0x01);
			LD_WriteReg(0x29,0x10) ;
			//		bMp3Play=0;				//	此时，只是主控MCU把所有MP3数据发送到LD3320芯片内，但是还没有把送入的数据全部播放完毕
			return;	
		}
		//        if(nMp3Pos == nMp3Size)
		//        {
		//           bMp3Play = 0;
		//           OSFlagPost(Flags,0x0002,OS_FLAG_SET,&err);/* 识别失败 2*/
		//           return;
		//        }
		   
		//LD_ReloadMp3Data();

		LD_WriteReg(0x29,ucHighInt);
		LD_WriteReg(0x02,ucLowInt) ;
		//     
	}
//  if((LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END))
//     return;
//     if(nMp3Pos>=nMp3Size)
//        LD_WriteReg(0xBC,0x01);
//     else
//     {
//        if(LD_ReadReg(0x2B)&0x04 )
//        {
//           LD_ReloadMp3Data();
//           LD_WriteReg(0x2B,0x00);
//        }
//    
//     }

}

/***********************************************************
* 名    称：uint8 LD_Check_ASRBusyFlag_b2(void)
* 功    能：检测 ASR 是否忙状态
* 入口参数：flag ---1：空闲状态  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 LD_Check_ASRBusyFlag_b2(void)
{
	uint8 j;
	uint8 flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		delay_ms(10);		
	}
	return flag;
}
/***********************************************************
* 名    称： void LD_AsrStart(void)
* 功    能：
* 入口参数：  ASR初始化
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD_AsrStart(void)
{
	LD_Init_ASR();
}
/***********************************************************
* 名    称： uint8 LD_AsrRun(void)
* 功    能： ASR执行函数
* 入口参数：  
* 出口参数：
* 说    明： VAD系列寄存器
* 调用方法： B3：语音端点检测，越小越灵敏，距离越远，误判更大，环境嘈杂用大值
          ： B4：需要连续多久才能认为是语音开始，默认150ms，单位10ms
          ： B5：与B4相反，默认600ms
          ： B6：检测到语音开始后，允许多久的时间，默认6秒，单位100ms
          ： B7：忽略前面多久的数据，单位20ms，默认40ms；
          ： B8：最长识别长度，默认60s；
**********************************************************/ 
uint8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);                /*麦克风音量:越大越灵敏，误识别越多，嘈杂环境用小值*/
  LD_WriteReg(0xB3, 0x28);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);                   /*清除FIFO内容第0位：写入1→清除FIFO_DATA 第2位：写入1→清除FIFO_EXT*/
	delay_ms( 1 );
	LD_WriteReg(0x08, 0x00);                   /*清除FIFO内容第0位（清除指定FIFO后再写入一次00H）*/
	delay_ms( 1);

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);                   /*写06H 通知DSP开始识别语音*/
	delay_ms( 1);
	LD_WriteReg(0x1C, 0x0b);                   /*ADC开关控制  写0BH 麦克风输入ADC通道可用*/
	LD_WriteReg(0x29, 0x10);                   /*中断允许 第2位：FIFO 中断允许 第4位：同步中断允许 1允许；0不允许*/
	LD_WriteReg(0xBD, 0x00);                   /*初始化控制寄存器 写入00 然后启动 为ASR模块 */
	return 1;
}

/***********************************************************
* 名    称：uint8 LD_AsrAddFixed(void)
* 功    能：添加识别关键词语
* 入口参数：  
* 出口参数： flag-- 1：success
* 说    明：开发者可以学习"语音识别芯片LD3320高阶秘籍.pdf"中
						关于垃圾词语吸收错误的用法，提高识别率。
* 调用方法： 
**********************************************************/ 
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 20    /*数组二维数值*/
	#define DATE_B 50		/*数组一维数值*/
	 uint8  sRecog[DATE_A][DATE_B] = {"guang dong","he nan","si chuan","xin nian hao","kai deng",
"guan deng","dan wei"," ming zi","xin ming","deng","dan","wei","kai","guan","xing","ming","xin nian","guang"};	/*添加关键词，用户修改*/
	 uint8  pCode[DATE_A] = {CODE_GUANGDONG,CODE_HENAN,CODE_SICHUAN,CODE_CHILD,CODE_OPEN,CODE_CLOSE,CODE_CPN,CODE_QM,CODE_QM,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_CHILD,CODE_ERRO};	/*添加识别码，用户修改*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k] );
		LD_WriteReg(0xc3, 0 ); 
		LD_WriteReg(0x08, 0x04);
		delay_ms(1);
		LD_WriteReg(0x08, 0x00);
		delay_ms(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);
//		LD_WriteReg(0x37, 0x04);
	}	 

    return flag;
}

/***********************************************************
* 名    称： uint8 LD_GetResult(void)
* 功    能： 获取识别结果
* 入口参数：  
* 出口参数： 返回识别码 LD_ReadReg(0xc5 );
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5 );
}
/******************************************************************************* 
* Function     : LD_Init_MP3 
* Description  : --MP3初始化 
* Input	       : none 
* Output       : none 
* Created      : 2018/2/3, by WangLei
*******************************************************************************/ 
void LD_Init_MP3(void)
{
   nLD_Mode = LD_MODE_MP3;
   LD_Init_Common();
   
   LD_WriteReg(0xBD,0x02);
   LD_WriteReg(0x17, 0x48);
   delay_ms(10);
   LD_WriteReg(0x85, 0x52); 
   LD_WriteReg(0x8F, 0x00);  
   LD_WriteReg(0x81, 0x00);
   LD_WriteReg(0x83, 0x00);
   LD_WriteReg(0x8E, 0xff);
   LD_WriteReg(0x8D, 0xff);
   delay_ms(1);
   LD_WriteReg(0x87, 0xff);
   LD_WriteReg(0x89, 0xff);
   delay_ms(1);
   LD_WriteReg(0x22, 0x00);    
   LD_WriteReg(0x23, 0x00);
   LD_WriteReg(0x20, 0xef);    
   LD_WriteReg(0x21, 0x07);
   LD_WriteReg(0x24, 0x77);          
   LD_WriteReg(0x25, 0x03);
   LD_WriteReg(0x26, 0xbb);    
   LD_WriteReg(0x27, 0x01); 

}
/******************************************************************************* 
* Function     : LD_AdjustMIX2SPVolume 
* Description  : --声音播放音量调节  8e寄存器 参数val表示音量值，范围0-15
* Input	       : val 
* Output       : none 
* Created      : 2018/2/3, by WangLei
*******************************************************************************/ 
void LD_AdjustMIX2SPVolume(u8 val)		   
{
	ucSPVol = val;
	val = ((15-val)&0x0f) << 2;
	LD_WriteReg(0x8E, val | 0xc3); 
  LD_WriteReg(0x87, 0xF8);
}	
/******************************************************************************* 
* Function     : LD_play() 
* Description  : --播放MP3 
* Input	       :  
* Output       :  
* Created      : 2018/2/3, by WangLei
*******************************************************************************/ 
//void LD_play(u8* Name)
//{
//	nMp3Pos=0;
//	bMp3Play=1;
//  SD_Read_function(Name);
//  nMp3Size = ConfigFile.fsize;
//	if (nMp3Pos >=  nMp3Size)
//		return ; 
//	LD_ReloadMp3Data();						   //放入MP3长度
//  LD_WriteReg(0xBA, 0x00); //					   读
//	LD_WriteReg(0x17, 0x48);					   //激活DSP
//	LD_WriteReg(0x33, 0x01);					   //开始播放
//	LD_WriteReg(0x29, 0x04);											  
//	LD_WriteReg(0x02, 0x01); 					  //允许FIFO_DATA中断
//	LD_WriteReg(0x85, 0x5A);					  //播放
//  
//}
///******************************************************************************* 
//* Function     : LD_ReloadMp3Data 
//* Description  : --装载MP3数据 
//* Input	       :  
//* Output       :  
//* Created      : 2018/2/3, by WangLei
//*******************************************************************************/ 
//void LD_ReloadMp3Data()		  //送入数据 SPI
//{
//   OS_CPU_SR cpu_sr = 0;
//   OS_ENTER_CRITICAL();
//	ucStatus = LD_ReadReg(0x06);
//	while ( !(ucStatus&MASK_FIFO_STATUS_AFULL) && nMp3Pos<nMp3Size )
//	{
//        stringBuf[0]=0;
//        file_read(&ConfigFile,stringBuf,1,&stringSize);//修改了SD卡时钟 1M  将文件内容读出到数据缓冲区
//        LD_WriteReg(0x01,stringBuf[0]);
//        nMp3Pos++;
//        ucStatus = LD_ReadReg(0x06);
//		
//	}
//  OS_EXIT_CRITICAL();
//}
///******************************************************************************* 
//* Function     : SD_Read_function() 
//* Description  : --自定义读取函数 
//* Input	       : none 
//* Output       : none 
//* Created      : 2017/12/25, by WangLei
//*******************************************************************************/ 
//void SD_Read_function( u8* name)
//{
//   f_mount (&FsFat,"1:",1);
//   f_chdrive("1:");//运行此句才能将当前盘符改写为1：即sd卡，否则默认是0：
//   u8 result;
//   result= file_open(&ConfigFile,(const TCHAR*)name);
////   result = ConfigFile.fsize;
////   file_read_anyposition(&ConfigFile,stringBuf,0,10,&stringSize);
////   f_close(&ConfigFile);
//}

//#define LD_RST_H() GPIO_SetBits(GPIOA, GPIO_Pin_11)
//#define LD_RST_L() GPIO_ResetBits(GPIOA, GPIO_Pin_11)

//#define LD_CS_H()	GPIO_SetBits(GPIOA, GPIO_Pin_4)
//#define LD_CS_L()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
void LD3320_GPIO_Cfg(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;//;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	LD_RST_H();
	//LD3320_SPI_init();
	 
}

void LD3320_init(void)
{
	LD3320_GPIO_Cfg();	
	//LD3320_SPI_init();	
	SIM_SPI_init();	
	LD_reset();
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

//uint8 LD_Check_ASRBusyFlag_b2()
//{
//	uint8 j;
//	uint8 flag = 0;
//	for (j=0; j<10; j++)
//	{
//	if (LD_ReadReg(0xb2) == 0x21)
//	{
//	flag = 1;
//	break;
//	}
//	delay(10); 
//	}
//	return flag;
//}

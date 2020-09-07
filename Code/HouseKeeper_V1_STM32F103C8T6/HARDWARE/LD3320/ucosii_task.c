/** 
******************************************************************************** 
* @file        : ucosii_task.c 
* Description  : --ucosii任务定义 
* @author      : wanglei 
* @date        : 2018/1/31
******************************************************************************** 
**/

/* 在sys.h中来启动UCOSii中的delay*/
/* Includes ------------------------------------------------------------------*/
#include "ucosii_task.h"
/** @defgroup EVENT Definitions
  * @{
  */
OS_FLAG_GRP *Flags;
OS_TMR *Tmr_1;
OS_EVENT *Sem;
u8*const KAIDENG="/开灯.mp3";		//GBK24的存放位置
u8*const GUANDENG="/关灯.mp3";		//GBK24的存放位置
u8*const DANWEI="/公司.mp3";		//GBK24的存放位置
u8*const XINGMING="/拜年.mp3";		//GBK24的存放位置
u8*const ERTONG="/儿童.mp3";		//GBK24的存放位置
u8*const SICHUAN="/四川.mp3";		//GBK24的存放位置
u8*const GUANGDONG="/广东.mp3";		//GBK24的存放位置
u8*const HENAN="/河南.mp3";		//GBK24的存放位置
/**
  * @}
  */

/** @defgroup task stacks Definitions
  * @{
  */
OS_STK START_TASK_STK[START_STK_SIZE];
OS_STK LCD_TASK_STK[LCD_STK_SIZE];
OS_STK RTC_TASK_STK[RTC_STK_SIZE];
OS_STK ASR_TASK_STK[ASR_STK_SIZE];
/**
  * @}
  */
/******************************************************************************* 
* Function     : TASK_CREATE() 
* Description  : --建立启动任务 
* Input	       : none 
* Output       : none 
* Created      : 2018/1/31, by WangLei
*******************************************************************************/ 
void START_TASK_CREATE()
{
   OSTaskCreate(Start_Task,(void *)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRO);
}
/******************************************************************************* 
* Function     : Start_Task() 
* Description  : --启动函数 
* Input	       : none 
* Output       : none 
* Created      : 2018/1/31, by WangLei
*******************************************************************************/ 
void Start_Task(void* pdata)
{
   u8 err;
  OS_CPU_SR cpu_sr = 0;
  pdata = pdata;
  Flags = OSFlagCreate(0,&err);
  Sem = OSSemCreate(0);
  
  OSStatInit();
  OS_ENTER_CRITICAL();
  Tmr_1 = OSTmrCreate(10,20,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)TIMR1_CallBack,0,"Tmr_1",&err);
  OSTaskCreate(Lcd_Show,(void *)0,(OS_STK*)&LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRO);
  OSTaskCreate(ASR_ROLL,(void *)0,(OS_STK*)&ASR_TASK_STK[ASR_STK_SIZE-1],ASR_TASK_PRO);
  OSTmrStart(Tmr_1,&err);
  OSTaskSuspend(START_TASK_PRO);
  OS_EXIT_CRITICAL();
  
}
/******************************************************************************* 
* Function     : Lcd_Show() 
* Description  : --液晶显示函数 
* Input	       : none 
* Output       : none 
* Created      : 2018/1/31, by WangLei
*******************************************************************************/ 
void Lcd_Show(void* pdata)
{
   pdata = pdata;
   u8 err; 
   while(1)
   {
      OSSemPend(Sem,0,&err);
//      Flags_recive = OSFlagPend(Flags,0x0001,OS_FLAG_WAIT_SET_ANY,0,&err);
         RTC_Get();
      LCD_ShowNum(0,0,calendar.w_year,4,24);
      LCD_ShowNum(60,0,calendar.w_month,2,24);
      LCD_ShowNum(96,0,calendar.w_date,2,24);
      LCD_ShowNum(140,0,calendar.hour,2,24);
      LCD_ShowNum(176,0,calendar.min,2,24);
      LCD_ShowNum(212,0,calendar.sec,2,24);
      
//      OSFlagPost(Flags,0x0000,OS_FLAG_CLR,&err);
   }
   
}
/******************************************************************************* 
* Function     :  Rtc_Gui() 
* Description  : --时间显示位置 
* Input	       : none 
* Output       : none 
* Created      : 2018/1/31, by WangLei
*******************************************************************************/ 
void  Rtc_Gui()
{
   POINT_COLOR = BLUE;
   LCD_ShowString(48,0,200,24,24,"/");
   LCD_ShowString(84,0,100,24,24,"/");
   LCD_ShowString(164,0,100,24,24,":");
   LCD_ShowString(200,0,100,24,24,":");
}
/******************************************************************************* 
* Function     : ASR_ROLL() 
* Description  : --实时语音识别 
* Input	       : none 
* Output       : none 
* Created      : 2018/1/31, by WangLei
*******************************************************************************/ 
void ASR_ROLL(void* pdata)
{
   u8 err;
   u16 Flags_recive;
   pdata = pdata;
//      LD_Init_MP3();
//      LD_AdjustMIX2SPVolume(7);
//      LD_play(KAIDENG);
   RunASR();
   while(1)
   {
      Flags_recive = OSFlagPend(Flags,0x0003,OS_FLAG_WAIT_SET_ANY,0,&err);
      if(Flags_recive == 0x0001)
      {
         LD_reset();
         LD_Init_MP3();
         LD_AdjustMIX2SPVolume(2);
         switch(LD_ReadReg(0xc5))
         {
          case 1:LD_play(KAIDENG);LCD_LED=1;
            break;  
          case 2:LD_play(GUANDENG);LCD_LED=0;
            break;
          case 3:LD_play(DANWEI);
            break;
          case 4:LD_play(XINGMING);
            break;
          case 5:LD_play(ERTONG);
            break;
          case 6:LD_play(SICHUAN);
            break;
          case 7:LD_play(GUANGDONG);
            break;
          case 8:LD_play(HENAN);
            break;
          default: LD_reset(); RunASR();

            break;
         }
         if(bMp3Play == 0)
         {
           
         }
             
      }
      if(Flags_recive == 0x0002 &&  bMp3Play==0 )
      {
         LD_reset();
         RunASR();
      }
      delay_ms(10);
      OSFlagPost(Flags,0x000f,OS_FLAG_CLR,&err);
   }
}
/******************************************************************************* 
* Function     : timr1_callback() 
* Description  : -- 
* Input	       : ptmr parg 
* Output       :  
* Created      : 2018/2/2, by WangLei
*******************************************************************************/ 
void TIMR1_CallBack(OS_TMR*ptmr,void *parg)
{
   u8 err;
//   if(LD_ReadReg(0x2B)&0x04)
    ProcessInt0();
//         OSSemPost(Sem);
//   LCD_ShowxNum(300,150,nMp3Pos,20,24,0);
//   LCD_ShowxNum(300,180,LD_ReadReg(0x2B),20,24,0);
//   LCD_ShowxNum(300,210,LD_ReadReg(0x06),20,24,0);
//   if((LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END))
//      return;
//   if(nMp3Pos>=nMp3Size)
//      LD_WriteReg(0xBC,0x01);
//   else
//   {
//      if(LD_ReadReg(0x2B)&0x04 )
//      {
////         LD_ReloadMp3Data();
//         OSSemPost(Sem);
//         LD_WriteReg(0x2B,0x00);
//      }
//      
//   }
}


/** 
******************************************************************************** 
* @file        : ucosii_task.h 
* Description  : --ucosii任务定义 
* @author      : wanglei 
* @date        : 2018/1/31
******************************************************************************** 
**/

#ifndef UCOSII_TASK_H
#define UCOSII_TASK_H
/* Includes ------------------------------------------------------------------*/
#include <includes.h>
#include "stm32f10x.h"
#include "rtc.h"
#include "ili93xx.h"
#include "reg_rw.h"
#include "delay.h"
/** @defgroup EVENT Definitions
  * @{
  */
extern OS_FLAG_GRP *Flags;
extern OS_TMR *Tmr_1;
extern OS_EVENT *Sem;
/**
  * @}
  */
/** @defgroup START Definitions
  * @{
  */
#define START_TASK_PRO   10
#define START_STK_SIZE   128
void Start_Task(void* pdata);
/**
  * @}
  */
/** @defgroup LCD_SHOW Definitions
  * @{
  */
#define LCD_TASK_PRO  8
#define LCD_STK_SIZE  64
void Lcd_Show(void* pdata);
/**
  * @}
  */
/** @defgroup RTC Definitions
  * @{
  */
#define RTC_TASK_PRO  7
#define RTC_STK_SIZE  32
void Rtc_Show(void* pdata);
/**
  * @}
  */
/** @defgroup ASR Definitions
  * @{
  */
#define ASR_TASK_PRO 5
#define ASR_STK_SIZE 512
void ASR_ROLL(void* pdata);
/**
  * @}
  */
/** @defgroup function Definitions
  * @{
  */
void START_TASK_CREATE();
void Rtc_Gui();
void TIMR1_CallBack(OS_TMR*ptmr,void *parg);
/**
  * @}
  */


#endif
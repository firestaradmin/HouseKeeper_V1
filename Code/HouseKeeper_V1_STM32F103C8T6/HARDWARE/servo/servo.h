
/************************************************************************************
*  Copyright (c), 2020, LXG.
*
* FileName		:
* Author		:firestaradmin
* Version		:1.0
* Date			:2020.1.19
* Description	:RH6015 ´¥Ãþ¡¤IC
* History		:
*
*
*************************************************************************************/
#ifndef __SERVO_H_
#define __SERVO_H_
	
#include "sys.h"
void servo_init(void);
void servo_set_compare(u16 value);
void servo_control(u8 value);
#endif




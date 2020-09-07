/************************************************************************************
*  Copyright (c), 2020, LXG.
*
* FileName		:
* Author		:firestaradmin
* Version		:1.0
* Date			:
* Description	:
* History		:
*
*
*************************************************************************************/
#include "servo.h"
#include "myTIM.h"

#define SERVO_OPEN  5
#define SERVO_CLOSE 12



void servo_init()
{
	TIM3_PWM_Init(99,14400);	//T = 20ms    50Hz
	TIM_SetCompare3(TIM3,10);
	servo_control(0);
}


void servo_set_compare(u16 value)
{
	
	TIM_SetCompare3(TIM3,value);
}

void servo_control(u8 value)
{
	if(value)
		servo_set_compare(SERVO_OPEN);
	else
		servo_set_compare(SERVO_CLOSE);
}


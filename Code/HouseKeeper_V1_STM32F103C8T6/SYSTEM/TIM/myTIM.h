#ifndef __MYTIM_H_
#define __MYTIM_H_
#include <stm32f10x.h>

extern u8 servo_Stauts;

void TIM2_Int_Init(u16 arr,u16 psc);

void TIM2_IRQHandler(void);

void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);


#endif



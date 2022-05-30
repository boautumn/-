#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern short temp;					//ÎÂ¶È	
extern float pitch,roll,yaw; 		//Å·À­½Ç
extern u8 data[9];
extern u16 balance_pitch,balance_roll,balance_yaw;

void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);


#endif



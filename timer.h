#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx_tim.h"

#define TIM2_PRESCALER_VAL		42000 /* to divide it to 2khz */


void Tim2Init(unsigned short int period);
void Tim2Start();
void Tim2Stop();
void TIMER3_Configuration(int duty);
void Tim4Init(unsigned short int period);
void Tim4Start();
void Tim4Stop();


#endif

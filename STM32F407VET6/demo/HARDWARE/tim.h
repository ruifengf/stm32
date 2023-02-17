#ifndef TIM_H
#define TIM_H

#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "sys.h"
#include "misc.h"

#define TIMER3_CLOCK  10000
#define TIMER3_DIVSION  8400


void Tim3_Init(void);
void Pwm_Duty(uint32_t duty);
void Pwm_Freq(uint32_t freq);

#endif

#ifndef DELAY_H
#define DELAY_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "sys.h"




void Delay_Init(void);
void Delay_ms(uint16_t time);
void Delay_us(uint32_t time);
void Systick_init(void);


#endif

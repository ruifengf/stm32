#ifndef ULTRASOUND_H
#define ULTRASOUND_H

#include "stm32f4xx_gpio.h"
#include "sys.h"
#include "stm32f4xx_rcc.h"
#include "delay.h"
#include <stdio.h>


#define ULSOUND_PORT   GPIOE
#define TRIG_PIN  GPIO_Pin_2
#define ECHO_PIN  GPIO_Pin_3

#define TRIG_PIN_OUT PEout(2)
#define ECHO_PIN_IN PEin(3)

void UlSound_Init(void);
u32 UlSound_Ranging(void);



#endif

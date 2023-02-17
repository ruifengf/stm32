#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "sys.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"

#define BUTTON_PORT   GPIOE
#define BUTTON1_IN     PEin(3)
#define BUTTON2_IN     PEin(4)
#define BUTTON_PIN_1  GPIO_Pin_4
#define BUTTON_PIN_2  GPIO_Pin_3



void Button_Init(void);

#endif

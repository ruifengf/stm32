#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "sys.h"
#include "stm32f4xx_rcc.h"

#define LED_PORT   GPIOA
#define LED_PIN_1  GPIO_Pin_6
#define LED_PIN_2  GPIO_Pin_7
#define TEST_PIN  GPIO_Pin_5

#define LED_D2   PAout(6)
#define LED_D3   PAout(7)


void LED_Init(void);

#endif

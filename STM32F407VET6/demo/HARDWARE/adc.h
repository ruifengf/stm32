#ifndef ADC_H
#define ADC_H

#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"
#include "sys.h"
#include "type.h"

#define ADC_PIN_1  GPIO_Pin_5
#define ADC_PORT   GPIOA
#define DAC_PIN    GPIO_Pin_4
#define SAMPLE_TIMES  12

static U16 Get_Adc(void);
void Adc_Init(void);
void Dac_Init(void);
#endif

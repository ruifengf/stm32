#ifndef  DHT_11_H
#define  DHT_11_H

#include "stm32f4xx_gpio.h"
#include "sys.h"
#include "stm32f4xx_rcc.h"
#include "delay.h"
#include <stdio.h>

void Dht11_Init(void);
int Dht11_Start(void);
int32_t Dht11_Read_Byte(uint8_t *pbuf);
int32_t Dht11_Read_Data(uint8_t *pbuf);


/* PORT and PIN configuration */
#define  DHT11_PIN_1   GPIO_Pin_0
#define  DHT11_OUT_PIN PEout(0)
#define  DHT11_IN_PIN  PEin(0)
#define  DHT11_PORT    GPIOE

#define  PIN_HIGH   1
#define  PIN_LOW    0






/* DTH11 sequential control configuration */
#define  WAIT_TIME       500U
#define  RESPON_TIME     10U
#define  WAITING_TIME    100U
#define  START_TIME      10U
#define  BIT_TIME        4U
#define  BIT_LOOP        8U 
#define  BYTE_LOOP       5U





#endif

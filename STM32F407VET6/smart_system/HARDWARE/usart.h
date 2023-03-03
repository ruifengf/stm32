#ifndef USART_H
#define USART_H

#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "sys.h"
#include "type.h"
#define TX1_PIN GPIO_Pin_9
#define RX1_PIN GPIO_Pin_10
#define USART1_PORT GPIOA

#define TX2_PIN GPIO_Pin_5
#define RX2_PIN GPIO_Pin_6
#define USART2_PORT GPIOD

#define USART2_BAUDRATE 9600
#define USART1_BAUDRATE 115200

void Usart1_Init(u32 BaudRate);
void Usart2_Init(u32 BaudRate);
void Send_Str(const char* str);
void Usart1_SendString(unsigned char *str, unsigned short len);



U8 Get_Send_Count(void);

void Set_Send_Count(void);
#endif

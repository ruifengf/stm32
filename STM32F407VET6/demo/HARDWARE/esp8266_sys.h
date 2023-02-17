#ifndef _ESP8266_SYS_H_
#define _ESP8266_SYS_H_

#include "sys.h"



#define SYS_REV_OK		0	//接收完成标志
#define SYS_REV_WAIT	1	//接收未完成标志


void ESP8266_Sys_Init(void);
void Usart3_Init(unsigned int baud);
void ESP8266_Sys_Clear(void);

void ESP8266_Sys_SendData(unsigned char *data, unsigned short len);
void Usart3_SendString(unsigned char *str, unsigned short len);

#endif


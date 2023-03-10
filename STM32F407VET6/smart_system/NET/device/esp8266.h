#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "sys.h"




#define REV_OK		0	
#define REV_WAIT	1	
#define ESP_RST_PIN GPIO_Pin_4

#define ESP_RST_VOL  PDout(4)


void ESP8266_Init(void);
void ESP8266_Clear(void);

U8 ESP8266_SendCmd(char *cmd, char *res, u16 time, OS_ERR *p_err);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void Usart2_SendString(unsigned char *str, unsigned short len);

#endif

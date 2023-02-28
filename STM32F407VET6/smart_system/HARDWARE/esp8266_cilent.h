#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "sys.h"
#include "includes.h"
#include "type.h"


#define REV_OK		0	
#define REV_WAIT	1	
#define ESP_PORT   GPIOD
#define ESP_RST_PIN GPIO_Pin_2
#define ESP_RST_VOL  PDout(2)
#define ESP_BUFF_SIZE 128

#define SEND_COMPLETE 1
#define SEND_UNCOMPLETE 0



void ESP8266_Init(void);
void ESP8266_Clear(void);

U8 Send_Complete(void);
U8 ESP8266_SendCmd(char *cmd, char *res, u16 time, OS_ERR *p_err);

void ESP8266_SendData(unsigned char *data, unsigned short len);

U8 *ESP8266_GetIPD(unsigned short timeOut);
void Usart2_SendString(unsigned char *str, unsigned short len);
void WIFI_CWJAP(OS_ERR *p_err);


#endif

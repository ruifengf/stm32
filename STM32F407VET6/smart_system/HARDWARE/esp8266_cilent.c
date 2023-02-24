#include "stm32f4xx.h"
#include "esp8266.h"

#include "delay.h"
#include "usart.h"
#include "led.h"

#include <string.h>
#include <stdio.h>

#define ESP8266_SYS_WIFI_INFO		"AT+CWJAP=\"kk\",\"123abc\"\r\n"
//#define ESP8266_WIFI_INFO		"AT+CWJAP=\"Redmi_K40_Pro\",\"ya.shi.le\"\r\n"
#define ESP8266_SYS_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"

unsigned char esp8266_sys_buf[128];
unsigned short esp8266_sys_cnt = 0, esp8266_sys_cntPre = 0;


/*
**********************************
*                                *
*                                *
*                                *
**********************************
*/
void ESP8266__Sys__Clear(void)
{

	memset(esp8266_sys_buf, 0, sizeof(esp8266_sys_buf));
	esp8266_sys_cnt = 0;
}

//==========================================================
//
//==========================================================
_Bool ESP8266_Sys_WaitRecive(void)
{

	if(esp8266_sys_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中??所以直接跳出??结束函数
		return REV_WAIT;
		
	if(esp8266_sys_cnt == esp8266_sys_cntPre)				//如果上一次的值和这次?嗤???则说明接收完毕
	{
		esp8266_sys_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_sys_cntPre = esp8266_sys_cnt;					//置为?嗤?
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	
//==========================================================
_Bool ESP8266_Sys_SendCmd(char *cmd, char *res, u16 time)
{	
	Usart1_SendString((unsigned char *)cmd, strlen((const char *)cmd));

	while(time--)
	{
		if(ESP8266_Sys_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_sys_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Cli_Clear();									//清空缓存
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	
//==========================================================
void ESP8266_Sys_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Sys_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_Sys_SendCmd(cmdBuf, ">", 200))				//收到‘>’时可以发送数据
	{
		Usart3_SendString(data, len);		//发送设备连接请求数据
	}

}

void WIFI_CWJAP(void)
{
	
	while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK", 200))
		delay_ms(500);	
	
	printf("CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK", 200))
		delay_ms(500);
	
	printf("AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK", 200))
		delay_ms(500);
	
	printf("CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP", 200))
		delay_ms(500);
	AT+CIPMUX=1
	AT+CIPSERVER=1
	
	printf("CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT", 200))
		delay_ms(500);
	
	printf("ESP8266 Init OK\r\n");
}


//==========================================================
//	
//==========================================================
void ESP8266_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	//ESP8266复位引脚
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_3;					//GPIOA1-复位
	GPIO_Initure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
	delay_ms(250);
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
	delay_ms(500);
	ESP8266_Clear();

	printf("AT\r\n");
	while(ESP8266_SendCmd("AT\r\n\r", "OK", 400))
		delay_ms(500);
}





//==========================================================
//		
//==========================================================
void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断
	{
		if(esp8266_sys_cnt >= sizeof(esp8266_buf))	esp8266_sys_cnt = 0; //防止串口被刷爆
		esp8266_sys_buf[esp8266_sys_cnt++] = USART3->DR;
		
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}

} 

/*
************************************************************
*	函数名称??	Usart_SendString
*
*	函数功能??	串口数据发送
*
*	入口参数??	USARTx?捍?口组
*				str?阂?发送的数据
*				len?菏?据长度
*
*	返回参数??	无
*
*	说明??		
************************************************************
*/
void Usart1_SendString(unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USART1, *str++);									//发送数据
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		//等待发送完成
	}
}





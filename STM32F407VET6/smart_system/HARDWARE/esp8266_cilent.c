#include "stm32f4xx.h"
#include "esp8266_cilent.h"

#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

#include <string.h>
#include <stdio.h>

#define ESP8266_WIFI_INFO_1		"AT+CWJAP=\"kk\",\"frf1234567\"\r\n"
#define ESP8266_WIFI_INFO_2		"AT+CWJAP=\"kun5g\",\"66666666\"\r\n"
#define ESP8266_WIFI_INFO_3		"AT+CWJAP=\"Xiaomi_D6DB209\",\"98765mmm\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"
#define ESP8266_MODE            "AT+CWMODE=1\r\n"
#define ESP8266_DHCP            "AT+CWDHCP=1,1\r\n"
extern char esp8266_buf[128] = {0};
extern OS_Q    g_queue_usart2;

extern volatile char usart2_data[128];

/*
**********************************
*                                *
*                                *
*                                *
**********************************
*/
void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
}


U8 Send_Complete(void)
{
	static U8 send_count_new; 
	U8 send_count_old;
	
	//与usart2共享send_count，需要关中断进行保护
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	send_count_old = Get_Send_Count();
	CPU_CRITICAL_EXIT();
	if(send_count_old == 0)
	{
		return SEND_UNCOMPLETE;
	}
	else if(send_count_new != send_count_old)
	{
		send_count_new = send_count_old;
		return SEND_UNCOMPLETE;
	}
	else
	{
		send_count_new = 0;
		CPU_CRITICAL_ENTER();
		Set_Send_Count();
		CPU_CRITICAL_EXIT();
		return SEND_COMPLETE;
	}

}

//==========================================================
//	
//==========================================================
U8 ESP8266_SendCmd(char *cmd, char *res, u16 time, OS_ERR *p_err)
{	
	//U16 msg_size;
	//U8 loop = 0;
	//U8* data = NULL;

	Usart2_SendString((unsigned char *)cmd, strlen((const char *)cmd));
	while(time--)
	{
		//data = OSQPend(&g_queue_usart2,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,p_err);
		//Debug_Printf("%s\r\n",data);
		//while(loop < msg_size)
		//{
			//esp8266_buf[loop] = data[loop];
			//loop++;
		
		
		//}
		Delay_ms(1);
		if(SEND_COMPLETE == Send_Complete())
		{
			Debug_Printf("ack:%s\r\n", (const char *)esp8266_buf);
			OSIntExit();

			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}		
		}
	}
	
	return 1;

}


//==========================================================
//	
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{
	OS_ERR err;
	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">", 200, &err))				//收到‘>’时可以发送数据
	{
		Usart2_SendString(data, len);		//发送设备连接请求数据
	}

}

void WIFI_CWJAP(OS_ERR *p_err)
{	

	while(ESP8266_SendCmd(ESP8266_MODE, "OK", 200, p_err))
	Delay_ms(500);
	Debug_Printf("成功配置ESP8266为客户端模式\r\n");
	
	while(ESP8266_SendCmd(ESP8266_DHCP, "OK", 200, p_err))
	Delay_ms(500);
	Debug_Printf("成功配置为动态获取IP地址\r\n");
	
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO_3, "GOT IP", 200, p_err))
	Delay_ms(500);
	Debug_Printf("成功连接WIFI热点\r\n");
	
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT", 200, p_err))
	Delay_ms(500);
	Debug_Printf("成功连接ONENET平台\r\n");
	Debug_Printf("ESP8266初始化完毕\r\n");
		
}


//==========================================================
//	
//==========================================================
void ESP8266_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = ESP_RST_PIN;					
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	
	GPIO_Init(ESP_PORT, &GPIO_InitStructure);
	
	ESP_RST_VOL = 0;
	Delay_ms(500);
	ESP_RST_VOL = 1;
	Delay_ms(500);
	ESP8266_Clear();

}











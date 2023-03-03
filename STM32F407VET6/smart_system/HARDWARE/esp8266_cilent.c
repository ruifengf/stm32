#include "stm32f4xx.h"
#include "esp8266_cilent.h"

#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

#include <string.h>
#include <stdio.h>

//#define ESP8266_WIFI_INFO_1		"AT+CWJAP=\"kk\",\"frf1234567\"\r\n"
//#define ESP8266_WIFI_INFO_2		"AT+CWJAP=\"kun5g\",\"66666666\"\r\n"
#define ESP8266_WIFI_INFO_3		"AT+CWJAP=\"Xiaomi_D6DB209\",\"98765mmm\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"
//#define ESP8266_NATIVE_INFO		"AT+CIPSTART=\"TCP\",\"192.168.31.78\",8888\r\n"

#define ESP8266_MODE            "AT+CWMODE=1\r\n"
#define ESP8266_DHCP            "AT+CWDHCP=1,1\r\n"
#define ESP8266_IPCHECK         "AT+CIFSR\r\n"

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

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
U8 *ESP8266_GetIPD(unsigned short timeOut)
{
	char *ptrIPD = NULL;	
	
	while(timeOut--)
	{
		if(SEND_COMPLETE == Send_Complete())								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//Debug_Printf("ptrIPD not found\r\n");
			}
			else
			{
				//Debug_Printf("ptrIPD found\r\n");
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
				{

					return NULL;
				}									
			}
		}
		
		Delay_ms(10);													//延时等待
	} 
	//
	return NULL;														//超时还未找到，返回空指针
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
		//Debug_Printf("zero\r\n");
		return SEND_UNCOMPLETE;
	}
	else if(send_count_new != send_count_old)
	{
		send_count_new = send_count_old;
		//Debug_Printf("%d\r\n", send_count_old);
		return SEND_UNCOMPLETE;
	}
	else
	{
		send_count_new = 0;
		CPU_CRITICAL_ENTER();
		Set_Send_Count();
		CPU_CRITICAL_EXIT();
		Debug_Printf("finsh\r\n");
		return SEND_COMPLETE;
	}

}

//==========================================================
//	
//==========================================================
U8 ESP8266_SendCmd(char *cmd, char *res, u16 time)
{	

	Usart2_SendString(cmd, strlen((const char *)cmd));
	while(time--)
	{
		Delay_ms(1);
		if(SEND_COMPLETE == Send_Complete())
		{
			//Debug_Printf("ack:%s\r\n", (const char *)esp8266_buf);
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
	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">", 200))				//收到‘>’时可以发送数据
	{
		Usart2_SendString(data, len);		//发送设备连接请求数据
	}

}

void WIFI_CWJAP(void)
{	
	//配置ESP8266为客户端模式
	while(ESP8266_SendCmd(ESP8266_MODE, "OK", 200))
	Delay_ms(500);
	Debug_Printf("成功配置ESP8266为客户端模式\r\n");
	
	//配置为动态获取IP地址
	while(ESP8266_SendCmd(ESP8266_DHCP, "OK", 200))
	Delay_ms(500);
	Debug_Printf("成功配置为动态获取IP地址\r\n");
	
	//连接对应的WIFI热点
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO_3, "GOT IP", 200))
	Delay_ms(500);
	Debug_Printf("成功连接WIFI热点\r\n");
	
	//建立TCP连接，连接ONENET服务器
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT", 200))
	Delay_ms(500);
	Debug_Printf("成功连接ONENET服务器\r\n");
	
	//while(ESP8266_SendCmd(ESP8266_IPCHECK, "OK", 200))
	//Delay_ms(500);
	//Debug_Printf("成功获取IP地址\r\n");
	
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
	
	
	//测试指令
	while(ESP8266_SendCmd("AT\r\n", "OK", 400))

		
	
	Delay_ms(500);

	WIFI_CWJAP();
	while(OneNet_DevLink())			//接入OneNET
	Delay_ms(500);

}











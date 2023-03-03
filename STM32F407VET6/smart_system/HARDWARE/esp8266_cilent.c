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
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
U8 *ESP8266_GetIPD(unsigned short timeOut)
{
	char *ptrIPD = NULL;	
	
	while(timeOut--)
	{
		if(SEND_COMPLETE == Send_Complete())								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//Debug_Printf("ptrIPD not found\r\n");
			}
			else
			{
				//Debug_Printf("ptrIPD found\r\n");
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
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
		
		Delay_ms(10);													//��ʱ�ȴ�
	} 
	//
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��
}


U8 Send_Complete(void)
{
	static U8 send_count_new; 
	U8 send_count_old;
	
	//��usart2����send_count����Ҫ���жϽ��б���
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

			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
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
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">", 200))				//�յ���>��ʱ���Է�������
	{
		Usart2_SendString(data, len);		//�����豸������������
	}

}

void WIFI_CWJAP(void)
{	
	//����ESP8266Ϊ�ͻ���ģʽ
	while(ESP8266_SendCmd(ESP8266_MODE, "OK", 200))
	Delay_ms(500);
	Debug_Printf("�ɹ�����ESP8266Ϊ�ͻ���ģʽ\r\n");
	
	//����Ϊ��̬��ȡIP��ַ
	while(ESP8266_SendCmd(ESP8266_DHCP, "OK", 200))
	Delay_ms(500);
	Debug_Printf("�ɹ�����Ϊ��̬��ȡIP��ַ\r\n");
	
	//���Ӷ�Ӧ��WIFI�ȵ�
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO_3, "GOT IP", 200))
	Delay_ms(500);
	Debug_Printf("�ɹ�����WIFI�ȵ�\r\n");
	
	//����TCP���ӣ�����ONENET������
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT", 200))
	Delay_ms(500);
	Debug_Printf("�ɹ�����ONENET������\r\n");
	
	//while(ESP8266_SendCmd(ESP8266_IPCHECK, "OK", 200))
	//Delay_ms(500);
	//Debug_Printf("�ɹ���ȡIP��ַ\r\n");
	
	Debug_Printf("ESP8266��ʼ�����\r\n");
		
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
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	
	GPIO_Init(ESP_PORT, &GPIO_InitStructure);
	
	ESP_RST_VOL = 0;
	Delay_ms(500);
	ESP_RST_VOL = 1;
	Delay_ms(500);
	ESP8266_Clear();
	
	
	//����ָ��
	while(ESP8266_SendCmd("AT\r\n", "OK", 400))

		
	
	Delay_ms(500);

	WIFI_CWJAP();
	while(OneNet_DevLink())			//����OneNET
	Delay_ms(500);

}











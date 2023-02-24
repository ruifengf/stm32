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

	if(esp8266_sys_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ���������??����ֱ������??��������
		return REV_WAIT;
		
	if(esp8266_sys_cnt == esp8266_sys_cntPre)				//�����һ�ε�ֵ�����?��???��˵���������
	{
		esp8266_sys_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_sys_cntPre = esp8266_sys_cnt;					//��Ϊ?��?
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	
//==========================================================
_Bool ESP8266_Sys_SendCmd(char *cmd, char *res, u16 time)
{	
	Usart1_SendString((unsigned char *)cmd, strlen((const char *)cmd));

	while(time--)
	{
		if(ESP8266_Sys_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_sys_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Cli_Clear();									//��ջ���
				
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
	
	ESP8266_Sys_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_Sys_SendCmd(cmdBuf, ">", 200))				//�յ���>��ʱ���Է�������
	{
		Usart3_SendString(data, len);		//�����豸������������
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

	//ESP8266��λ����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_3;					//GPIOA1-��λ
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

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_sys_cnt >= sizeof(esp8266_buf))	esp8266_sys_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_sys_buf[esp8266_sys_cnt++] = USART3->DR;
		
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}

} 

/*
************************************************************
*	��������??	Usart_SendString
*
*	��������??	�������ݷ���
*
*	��ڲ���??	USARTx?��?����
*				str?��?���͵�����
*				len?��?�ݳ���
*
*	���ز���??	��
*
*	˵��??		
************************************************************
*/
void Usart1_SendString(unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USART1, *str++);									//��������
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		//�ȴ��������
	}
}





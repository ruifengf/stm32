//��Ƭ��ͷ�ļ�
#include "stm32f4xx.h"

//�����豸����
#include "esp8266_sys.h"

//Ӳ������
#include "delay.h"
#include "usart.h"
#include "led.h"

//C��
#include <string.h>
#include <stdio.h>

#define ESP8266_SYS_WIFI_INFO		"AT+CWSAP=\"KKKK\",\"123abc\",2,0\r\n"     

unsigned char esp8266_sys_buf[128];
unsigned short esp8266_sys_cnt = 0, esp8266_sys_cntPre = 0;


//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Sys_Clear(void)
{

	memset(esp8266_sys_buf, 0, sizeof(esp8266_sys_buf));
	esp8266_sys_cnt = 0;
}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_Sys_WaitRecive(void)
{

	if(esp8266_sys_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return SYS_REV_WAIT;
		
	if(esp8266_sys_cnt == esp8266_sys_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_sys_cnt = 0;							//��0���ռ���
			
		return SYS_REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_sys_cntPre = esp8266_sys_cnt;					//��Ϊ��ͬ
	
	return SYS_REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_Sys_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
static _Bool ESP8266_Sys_SendCmd(char *cmd, char *res, u16 time)
{	
	printf("start\r\n");
	Usart3_SendString((unsigned char *)cmd, strlen((const char *)cmd));
	printf("send\r\n");
	while(time--)
	{
		if(ESP8266_Sys_WaitRecive() == SYS_REV_OK)							//����յ�����
		{
			printf("test\r\n");
			if(strstr((const char *)esp8266_sys_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Sys_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
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



//WIFIд���ڳ�����
//ONENET�ٷ������õķ�ʽ
static void WIFIS_Sys_CWJAP(void)
{
	printf("AT+CWMODE_SYS\r\n");      //����Ϊ������ģʽ
	while(ESP8266_Sys_SendCmd("AT+CWMODE=2\r\n", "OK", 200))
		delay_ms(500);
	
	printf("AT+CWSAP_SYS\r\n");       //����AP���ơ����롢ͨ���š����ܷ�ʽ
	while(ESP8266_Sys_SendCmd(ESP8266_SYS_WIFI_INFO, "OK", 200))
		delay_ms(500);
	
	printf("AT+RST_SYS\r\n");         //����
	while(ESP8266_Sys_SendCmd("AT+RST\r\n", "ready", 200))
		delay_ms(500);
	
	printf("AT+CIPMUX_SYS\r\n");      //��������ģʽΪ������ģʽ
	while(ESP8266_Sys_SendCmd("AT+CIPMUX=0\r\n", "OK", 200))
		delay_ms(500);
	
	printf("AT+CIPSERVER_SYS\r\n");     //���������������ö˿ں�
	while(ESP8266_Sys_SendCmd("AT+CIPSERVER=1,8888\r\n", "OK", 200))
		delay_ms(500);
	
	printf("ESP8266_Sys Init OK\r\n");
}


//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Sys_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//ESP8266��λ����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_1;					//GPIOA1-��λ
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
	delay_ms(250);
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
	delay_ms(500);
	ESP8266_Sys_Clear();

	printf("ATT\r\n");
	while(ESP8266_Sys_SendCmd("AT\r\n\r", "OK", 400))
		delay_ms(500);
	printf("AT\r\n");
	WIFIS_Sys_CWJAP();
}


/*
************************************************************
*	�������ƣ�	Usart3_Init
*
*	�������ܣ�	����3��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PB10		RX-PB11
************************************************************
*/
void Usart3_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpio_initstruct;
	USART_InitTypeDef usart_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	USART_DeInit(USART3); 
	//PB10	TXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_initstruct);
	
	//PB11	RXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_11;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_initstruct);
	
	usart_initstruct.USART_BaudRate = baud;
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usart_initstruct.USART_Parity = USART_Parity_No;									//��У��
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART3, &usart_initstruct);
	
	USART_Cmd(USART3, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvic_initstruct.NVIC_IRQChannel = USART3_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvic_initstruct);

}


//==========================================================
//	�������ƣ�	USART3_IRQHandler
//
//	�������ܣ�	����3�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_sys_cnt >= sizeof(esp8266_sys_buf))	esp8266_sys_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_sys_buf[esp8266_sys_cnt++] = USART3->DR;
		
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}

}


/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart3_SendString(unsigned char *str, unsigned short len)
{
	printf("sta\r\n");
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		
		USART_SendData(USART3, *str++);									//��������
		printf("count = %d\r\n", count);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		//�ȴ��������
	}
	printf("end\r\n");
}





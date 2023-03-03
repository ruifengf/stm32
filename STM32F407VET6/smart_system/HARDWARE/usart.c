#include <stdio.h>
#include "usart.h"
#include "led.h"
#include "includes.h"

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
    
}
void _sys_exit(int return_code) {
label: goto label; /* endless loop */
}
U8 data_index = 0;
char alarm_data[30];
U8 alarm_set_event;
extern OS_Q    g_queue_usart2;
static volatile U8 usart2_data_index = 0;
static volatile char usart2_data[128];
extern unsigned char esp8266_buf[128];

extern OS_Q    g_queue_usart1;
static volatile U8 usart1_data_index = 0;
static volatile char usart1_data[30];

extern OS_MUTEX  g_printf_mutex;
static U8 usart2_complete = 0;


U8 Get_Send_Count(void)
{
	return usart2_data_index;
}

void Set_Send_Count(void)
{
	usart2_data_index = 0;
}


void Usart1_Init(u32 BaudRate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


    GPIO_InitStructure.GPIO_Pin = TX1_PIN | RX1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(USART1_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig(USART1_PORT, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(USART1_PORT, GPIO_PinSource10, GPIO_AF_USART1);

    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
    USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Usart2_Init(u32 BaudRate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);


    GPIO_InitStructure.GPIO_Pin = TX2_PIN | RX2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(USART2_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig(USART2_PORT, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(USART2_PORT, GPIO_PinSource6, GPIO_AF_USART2);

    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
    USART_Init(USART2, &USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


    

void USART1_IRQHandler(void)
{   
	uint8_t usart1_complete = 0;
	OS_ERR       err;
	//进入中断
	OSIntEnter(); 
    if (SET == USART_GetITStatus(USART1, USART_IT_RXNE))
    {
		usart1_data[usart1_data_index] = USART_ReceiveData(USART1);
        if('#' == usart1_data[usart1_data_index] || 30 == usart1_data_index)
        {
            usart1_complete = 1;            
        }
        else
        {
            usart1_data_index++;
        }
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
		// USART_SendData(USART2, data);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
	if(usart1_complete)
	{
		
		OSQPost(&g_queue_usart1, (void*)&usart1_data[0], usart1_data_index, OS_OPT_POST_FIFO, &err);
		usart1_complete = 0;
		usart1_data_index = 0;
	}
	OSIntExit();


}    
void USART2_IRQHandler(void)
{  
	OS_ERR       err;
	OSIntEnter();	
    if (SET == USART_GetITStatus(USART2, USART_IT_RXNE))
    {
		esp8266_buf[usart2_data_index] = USART_ReceiveData(USART2);
		Debug_Printf("%c", esp8266_buf[usart2_data_index]);
        if(127 == usart2_data_index)
        {
            usart2_data_index = 0; 
        }
        else
        {
            usart2_data_index++;
        }
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
	//OSQPost(&g_queue_usart2, (void*)&usart2_data[0], usart2_data_index, OS_OPT_POST_FIFO, &err);
	OSIntExit();

}

void Send_Str(const char* str)
{
	const char *p  = str;
    while(*p != '\0')
	{		
		while( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
        USART_SendData(USART1, *p);			
		p++;
	    USART_ClearFlag(USART2,USART_FLAG_TC);
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
void Usart2_SendString(unsigned char *str, unsigned short len)
{	
	while(len != 0)
	{	
       // USART2->SR &= (uint16_t)~USART_FLAG_TC; //对发送标志位进行清零		
		USART_SendData(USART2, *str);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		str++;
		len--;		
	}

}

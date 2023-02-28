#include "led.h"
#include "button.h"
#include "tim.h"
#include "usart.h"
#include "ultrasound.h"
#include "blue_tooth.h"
#include "dht11.h"
#include "rtc.h"
#include "flash.h"
#include "adc.h"
#include "includes.h"
#include "delay.h"
#include "oled.h"
#include "bmp.h"
#include "esp8266_cilent.h"

//Interrupt Flag Group
OS_FLAG_GRP  g_flag_group;

//Semaphore
OS_SEM  g_dht11_sem;

//Message Queue
OS_Q    g_queue_usart2;
OS_Q    g_queue_usart1;
OS_Q    g_queue_dht11;

//Mutex Semaphore
OS_MUTEX  g_printf_mutex;

//OS Timer
OS_TMR  g_timer;

//Task Init
OS_TCB task_init_tcb;
void Task_Init(void *parg);
CPU_STK task_init_stk[128];			

//Debug Task
OS_TCB task_debug_tcb;
void Task_Debug(void *parg);
CPU_STK task_debug_stk[512];	

//Dht11 Task
OS_TCB task_dht11_tcb;				
void Task_Dht11(void *parg);			
CPU_STK task_dht11_stk[128];

//Button Task
OS_TCB task_button_tcb;				
void Task_Button(void *parg);			
CPU_STK task_button_stk[128];

//W25Q16 Task
OS_TCB task_w25q16_tcb;				
void Task_W25Q16(void *parg);			
CPU_STK task_w25q26_stk[256];

//ESP8266 Task
OS_TCB task_esp8266_tcb;				
void Task_ESP8266(void *parg);			
CPU_STK task_esp8266_stk[256];

extern unsigned char esp8266_buf[128];

//Task_Timer
void Task_Timer(void *p_tmr, void *p_arg);
 
 
 
 
int main(void)
{
	OS_ERR err;

	Systick_init();  
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
	Usart1_Init(115200);
	Usart2_Init(115200);	

	OSInit(&err);
	
	OSTaskCreate(	&task_init_tcb,									
					"Task_Init",									
					Task_Init,										
					0,											
					7,											
					task_init_stk,									
					128/10,										
					128,										
					0,											
					0,																			
					0,											
					OS_OPT_TASK_NONE,							
					&err										
				);

	OSStart(&err);
					
	while(1);
}


void Debug_Printf(const char *format, ...)
{
#if DEBUG_PRINTF_EN	
	OS_ERR err;
	
	va_list args;
	va_start(args, format);
	
	OSMutexPend(&g_printf_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&err);	
	vprintf(format, args);
	OSMutexPost(&g_printf_mutex,OS_OPT_POST_NONE,&err);
	
	va_end(args);
#else
	(void)0;
#endif
}





void Task_Init(void *parg)
{
	OS_ERR err;
	DEBUG("Task_Init is create ok\r\n");

	//Hardware Init
	LED_Init();         												
	Button_Init();
	Dht11_Init();
	W25Q16_Init();
	OLED_Init();
	
	//Semaphore Create
	OSSemCreate(&g_dht11_sem, "dht11_finish", 0, &err);
	
	//Mutex Semphore Create
	OSMutexCreate(&g_printf_mutex, "print_mutex", &err);
	
	//Message Queue Create
	OSQCreate(&g_queue_usart2, "usart_queue_2", 1, &err);
	OSQCreate(&g_queue_usart1, "usart_queue_1", 1, &err);
	OSQCreate(&g_queue_dht11, "dht11_queue", 10, &err);
	
	//Interrupt Flag Group Create
	OSFlagCreate(&g_flag_group, "g_flag_group", 0, &err);
	
	//OS Timer Create
	OSTmrCreate(&g_timer,"timer", 10, 100, OS_OPT_TMR_PERIODIC, Task_Timer ,NULL ,&err);
	//Creat Task to run 
	//Debug Task Create
	OSTaskCreate(	&task_debug_tcb,								
					"Task_Debug",								
					Task_Debug,									
					0,										
					4,										
					task_debug_stk,								
					512/10,									
					512,									
					0,										
					0,																					
					0,										
					OS_OPT_TASK_NONE,						
					&err									
				);                                          
	
	//Dht11 Task Create
	OSTaskCreate(	&task_dht11_tcb,								
					"Task_Dht11",							
					Task_Dht11,								
					0,										
					6,										
					task_dht11_stk,								
					128/10,									
					128,									
					0,										
					0,																					
					0,										
					OS_OPT_TASK_NONE,						
					&err									
				);
				
	//Button Task Create			
	OSTaskCreate(	&task_button_tcb,							
					"Task_Button",							
					Task_Button,							
					0,										
					6,										
					task_button_stk,								
					128/10,									
					128,									
					0,										
					0,																					
					0,										
					OS_OPT_TASK_NONE,						
					&err									
				);
				
	//W25Q16 Task Create			
	OSTaskCreate(	&task_w25q16_tcb,							
					"Task_W25Q16",							
					Task_W25Q16,							
					0,										
					5,										
					task_w25q26_stk,								
					256/10,									
					256,									
					0,										
					0,																					
					0,										
					OS_OPT_TASK_NONE,						
					&err									
				);
	//ESP8266 Task Create			
	OSTaskCreate(	&task_esp8266_tcb,							
					"Task_ESP8266",							
					Task_ESP8266,							
					0,										
					4,										
					task_esp8266_stk,								
					256/10,									
					256,									
					0,										
					0,																					
					0,										
					OS_OPT_TASK_NONE,						
					&err									
				);			
	//OS Timer Start																													
	OSTmrStart(&g_timer, &err);

		
	//Task Delete
	OSTaskDel(NULL,&err);
	
}

void Task_Debug(void *parg)
{
	Debug_Printf("Task_Debug is create ok\r\n");
	while(1)
	{
		OLED_Clear();
		Delay_ms(1000);
		OLED_ShowCHinese(0,0,0);//中
		OLED_ShowCHinese(18,0,1);//景
		OLED_ShowCHinese(36,0,2);//园
		OLED_ShowCHinese(54,0,3);//电
		OLED_ShowCHinese(72,0,4);//子
		OLED_ShowCHinese(90,0,5);//科
		OLED_ShowCHinese(108,0,6);//技
		Delay_ms(1000);
		OLED_DrawBMP(0,0,128,8,&BMP2[0]);
		Delay_ms(1000);
		 
	}
}


void Task_Dht11(void *parg)
{
	OS_ERR err;	
	uint8_t buf[5] = {0};
	int32_t rt=0;
	DEBUG("Task_Dht11 is create ok\r\n");
	while(1)
	{		
		memset(buf,0,sizeof(buf));	
		
		rt = Dht11_Read_Data(buf);
		if(rt == 0)
		{
			OSQPost(&g_queue_dht11, (void*)buf, 5, OS_OPT_POST_FIFO, &err);
			OSSemPost(&g_dht11_sem,OS_OPT_POST_1, &err);
		}
		else
		{
			//Debug_Printf("dht11 read error code %d\r\n",rt);		
		}
		OSTimeDly(2000,OS_OPT_TIME_PERIODIC,&err);
	}
}

void Task_Button(void *parg)
{
	OS_ERR err;	
	OS_FLAGS flags;
	while(1)
	{
		flags = OSFlagPend (&g_flag_group, FLAG_FROUP_BUTTON1_DOWN|FLAG_FROUP_BUTTON2_DOWN, 
							0, 
							OS_OPT_PEND_BLOCKING+OS_OPT_PEND_FLAG_SET_ANY+ OS_OPT_PEND_FLAG_CONSUME,  
							NULL, 
							&err);
		if(flags & FLAG_FROUP_BUTTON1_DOWN)
		{
			NVIC_DisableIRQ(EXTI3_IRQn);
			if(BUTTON2_IN == 0)
			{
				Debug_Printf("button 1 setdown\r\n");
				Delay_ms(10);
				if(BUTTON1_IN == 0)
				{
					Debug_Printf("button 1 setdown\r\n");
				}


			}
			NVIC_EnableIRQ(EXTI3_IRQn);
			
		}
		else if(flags & FLAG_FROUP_BUTTON2_DOWN)
		{
			Debug_Printf("button 2 setdown\r\n");
		}
		else
		{
			
		}

	
	}



}

void Task_W25Q16(void *parg)
{
	U8 data_1[256];
	U8 data_2[256];
	U16 index;
	Debug_Printf("Task_W25Q16 is create ok\r\n");
	while(1)
	{
		
		for(index = 0; index < 10; index++)
		{
			data_1[index] = index;		
		}
		W25Q16_Erase_Sector(0x000000);
		W25Q16_Write_Data(0x000000, 10, &data_1[0]);
		W25Q16_Read_Data(0x000000, 10, &data_2[0]);
		
		for(index = 0; index < 10; index++)
		{
			//Debug_Printf("new_data[%d]: %d\r\n", index, data_2[index]);
		}
		Delay_ms(1000);
	
	}
	


}






void Task_ESP8266(void *parg)
{
	OS_ERR err;
	U8 return_symbol;
		
	char *onenet_data = "AT\r\n";
	
	Debug_Printf("Task_ESP8266 is create ok\r\n");
	ESP8266_Init();

	return_symbol = 1;
	while(1 == return_symbol)
	{
		return_symbol = ESP8266_SendCmd("AT\r\n", "OK", 400, &err);
	}
		
	
	Delay_ms(500);

	WIFI_CWJAP(&err);

	while(1)
	{	

	}

		
}



void Task_Timer(void *p_tmr, void *p_arg)
{
	//Debug_Printf("timer callback\r\n");
}


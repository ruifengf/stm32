#include "delay.h"
#include "includes.h"							//ucos using 

static uint16_t fac_us=0;						//usÑÓÊ±±¶³ËÊý			   
static uint16_t fac_ms=0;						//msÑÓÊ±±¶³ËÊý,ÔÚosÏÂ,´ú±íÃ¿¸ö½ÚÅÄµÄmsÊý
void Delay_Init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}
#if UCOS_USE
void Systick_init(void)
{

	uint32_t reload;

 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//SYSTICK using peripheral clock = 168/8= 21MHz
	fac_us=SystemCoreClock/8/1000000;					

	reload=SystemCoreClock/8/1000000;						   
	reload*=1000000/OSCfg_TickRate_Hz;					//¸ù¾ÝOSCfg_TickRate_HzÉè¶¨Òç³öÊ±¼ä
														//reloadÎª24Î»¼Ä´æÆ÷,×î´óÖµ:16777216,ÔÚ72MÏÂ,Ô¼ºÏ1.86s×óÓÒ	
	fac_ms=1000/OSCfg_TickRate_Hz;						//´ú±íOS¿ÉÒÔÑÓÊ±µÄ×îÉÙµ¥Î»	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			//¿ªÆôSYSTICKÖÐ¶Ï
	SysTick->LOAD=reload; 								//Ã¿1/OSCfg_TickRate_HzÃëÖÐ¶ÏÒ»´Î	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 			//¿ªÆôSYSTICK
}	

void Delay_ms(uint16_t nms)
{	
	OS_ERR err; 
	
	if(OSRunning&&OSIntNestingCtr==0)		
	{		 
		if(nms>=fac_ms)						
			OSTimeDly(nms/fac_ms,OS_OPT_TIME_PERIODIC,&err);
		
		nms%=fac_ms;						  
	}
	
	Delay_us((uint32_t)(nms*1000));			

}
void Delay_us(uint32_t nus)
{		
	OS_ERR err; 
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOADµÄÖµ	    	 
	ticks=nus*fac_us; 						//ÐèÒªµÄ½ÚÅÄÊý 
	
	OSSchedLock(&err);						//×èÖ¹OSµ÷¶È£¬·ÀÖ¹´ò¶ÏusÑÓÊ±
	
	told=SysTick->VAL;        				//¸Õ½øÈëÊ±µÄ¼ÆÊýÆ÷Öµ
	
	while(1)
	{
		tnow=SysTick->VAL;					//»ñÈ¡µ±Ç°¼ÆÊýÖµ
		
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;			//ÕâÀï×¢ÒâÒ»ÏÂSYSTICKÊÇÒ»¸öµÝ¼õµÄ¼ÆÊýÆ÷¾Í¿ÉÒÔÁË.
			else 
				tcnt+=reload-tnow+told;
			
			told=tnow;
			
			if(tcnt>=ticks)
				break;						//Ê±¼ä³¬¹ý/µÈÓÚÒªÑÓ³ÙµÄÊ±¼ä,ÔòÍË³ö.
		}  
	}
	
	OSSchedUnlock(&err);					//»Ö¸´OSµ÷¶È											    
} 

void SysTick_Handler(void)
{	
	if(OSRunning==OS_STATE_OS_RUNNING)			//OS¿ªÊ¼ÅÜÁË,²ÅÖ´ÐÐÕý³£µÄµ÷¶È´¦Àí
	{
		OSIntEnter();							//½øÈëÖÐ¶Ï
		OSTimeTick();       					//µ÷ÓÃucosµÄÊ±ÖÓ·þÎñ³ÌÐò               
		OSIntExit();       	 					//´¥·¢ÈÎÎñÇÐ»»ÈíÖÐ¶Ï
	}
}


#else
void Delay_ms(uint16_t time)
{
    while (time--)
    {
        SysTick->CTRL = 0;
        SysTick->LOAD = (168000)-1;
        SysTick->VAL = 0;
        SysTick->CTRL = 5;
        while (1)
        {
            if (SysTick->CTRL & 0x00010000)
            {
                break;
            }

            if ((SysTick->CTRL & 0x1) == 0)
            {
                return;
            }
             
        }
                
    }
    SysTick->CTRL = 0;
}

void Delay_us(uint32_t time)
{
    while (time--)
    {
        SysTick->CTRL = 0;
        SysTick->LOAD = (168)-1;
        SysTick->VAL = 0;
        SysTick->CTRL = 5;
        while (1)
        {
            if (SysTick->CTRL & 0x00010000)
            {
                break;
            }

            if ((SysTick->CTRL & 0x1) == 0)
            {
                return;
            }
             
        }
                
    }
    SysTick->CTRL = 0;
}




#endif
/*
void SysTick_Handler(void)
{
    static uint32_t counter = 0;
    counter++;

    if(counter > 500)
    {
        counter = 0;
        PAout(6) ^= 1;
    }
}
*/

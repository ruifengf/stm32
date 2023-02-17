#include "tim.h"


static u32 timer3_freq;
static u32 timer3_counter;

void Tim3_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    /* TIM3 configuration */
    TIM_TimeBaseStructure.TIM_Period = (TIMER3_CLOCK/100)-1;         //频率 
    TIM_TimeBaseStructure.TIM_Prescaler = (TIMER3_DIVSION - 1);         //分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    timer3_freq = 100;
    timer3_counter = TIMER3_CLOCK/100+1;


    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 80;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);


#if 0
    /* Enable the TIM3 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //使能定时器3中断方式：时间更新
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
#endif

    //使能定时器3工作
    TIM_Cmd(TIM3, ENABLE);

}

void Pwm_Duty(uint32_t duty)
{
    uint32_t cmp = 0;
    cmp = (timer3_counter + 1) * duty/100;
    TIM_SetCompare1(TIM3,cmp);
}

void Pwm_Freq(uint32_t freq)
{   
    /* TIM3 configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1;         //频率 
    TIM_TimeBaseStructure.TIM_Prescaler = (8400 - 1);         //分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    timer3_freq = freq;
    timer3_counter = TIMER3_CLOCK/freq + 1;
}

#if 0
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
       PAout(6) ^= 1;
    }
    else
    {
        
    }
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}
#endif

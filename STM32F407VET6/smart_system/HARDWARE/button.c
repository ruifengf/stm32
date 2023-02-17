#include "button.h"
#include "includes.h"


extern OS_FLAG_GRP  g_flag_group;

/**
 * @brief  Button Init
 * @param  none
 * @retval none  
 */
void Button_Init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能系统配置时钟

  //GPIOE3,E4初始化设置
  GPIO_InitStructure.GPIO_Pin = BUTTON_PIN_1 | BUTTON_PIN_2;//按键1和按键2对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);//初始化GPIO
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  //连接外部中断
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);

  EXTI_InitStructure.EXTI_Line = EXTI_Line3|EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void EXTI3_IRQHandler(void)
{
    OS_ERR err;
	OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        OSFlagPost(&g_flag_group, 0x01,OS_OPT_POST_FLAG_SET, &err);
    }
    else
    {
        
    }
	EXTI_ClearITPendingBit(EXTI_Line3);
	OSIntExit();
}

void EXTI4_IRQHandler(void)
{
    OS_ERR err;
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        OSFlagPost(&g_flag_group, 0x02,OS_OPT_POST_FLAG_SET, &err);
    }
    else
    {
        
    }
	EXTI_ClearITPendingBit(EXTI_Line4);
    OSIntExit();
}

#include "ultrasound.h"

/**
 * @brief  LED Init
 * @param  none
 * @retval none  
 */
void UlSound_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA时钟

  //GPIOE4初始化设置
  GPIO_InitStructure.GPIO_Pin = TRIG_PIN;//触发引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(ULSOUND_PORT, &GPIO_InitStructure);//初始化GPIO
  TRIG_PIN_OUT = 0;

  //GPIOE3初始化设置
  GPIO_InitStructure.GPIO_Pin = ECHO_PIN;//回响引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(ULSOUND_PORT, &GPIO_InitStructure);//初始化GPIO
}

u32 UlSound_Ranging(void)
{
    u32 time;
    u32 distance = 0;
    TRIG_PIN_OUT = 1;
    Delay_us(20);
    TRIG_PIN_OUT = 0;
	  time = 0;
    while(ECHO_PIN_IN == 0)
    {     
        Delay_ms(1);
        time++;
        if(time > 1000)
        {
          printf("error1\n");
          return 0;
        }
    }
    time = 0;
    while(ECHO_PIN_IN == 1)
    {      
        Delay_us(9);
        time++;
        if(time > 1000000)
        {
          printf("error2\n");
          return 0;
        }
    }
    time = time/2;
    distance = time*3; 
    return distance;  
}

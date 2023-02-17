#include "ultrasound.h"

/**
 * @brief  LED Init
 * @param  none
 * @retval none  
 */
void UlSound_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOAʱ��

  //GPIOE4��ʼ������
  GPIO_InitStructure.GPIO_Pin = TRIG_PIN;//��������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(ULSOUND_PORT, &GPIO_InitStructure);//��ʼ��GPIO
  TRIG_PIN_OUT = 0;

  //GPIOE3��ʼ������
  GPIO_InitStructure.GPIO_Pin = ECHO_PIN;//��������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(ULSOUND_PORT, &GPIO_InitStructure);//��ʼ��GPIO
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

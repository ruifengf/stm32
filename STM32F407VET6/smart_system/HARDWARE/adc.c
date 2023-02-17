#include "adc.h"



void Adc_Init(void)
{    
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); 	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);      

    /* GPIO Init ****************************************************************/
    GPIO_InitStructure.GPIO_Pin = ADC_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(ADC_PORT, &GPIO_InitStructure); 	

    /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);    


    /* ADC2 Init ****************************************************************/
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC2, &ADC_InitStructure);
    /* ADC2 regular channel7 configuration **************************************/
    ADC_RegularChannelConfig(ADC2, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);


    ADC_Cmd(ADC2,ENABLE);
    /* Start ADC Software Conversion */ 
    ADC_SoftwareStartConv(ADC2);

}

void Dac_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    DAC_InitTypeDef   DAC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);


    GPIO_InitStructure.GPIO_Pin = DAC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//普通输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);//初始化GPIO

    /* DAC channel2 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);     
    DAC_Cmd(DAC_Channel_1, ENABLE);

	PAin(4) = 0;
}

void filter()
{
    U32  index;
    U8   loop_1,loop_2;
    U16  Voltage_buffer[SAMPLE_TIMES];


    for(index = 0;index < SAMPLE_TIMES; index++)
    {
        Voltage_buffer[index] = Get_Adc();
    }

    for(loop_1 = 0;loop_1 < SAMPLE_TIMES; loop_1++)
    {

    }


}

static U16 Get_Adc(void)
{
    U16  adc_value = 0;
    U16  adc_voltage = 0;
    while(RESET == ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
    ADC_ClearFlag(ADC2, ADC_FLAG_EOC);
    adc_value = ADC_GetConversionValue(ADC2);
    adc_voltage = adc_value * 3300 / 4095;
    return  adc_voltage;

}

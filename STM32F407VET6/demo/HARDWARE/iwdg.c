#include "iwdg.h"




void  Iwdg_Init(void)
{
    //������ʶ������Ź�
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    //���ö������Ź���Ƶֵ
    //32KHz��ƵΪ125Hz
    IWDG_SetPrescaler(IWDG_Prescaler_256);

    //�������Ź�����ֵ,0~124
    IWDG_SetReload(125-1);
    //���ؼ���ֵ
    IWDG_ReloadCounter();

    IWDG_Enable();
}

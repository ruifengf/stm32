#include "iwdg.h"




void  Iwdg_Init(void)
{
    //允许访问独立看门狗
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    //配置独立看门狗分频值
    //32KHz分频为125Hz
    IWDG_SetPrescaler(IWDG_Prescaler_256);

    //独立看门狗计数值,0~124
    IWDG_SetReload(125-1);
    //重载计数值
    IWDG_ReloadCounter();

    IWDG_Enable();
}

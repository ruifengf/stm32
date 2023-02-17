



void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
       PAout(7) = 0;
       PAout(7) = 1;
    }
    else
    {
        
    }
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
       PAout(6) = 0;
       PAout(6) = 1;
    }
    else
    {
        
    }
	EXTI_ClearITPendingBit(EXTI_Line4);
}

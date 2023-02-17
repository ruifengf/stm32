#include "blue_tooth.h"
#include "usart.h"
#include "delay.h"


void Bluetooth_Init(void)
{
    Send_Str("AT\r\n");
    Delay_ms(500);
    Send_Str("AT+NAMERelf\r\n");
    Delay_ms(500);
    Send_Str("AT+RESET\r\n");
    Delay_ms(2000);
}






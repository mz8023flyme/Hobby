#include "stm32f10x.h"   
#include "bsp_led.h"
#include "bsp_usart.h"

u8 CHSTRE[100]={'h','e','l','l','o'};
u8 CH_string[100]={'\r'};
extern u8 CH;
int main(void)
{
        LED_GPIO_Config ();
        USART_Config ();
        printf ("���Գ���\n");
//        USART_Sentstring(DEBUG_USARTx,CHSTRE);
        while(1)
        {
                switch (CH)
                {
                        case 'R': LED_RED;
                          break;
                        case 'B': LED_BLUE;
                          break;
                        case 'G': LED_GREEN;
                          break;
                        default: LED_OFF;
                          break;
                }
        }

}


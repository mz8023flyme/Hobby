#include "stm32f10x.h"   
#include "bsp_led.h"
#include "bsp_usart.h"


uint8_t chen[]= {0x10,0x82,0x10,0x82,0x10,0x84,0x10,0x88,
		 0xFB,0xD0,0x10,0x82,0x30,0x82,0x39,0xC4,
		 0x55,0xA8,0x52,0x90,0x92,0x82,0x14,0x82,
		 0x10,0x84,0x10,0x88,0x10,0x90,0x10,0xA0};/*"��",0*/

int main(void)
{	
	uint16_t i,j,k;
	
	USART_Config ();
	
	printf ("���Գ���\n");
	
	for(i=0;i<16;i++)
	{
		printf("\n");
		for(j=0;j<2;j++)
		{
			for(k=0;k<8;k++)
			{
				if(chen[i*2+j] & ( 0x80 >> k ))
				{
					printf("*");
				}
				else
				{
					printf(" ");
				}
			}
		}
	}
	
	while (1);	
	
}


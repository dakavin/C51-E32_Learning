#include "reg52.h"

void delay_ms(unsigned int xms) 
{
    unsigned int i, j;
    for(i=xms;i>0;i--)
    {
        for(j=124;j>0;j--)
        {}
    }
}


void open_led(unsigned int num){
	P2 = 0xFF & ~(1<<num)
}

void main()
{
	while(1){
		open_led(7);
		delay_ms(500);
		open_led(6);
		delay_ms(500);
		open_led(5);
		delay_ms(500);
	}
}
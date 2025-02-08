#include <reg52.h>

sbit led1= P2^7;         
sbit key1 = P3^7;

void delay_ms(unsigned int xms)   //@12MHz
{
    unsigned int i, j;
    for(i=xms;i>0;i--)
    {
        for(j=124;j>0;j--)
        {}
    }
}

void main()
{
		delay_ms(10);
    while(1)
    {
        //?????????
        if(key1 == 0)
        {
            led1= ~led1;
						delay_ms(1000);
        }
    }                
}
#include <reg52.h>

#define TIMS (65536-92)

sbit dc_an1 = P2^4;
sbit dc_an2 = P2^3;

unsigned char count,compare;

void delay_ms(unsigned int xms)   //@12MHz
{
    unsigned int i, j;
    for(i=xms;i>0;i--)
    {
        for(j=124;j>0;j--)
        {}
    }
}

void main(){
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TH0 = TIMS >> 8;
	TL0 = TIMS;
	TR0 = 1;
	ET0 = 1;
	EA = 1;
	dc_an1 = 0;
	dc_an2 = 0;
	while(1){
		compare = 10;
		delay_ms(5000);
		compare = 30;
		delay_ms(5000);
		compare = 60;
		delay_ms(5000);
		compare = 100;
		delay_ms(5000);
	}
}

void Timer0() interrupt 1{
	TH0 = TIMS >> 8;
	TL0 = TIMS;
	count++;
	count %= 100;
	if(count<compare){
		dc_an2 = 1;
	}else{
		dc_an2 = 0;
	}
}


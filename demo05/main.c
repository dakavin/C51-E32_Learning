#include "reg52.h"

#define TIMS (65536 - 9216)

sbit led1 = P2^7;
int count = 0;

// Timer1的中断函数
void Timer1() interrupt 1{
	count++;
	if(count == 10){
		led1 = ~led1;
		count = 0;
		TL0 = TIMS;
		TL1 = TIMS >> 8;
	}
}

void main(){
	TMOD = 0x01; //工作模式为1，选择内部系统时钟，软件启动
	TR0 = 1;
	TL0 = TIMS;
	TL1 = TIMS >> 8;
	ET0 = 1;
	EA = 1;
	
	while(1);
	
}
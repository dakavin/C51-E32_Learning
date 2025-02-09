#include <reg52.h>

sbit buzz = P2^1;

//带参延时函数，不需要头文件
void delay_ms(unsigned int xms)   //@12MHz
{
    unsigned int i, j;
    for(i=xms;i>0;i--)
    {
        for(j=124;j>0;j--)
        {}
    }
}

void speak(){
	buzz = 0;
	delay_ms(1);
	buzz = 1;
	delay_ms(1);
}

void main(){
	while(1){
		speak();
	}
}
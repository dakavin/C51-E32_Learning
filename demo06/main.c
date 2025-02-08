#include <reg52.h>
#include <stdio.h>

sbit led = P2^7;

void UartInit(){
	SCON = 0x50; // uart模式选择 	REN = 1; 允许uart接收数据
	TMOD &= 0x0F; // 清除定时器1的设置
	TMOD |= 0x20; // 定时器选择模式3
	TR1 = 1;	// 打开定时器
	TL1 = 0xFD; // 定时器自动装填
	TH1 = 0xFD; // 定时器自动装填
	PCON |= 0x0; // 设置SMOD为0
	ES = 1;
	EA = 1;
}

void GetMsg() interrupt 4{
	char recv;
	if(RI==1){
		RI = 0;
		recv = SBUF;
		if(SBUF == 'O'){
			led = 0;
		}else if(SBUF == 'C'){
			led = 1;
		}
	}
}

char putchar(char dat){
	SBUF = dat;
	while(!TI);
	TI=0;
	return dat;
}

void main(){
	UartInit();
	printf("Input O for open the led\r\nInput C for close the led!");
	while(1);
}


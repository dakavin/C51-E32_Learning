#include <reg52.h>

#define RIGHT 1
#define LEFT -1
static unsigned int led_num = 5;
static int led_flag = RIGHT;

sfr P4 = 0xe8;                   //for 89C5xRC/RD+ series and 90C5xRc/RD+, location at 0E8H
//sfr P4   = 0xc0;                  //for 90C5xAD series, location at 0C0H
sbit INT2 = P4 ^ 3;
sbit INT3 = P4 ^ 2;

sfr XICON = 0xc0;                   //for 89C5xRC/RD+ series and 90C5xRc/RD+, location at 0C0H
//sfr XICON = 0xe8;                 //for 90C5xAD series, location at 0E8H
sbit PX3 = XICON ^ 7;
sbit EX3 = XICON ^ 6;
sbit IE3 = XICON ^ 5;
sbit IT3 = XICON ^ 4;
sbit PX2 = XICON ^ 3;
sbit EX2 = XICON ^ 2;
sbit IE2 = XICON ^ 1;
sbit IT2 = XICON ^ 0;

sbit key = P4 ^ 3;

void delay_ms(unsigned int xms)   //@12MHz
{
    unsigned int i, j;
    for (i = xms; i > 0; i--) {
        for (j = 124; j > 0; j--) {}
    }
}

void initInt2() {
    EX2 = 1;
    IT2 = 1; // 选择下降沿触发中断
}

void open_led(unsigned int num) {
    P2 = 0xFF & ~(1 << num);
}

void exit2() interrupt 6{
    if(key == 0){
        led_flag = LEFT;
        delay_ms(10);
    }
}

void main() {
    initInt2();
    EA = 1;
    while (1) {
        open_led(led_num);
        delay_ms(1000);
        if(key == 1 && led_flag == LEFT){
            led_flag = RIGHT;
        }
        // 下一个显示的LED计算
        led_num += led_flag;
        // 逻辑圆环搭建
        if (led_num < 5)
            led_num = 7;
        else if (led_num > 7) {
            led_num = 5;
        }
    }
}
#include "reg52.h"
#include "intrins.h"

#define RIGHT 1
#define LEFT -1
static int led_flag = RIGHT;
static unsigned int led_num = 7;

// 中断部分代码（触摸按键P4.3 / INT2#）
sfr P4    = 0xe8;                   //for 89C5xRC/RD+ series and 90C5xRc/RD+, location at 0E8H
//sfr P4   = 0xc0;                  //for 90C5xAD series, location at 0C0H
sbit INT2 = P4^3;
sbit INT3 = P4^2;

sfr XICON = 0xc0;                   //for 89C5xRC/RD+ series and 90C5xRc/RD+, location at 0C0H
//sfr XICON = 0xe8;                 //for 90C5xAD series, location at 0E8H
sbit PX3  = XICON^7;
sbit EX3  = XICON^6;
sbit IE3  = XICON^5;
sbit IT3  = XICON^4;
sbit PX2  = XICON^3;
sbit EX2  = XICON^2;
sbit IE2  = XICON^1;
sbit IT2  = XICON^0;

sfr IPH = 0xB7;

sbit key = P4^3;

// 延时函数
void delay_ms(unsigned int xms)   //@12MHz
{
    unsigned int i, j;
    for (i = xms; i > 0; i--) {
        for (j = 124; j > 0; j--) {}
    }
}

// 外部中断0的回调函数
//void exit0 interrupt 0{
//
//}

// 外部中断2的回调函数
void exit2() interrupt 6{
    // 按下的时候，触发中断2，并且读取到key对应的管脚值为0
    // 按键按下时，灯光切换方向
    if(key == 0){
        led_flag = LEFT;
        delay_ms(10);
    }
}

// led控制代码
void open_led(unsigned int num){
    P2 = 0xFF & ~(1 << num);
}

void main(){
    IT2 = 1;
    EX2 = 1;
    IPH = 0x40;
    PX2 = 1;
    EA = 1;
    while(1){
        open_led(led_num);
        delay_ms(1000);
        if(led_flag == LEFT && key == 1){
            led_flag = RIGHT;
        }
        led_num += led_flag;
        if(led_num < 5){
            led_num = 7;
        }else if(led_num >7){
            led_num = 5;
        }
    }
}
#include <reg52.h>

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

sfr IPH = 0xB7; // 定义默认值，否则无法使用（用于定义中断优先级）

sbit key1 = P4^3;

#define TIMS (65536-9216)
static unsigned int count = 0;

sbit ds_pin = P0^3;
sbit stcp_pin = P0^4;
sbit shcp_pin = P0^5;

// 记录按键状态
static unsigned int state = 0; // 0 --- 清零重新计数，~0 --- 暂停计数

//共阴 数码管数组：0-9
unsigned char num[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

// 延时函数
void delay_ms(unsigned int xms)   //@12MHz
{
    unsigned int i, j;
    for (i = xms; i > 0; i--) {
        for (j = 124; j > 0; j--) {}
    }
}

void hc595_send_byte(unsigned char byte){
    unsigned int i;
    for (i = 0; i < 8; ++i) {
        // 串行输入引脚，即数据在一根信号线上按顺序一位位地传输
        if (byte & 0x80)
            ds_pin = 1;
        else
            ds_pin = 0;
        // SHCP发生一次上升沿，芯片从DS引脚上获取当前数据
        shcp_pin = 0;
        shcp_pin = 1;
        byte <<= 1;
    }
}

// num-需要显示的内容 addr-在哪个数码管显示
void hc595_send_data(unsigned char num,unsigned char addr){
    // 先发需要显示的数字
    hc595_send_byte(num);
    // 将移位寄存器填满后，再往移位寄存器中塞数据，数据会被从9脚输出
    // 再发需要点亮的数码管，这时候移位寄存器中的数据被移位到芯片2中
    if(addr == 0)
        // Q0控制 0b1111 1110 0xFE
        hc595_send_byte(0xFE);
    else if(addr == 1)
        // Q1控制 0b1111 1101 0xFD
        hc595_send_byte(0xFD);
    // 移位寄存器的8位数据全传输完毕后，制造一次锁存寄存器引脚的上升沿
    stcp_pin = 0;
    stcp_pin = 1;
}

//10ms执行一次
void Timer0() interrupt 1 {
    //1000毫秒执行一次计数
    if(count < 9900){
        count++;
    }
    //每次产生中断后重新设置下次定时器初值 - 10毫秒产生1次中断
    TH0 = TIMS >> 8;
    TL0 = TIMS;
}

void exit2() interrupt 6 {
    //当按键按下时灯切换
    if (key1 == 0) {
        if (state == 0) {
            //当前状态为计数状态，下一个状态暂停计数
            //通过关闭计数器，使程序暂停计数
            TR0 = 0; //定时器计数关闭
        } else {
            //当前状态为计数状态，下一个状态重新计数
            //通过开启计数器，程序重新计数
            count = 0;
            TR0 = 1; //定时器计数开启
            //设置预装载值 使得定时时间是10ms
            TL0 = TIMS; //0b0000 0000
            TH0 = TIMS >> 8; //1101 1100
        }
        //改变状态变量
        state = ~state;
        delay_ms(10);
    }
}

void main() {
    IT2 = 1; // 设置外部中断2的触发条件为下降沿
    EX2 = 1; // 运行中断2经过
    TMOD = 0x01;                    //配置定时器0为16位定时器，TH0、TL0全用
    TH0 = TIMS >> 8;                //设置定时初值高8位
    TL0 = TIMS;                     //设置定时初值低8位
    ET0 = 1;  //开启定时器0中断
    EA  = 1;  //开启全局中断
    TR0 = 1;  //定时器0开始计数
    IPH = 0x60; // 设置中断优先级，INT2 > Time0,PX2H=PX2=1，PT0H=PT0=0； 0b 0110 0000
    //PT0H = 0;
    //PT0 = 0;
    while(1){
        //在第一个数码管显示数字
        hc595_send_data(num[count / 1000], 0);
        //在第二个数码管显示数字
        hc595_send_data(num[(count / 100) % 10], 1);
    };
}


#include <reg52.h>

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

// 触摸按键的管脚，也是外部中断INT2的管脚，可以在中断监控管脚变化情况
sbit key = P4 ^ 3;
//10ms定时器
TIMS = 65536 - 9216;
// 用于计数
static unsigned int count = 0;
// 用于监控数码管状态 0~计数状态 1~暂停状态
static unsigned int state = 0;
// 数码管控制管脚
sbit ds_pin = P0^3;
sbit stcp_pin = P0^4;
sbit shcp_pin = P0^5;

// 共阴极数码管数组：0~9
unsigned char num[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

//操控数码管相关函数
void hc595_send_byte(unsigned char byte){
    unsigned int i;
    for (i = 0; i < 8; ++i) {
        if(byte & 0x80)
            ds_pin = 1;
        else
            ds_pin = 0;
        shcp_pin = 0;
        shcp_pin = 1;
        byte <<= 1;
    }
}

void hc595_send_data(unsigned char num,unsigned char addr){
    hc595_send_byte(num);
    if(addr == 0){
        hc595_send_byte(0xFE);
    }else if(addr == 1){
        hc595_send_byte(0xFD);
    }
    stcp_pin = 0;
    stcp_pin = 1;
}

void delay_ms(unsigned int xms) { //@12MHz
    unsigned int i, j;
    for (i = xms; i > 0; i--) {
        for (j = 124; j > 0; j--) {}
    }
}

// 配置定时器0（使用定时器中断）
void initTimer0() {
    // 选择模式2（自动装载） GATE=0，CT=0（选择系统时钟），M1=1，M0=0 0b0000 0010
    TMOD = 0x01;
    // 手动转载值，后续还需要重新装载
    TH0 = TIMS >> 8;
    TL0 = TIMS;
    // 开关控制
    TR0 = 1;
    //允许中断
    ET0 = 1;
}

// 定时器中断代码，用于执行1s切换一下数码管数据
// 此时是10ms触发一次，所以需要*100，数码管最大值为99，那么count最大为9900
// 数码管范围 0000~9900
// 需要操控count的值变化即可
void Timer0() interrupt 1{
    if(count<9900){
        count++;
    }
    TH0 = TIMS >> 8;
    TL0 = TIMS;
}

//配置外部中断2
void initInt2(){
    IT2 = 1; // 设置外部中断2的触发条件为下降沿
    EX2 = 1;
}

// 使用外部中断INT2来控制状态变化
void exit2() interrupt 6{
    if(key == 0){
        if(state == 0){ //计数状态 ---> 暂停状态
            TR0 = 0;
        }else{
            count = 0;
            TR0 = 1;
            TH0 = TIMS >> 8;
            TL0 = TIMS;
        }
    }
    // 改变状态量
    state = ~state;
    delay_ms(10);
}

void main() {
    initTimer0();
    initInt2();
    EA = 1;
    while(1){
        // 显示十位
        hc595_send_data(num[(count/100)/10],0);
        // 显示个位
        hc595_send_data(num[(count/100)%10],1);
    }
}
#include <reg52.h>
#include <stdio.h>

sbit ds_pin = P0 ^ 3;
sbit stcp_pin = P0 ^ 4;
sbit shcp_pin = P0 ^ 5;

void delay_ms(unsigned int xms)   //@12MHz
{
    unsigned int i, j;
    for (i = xms; i > 0; i--) {
        for (j = 124; j > 0; j--) {}
    }
}

//共阴 数码管数组：0-9
unsigned char num[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void hc595_send_byte(unsigned char byte) {
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
void hc595_send_data(unsigned char num, unsigned char addr) {
    // 先发需要显示的数字
    hc595_send_byte(num);
    // 将移位寄存器填满后，再往移位寄存器中塞数据，数据会被从9脚输出
    // 再发需要点亮的数码管，这时候移位寄存器中的数据被移位到芯片2中
    if (addr == 0)
        // Q0控制 0b1111 1110 0xFE
        hc595_send_byte(0xFE);
    else if (addr == 1)
        // Q1控制 0b1111 1101 0xFD
        hc595_send_byte(0xFD);
    // 移位寄存器的8位数据全传输完毕后，制造一次锁存寄存器引脚的上升沿
    stcp_pin = 0;
    stcp_pin = 1;
}

void uart_init(void)                //9600bps@11.0592MHz
{
    PCON &= 0x7F;                //波特率不倍速
    SCON = 0x50;                //8位数据,可变波特率
    TMOD &= 0x0F;                //清除定时器1模式位
    TMOD |= 0x20;                //设定定时器1为8位自动重装方式
    TL1 = 0xFD;                //设定定时初值
    TH1 = 0xFD;                //设定定时器重装值
    ET1 = 0;                //禁止定时器1中断
    TR1 = 1;                //启动定时器1
}

/*
**重写printf调用的putchar函数，重定向到串口输出
**需要引入头文件<stdio.h>
*****/
char putchar(char dat) {
    //输出重定向到串口
    SBUF = dat;     //写入发送缓冲寄存器
    while (!TI);    //等待发送完成，TI发送溢出标志位 置1
    TI = 0;      //对溢出标志位清零
    return dat;  //返回给函数的调用者printf
}

void main() {
    uart_init();
    while (1) {
        int i,j;
        //// 在第一个数码管显示数字0
        hc595_send_data(num[0], 0);
        //// 在第二个数码管显示数字3
        hc595_send_data(num[0], 1);
        for (i = 0; i < 61; ++i) {
            int ge = i % 10;
            int bai = i / 10;
            printf("ge = %d, bai = %d \r\n", ge, bai);
            for (j = 0; j < 100; ++j) { // 保证百位和个位都有足够的显示时间
                // 显示百位
                hc595_send_data(num[bai], 0);
                delay_ms(5);

                // 显示个位
                hc595_send_data(num[ge], 1);
                delay_ms(5);
            }
        }
        delay_ms(5000);
    }
}
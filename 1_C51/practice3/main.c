#include <reg52.h>
#include <stdio.h>

sbit led=P2^7;

// 定义LED_PWM 占空比，亮灭时间
static int temp = 0;
static int high_time = 0;
static int low_time = 800;

void UartInit(void)                //9600bps@11.0592MHz
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
char putchar(char dat){
    //输出重定向到串口
    SBUF = dat;     //写入发送缓冲寄存器
    while(!TI);    //等待发送完成，TI发送溢出标志位 置1
    TI = 0;      //对溢出标志位清零
    return dat;  //返回给函数的调用者printf
}

void Delayus(int t){
    while(t--){
        unsigned char i;
        i = 2;
        while(--i);
    }
}

void main()
{
    UartInit();
    while(1)
    {
        if(RI==1)  //RI=1说明串口接收到了数据
        {
            RI=0;   //RI置0保证下次接收
            //判断接收的数据，作出相应的操作
            // 注意：SBUF是接收二进制数据的，所以不能简单的理解为“占100的多少”
            // 应该是 100 0b0110 0100  左移3位 0b 0011 0010 0000 也就是800
            temp = SBUF % 101;
            printf("rev data = %d\r\n",temp);
            high_time = temp << 3;
            low_time = 800 - high_time;
        }
        //由于Delayus出入栈和接下来led=1需要赋值时间，导致high_time 为0时，灯依旧有一些亮度
        // 加入一层if判断，如果high_time 为0时，不对led赋值为0
        if(high_time!=0){
            led = 0;
            Delayus(high_time);
        }
        led = 1;
        Delayus(low_time);
    }
}
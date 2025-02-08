#include <reg52.h>

sbit sda = P0^1;
sbit scl = P0^2;

void Delay10us()		//@11.0592MHz
{
	unsigned char i;
	i = 2;
	while (--i);
}


void i2c_up(){
	scl = 1;
	sda = 1;
	Delay10us();
	sda = 0;
	Delay10us();
}

void i2c_down(){
	scl = 1;
	sda = 0;
	Delay10us();
	sda = 1;
	Delay10us();
}

void i2c_write_bit(unsigned char databit){
	scl = 0;
	if(databit == 0){
		sda = 0;
	}else{
		sda = 1;
	}
	Delay10us();
	scl = 1;
	Delay10us();
}

void i2c_write_byte(unsigned char datasend){
	unsigned char i = 0;
	for(i = 0;i<8;i++){
		if(datasend & 0x80){
			i2c_write_bit(1);
		}else{
			i2c_write_bit(0);
		}
		datasend = datasend << 1;
	}
}

unsigned char i2c_read_bit(){
	unsigned char databit = 0;
	scl = 0;
	Delay10us();
	scl = 1;
	Delay10us();
	if(sda){
		databit = 1;
	}else{
		databit = 0;
	}
	return databit;
}

unsigned char i2c_read_byte(){
	unsigned char value = 0;
	unsigned char i = 0;
	sda = 1;
	for(i = 0;i<8;i++){
		if(i2c_read_bit()){
			value = value | 0x01;
		}
		if(i<7){
			value = value << 1;
		}
	}
	scl = 0;
	Delay10us();
	return value;
}

void i2c_ack(){
	scl = 0;
	sda = 0;
	Delay10us();
	scl = 1;
	Delay10us();
	scl = 0;
}

unsigned char i2c_wait_ack(){
	unsigned char time = 0;
	scl = 0;
	sda = 1;
	Delay10us();
	scl = 1;
	Delay10us();
	while(sda){
		Delay10us();
		time++;
		if(time > 100){
			return 1;
		}
	}
	scl = 0;
	return 0;
}

void i2c_ack(){
	scl = 0;
	sda = 1;
	Delay10us();
	scl = 1;
	Delay10us();
	scl = 0;
}


void main(){
	i2c_up();
	i2c_write_byte(0x55);
	i2c_wait_ack();
	i2c_down();
	while(1){
	
	}
}
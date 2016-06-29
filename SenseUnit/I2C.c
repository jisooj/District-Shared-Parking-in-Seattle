//Based on: http://www.8051projects.net/wiki/I2C_Implementation_on_8051
#include "I2C.h"
#include "si4010.h"

#define SDA GPIO3
#define SCL GPIO2
 
void I2CInit()
{
	SDA = 1;
	i2c_delay();
	SCL = 1;
	i2c_delay();
}
 
void I2CStart()
{
	SDA = 0;
	i2c_delay();
	SCL = 0;
	i2c_delay();
}
 
/*void I2CRestart()
{
	SDA = 1;
	i2c_delay();
	SCL = 1;
	i2c_delay();
	SDA = 0;
	i2c_delay();
	SCL = 0;
	i2c_delay();
}*/
 
void I2CStop()
{
	SCL = 0;
	i2c_delay();
	SDA = 0;
	i2c_delay();
	SCL = 1; //1
	i2c_delay();
	SDA = 1; //1
	i2c_delay();
}
 
void I2CAck()
{
	SDA = 0;
	i2c_delay();
	SCL = 1;
	i2c_delay();
	SCL = 0;
	i2c_delay();
	SDA = 1;
	i2c_delay();
}
 
void I2CNak()
{
	SDA = 1;
	i2c_delay();
	SCL = 1;
	i2c_delay();
	SCL = 0;
	i2c_delay();
	SDA = 1;
	i2c_delay();
}
 
unsigned char I2CSend(unsigned char Data)
{
	 unsigned char i, ack_bit;
	 for (i = 0; i < 8; i++) {
		if ((Data & 0x80) == 0)
			SDA = 0;
		else
			SDA = 1;
		i2c_delay();
		SCL = 1;
		i2c_delay();
		SCL = 0;
		Data<<=1;
		i2c_delay();
	 }
	 SDA = 1;
	 i2c_delay();
	 SCL = 1;
	 i2c_delay();
	 ack_bit = SDA;
	 SCL = 0;
	 i2c_delay();
	 return ack_bit;
}
 
unsigned char I2CRead()
{
	unsigned char i, Data=0;
	for (i = 0; i < 8; i++) {
		SCL = 1;
		i2c_delay();
		if(SDA)
			Data |=1;
		if(i<7)
			Data<<=1;
		SCL = 0;
		i2c_delay();
	}
	return Data;
}

void i2c_delay(void)  {
    int i;
    for(i=0;i<6;i++)
    {
    }
}
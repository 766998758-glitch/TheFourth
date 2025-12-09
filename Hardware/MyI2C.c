#include "stm32f10x.h"                  // Device header
#include "Delay.h"


void My_I2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)BitValue);
	Delay_us(10);
}
void My_I2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)BitValue);
	Delay_us(10);
}

uint8_t My_I2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
	Delay_us(10);
	return BitValue;
}






void MyI2C_Init(void)
{
	//由于我们现在使用软件I2C，所以直接用GPIO就可以，不用看库函数
	//软件I2C初始化有两个任务：
	//任务一，把SCL和SDA都初始化成开漏输出模式
	//任务二，把SCL和SDA置高电平
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
	//此时I2C总线处于空闲状态
}


/**********I2C的六个基本单元**********/
//我好像...就是在根据MPU6050的设计，在对准置高低电平的时间而已


//起始条件
void MyI2C_Start(void)
{
		My_I2C_W_SDA(1);
		My_I2C_W_SCL(1);
		My_I2C_W_SDA(0);
		My_I2C_W_SCL(0);
}
//终止条件
void MyI2C_Stop(void)
{
		My_I2C_W_SDA(0);
		My_I2C_W_SCL(1);
		My_I2C_W_SDA(1);
}
//发送字节
void MYI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		My_I2C_W_SDA(!!(Byte & (0X80 >> i)));
		My_I2C_W_SCL(1);
		My_I2C_W_SCL(0);
	}
}
//接受字节
uint8_t MYI2C_ReceiveByte(void)
{
	uint8_t i ,Byte = 0x00;
	for(i = 0; i < 8; i++)
	{
		My_I2C_W_SDA(1);
		My_I2C_W_SCL(1);
		if(My_I2C_R_SDA()){Byte |= (0x80 >> i);}
		My_I2C_W_SCL(0);
	}
	return Byte;
}
//发送应答
void MYI2C_SendAck(uint8_t AckBit)
{
	My_I2C_W_SDA(AckBit);
	My_I2C_W_SCL(1);
	My_I2C_W_SCL(0);
}
//接受应答
uint8_t MYI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	My_I2C_W_SDA(1);
	My_I2C_W_SCL(1);
	AckBit = My_I2C_R_SDA();
	My_I2C_W_SCL(0);
	return AckBit;
}

























#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS 0xD0


//按照指定地址写一个字节的时序
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();
	MYI2C_SendByte(MPU6050_ADDRESS);
	MYI2C_ReceiveAck();
	MYI2C_SendByte(RegAddress);
	MYI2C_ReceiveAck();
	MYI2C_SendByte(Data);
	MYI2C_ReceiveAck();
	MyI2C_Stop();
}

//按照指定地址读一个字节的时序
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();
	MYI2C_SendByte(MPU6050_ADDRESS);
	MYI2C_ReceiveAck();
	MYI2C_SendByte(RegAddress);
	MYI2C_ReceiveAck();
	
	MyI2C_Start();
	MYI2C_SendByte(MPU6050_ADDRESS | 0x01);
	MYI2C_ReceiveAck();
	Data = MYI2C_ReceiveByte();
	MYI2C_SendAck(1);
	MyI2C_Stop();
	
	return Data;
}

void MPU6050_Init(void)
{
	MyI2C_Init();
	//这几个家伙的配置要看你个人需求，从MPU6050手册里面找
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);    //写入电源管理寄存器1，解除睡眠，选择陀螺仪时钟
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);    //写入电源管理寄存器2，六个轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);    //采样率分频，采样分频为10
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);        //配置寄存器，滤波参数给最大
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);      //陀螺仪配置寄存器，最大量程
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);     //加速度计配置寄存器，最大量程
	
}

//寄存器数据获取
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;   //加速度计X轴的16位数据

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;   //加速度计Y轴的16位数据

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;   //加速度计Z轴的16位数据
	
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;   //陀螺仪X轴的16位数据

	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;   //陀螺仪轴的16位数据

	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;   //陀螺仪Z轴的16位数据
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
	
}







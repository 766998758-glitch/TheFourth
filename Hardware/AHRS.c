#include "stm32f10x.h"   
#include "AHRS.h"
#include "math.h"

static Angles_t angles = {0, 0, 0};  //全局角度变量，先初始化0

//角速度和加速度轴的互补滤波系数
#define GYRO_WEIGHT 0.98f
#define ACC_WEIGHT 0.02f

//模拟量转化成显示物理量
#define GYRO_SCALE 131.0f  //±250°/s~131
#define ACC_SCALE 16384.0f  //±2g~16384

void AHRS_Init(void)
{
	angles.pitch = 0;
	angles.yaw = 0;
	angles.roll = 0;
}

void AHRS_Update(int16_t acc_raw[3], int16_t gyro_raw[3], float dt)
{	
		//1.转化成物理单位
		float ax = acc_raw[0] / ACC_SCALE;
		float ay = acc_raw[1] / ACC_SCALE;
		float az = acc_raw[2] / ACC_SCALE;

		float gx = gyro_raw[0] / GYRO_SCALE;
		float gy = gyro_raw[1] / GYRO_SCALE;
		float gz = gyro_raw[2] / GYRO_SCALE;

    //2.从加速度轴计算角度
		float pitch_acc = atan2(-ax, sqrt(ay*ay + az*az)) * 57.3f;  //57.3=180/Π
		float roll_acc = atan2(ay, az) * 57.3f;

		//3.互补滤波(最简化版本)
		angles.pitch = GYRO_WEIGHT * (angles.pitch + gy * dt) + ACC_WEIGHT * pitch_acc;
		angles.roll = GYRO_WEIGHT * (angles.roll + gx * dt) + ACC_WEIGHT * roll_acc;

    //4.不对Yaw做任何处理，直接积分(肯定得改，但是不是现在)
		angles.yaw += gz * dt;

    //5.保持Yaw别突破0/360°了
		if(angles.yaw > 360){angles.yaw -= 360;}
		if(angles.yaw < 0){angles.yaw += 360;}
}

Angles_t AHRS_GetAngles(void)
{
	return angles;
}












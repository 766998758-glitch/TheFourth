#include "stm32f10x.h"                  // Device header
#include "MPU6050.h"
#include "Delay.h"
#include "AHRS.h"

//校准陀螺仪零偏
int16_t gyro_bias[3] = {0, 0, 0};


//校准函数
void Calibrate(void)
{
	int32_t sum[3] = {0, 0, 0};
	int32_t yaw_bias_sum = 0;  //Yaw零偏误差
		
	Delay_ms(1000); //等我放平了
	
	//采样
	for(int i = 0; i < 500; i++)   //200个样本
	{
		int16_t acc[3], gyro[3];
		MPU6050_GetData(&acc[0],&acc[1],&acc[2],
										&gyro[0],&gyro[1],&gyro[2]);
	
		//对p和r的零偏处理
		sum[0] += gyro[0];
		sum[1] += gyro[1];
		sum[2] += gyro[2];
	
//		//对z轴处理成物理单位，用于Yaw零偏计算
//		float gz_raw = gyro[2];
//		float gz_deg_per_sec = gz_raw / 131.0f;
//		yaw_bias_sum += (int32_t)(gz_deg_per_sec * 1000.f);
		
		Delay_ms(10);
	}
	
	//计算陀螺仪零偏
	gyro_bias[0] = sum[0] / 500;
	gyro_bias[1] = sum[1] / 500;
	gyro_bias[2] = sum[2] / 500;
	
	//计算Yaw零偏，直接设置在AHRS.c里
	float gz_bias_deg = (float)gyro_bias[2] / 65.5f;  // 和GYRO_SCALE一致
  AHRS_SetYawBias(gz_bias_deg);
	
	//重置角度
	AHRS_Init();
}








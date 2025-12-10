#include "stm32f10x.h"                  // Device header
#include "MPU6050.h"
#include "Delay.h"

//校准数据
int16_t gyro_bias[3] = {0, 0, 0};


//校准函数
void Calibrate(void)
{
	int32_t sum[3] = {0, 0, 0};
		
	for(int i = 0; i < 50; i++)
	{
		int16_t acc[3], gyro[3];
		MPU6050_GetData(&acc[0],&acc[1],&acc[2],
										&gyro[0],&gyro[1],&gyro[2]);
	
		sum[0] += gyro[0];
		sum[1] += gyro[1];
		sum[2] += gyro[2];
	
		Delay_ms(20);
	}
	
	gyro_bias[0] = sum[0] / 50;
	gyro_bias[1] = sum[1] / 50;
	gyro_bias[2] = sum[2] / 50;
	
}

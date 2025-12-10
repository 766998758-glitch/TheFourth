#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "AHRS.h"
#include "MPU6050.h"
#include "Calibrate.h"

extern int16_t gyro_bias[3];
volatile Angles_t g_angles;    //共享的角度数据
volatile uint8_t data_ready = 0;

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	AHRS_Init();
	Calibrate();
	Timer_Init();
	
	OLED_ShowString(1, 1, "Pitch:");
	OLED_ShowString(2, 1, "Yaw:");
	
	while (1)
	{
		if(data_ready)
		{
			data_ready = 0;
			
			//安全复制数据
			Angles_t angles = g_angles;
			
		  int16_t pitch_x100 = (int16_t)(angles.pitch * 100);
      int16_t yaw_x100 = (int16_t)(angles.yaw * 100);
        
      OLED_ShowSignedNum(1, 7, pitch_x100, 3);
      OLED_ShowSignedNum(2, 7, yaw_x100, 3);
		}
	}
}




void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
				
				//1.读取传感器数据
				int16_t acc[3], gyro[3];
				MPU6050_GetData(&acc[0],&acc[1],&acc[2],
												&gyro[0],&gyro[1],&gyro[2]);
				
				//2.减去零偏
				gyro[0] -= gyro_bias[0];
				gyro[1] -= gyro_bias[1];
				gyro[2] -= gyro_bias[2];
			
				//3.姿态解算 dt=0.01s
				AHRS_Update(acc, gyro, 0.01f);
			
				//4.获取角度
				g_angles = AHRS_GetAngles();		
			
				//5.设置数据就绪标志
				data_ready = 1;
		}
}

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "AHRS.h"
#include "MPU6050.h"
#include "Calibrate.h"
#include "AD.h"


extern int16_t gyro_bias[3];
volatile Angles_t g_angles;    
volatile uint8_t data_ready = 0;

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	AHRS_Init();
	Calibrate();
	Timer_Init();
	AD_Init();
	
	OLED_ShowString(1, 1, "Pitch:");
	OLED_ShowString(2, 1, "Roll:");
	OLED_ShowString(3, 1, "Yaw:");
	
	
	while (1)
	{
		if(data_ready)
		{
			data_ready = 0;
			
			Angles_t angles = g_angles;
			
		   int16_t pitch_int = (int16_t)angles.pitch;
       if(pitch_int > 180) pitch_int = 180;
       if(pitch_int < -180) pitch_int = -180;
       OLED_ShowSignedNum(1, 7, pitch_int, 3);
            
       int16_t roll_int = (int16_t)angles.roll;
			 if(roll_int > 180) roll_int = 180;
       if(roll_int < -180) roll_int = -180;
       OLED_ShowSignedNum(2, 7, roll_int, 3);
            
           
       int16_t yaw_int = (int16_t)angles.yaw;
       OLED_ShowNum(3, 7, yaw_int, 4);  
			
		}
	}
}




void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
				
				int16_t acc[3], gyro[3];
				MPU6050_GetData(&acc[0],&acc[1],&acc[2],
												&gyro[0],&gyro[1],&gyro[2]);
				
				gyro[0] -= gyro_bias[0];
				gyro[1] -= gyro_bias[1];
				gyro[2] -= gyro_bias[2];
			
				AHRS_Update(acc, gyro, 0.01f);
			
				g_angles = AHRS_GetAngles();		
			
        data_ready = 1;
		}
}
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "AHRS.h"
#include "MPU6050.h"
#include "Calibrate.h"
#include "Cursor&Menu.h"
#include "Key.h"

extern int16_t gyro_bias[3];
volatile Angles_t g_angles;    
volatile uint8_t data_ready = 0;
int32_t pitch = 0,roll = 0,yaw = 0;

extern void UpdateCurrentMenu(void);

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	AHRS_Init();
	Calibrate();
	Timer_Init();
	Key_Init();
	
	OLED_Clear();
	line = 2;
	OLED_ShowString(2, 3, "ADC");
	OLED_ShowString(3, 3, "Store");
	OLED_ShowString(4, 3, "IMU");
	OLED_ShowString(line, 2, ">");  
//	if(line == 2 && KeyNum == 2)
//	{
//		OLED_Clear();
//		Menu1();
//	}
//	else if(line == 3 && KeyNum == 2)
//	{
//		OLED_Clear();
//		Menu2();
//	}
//	else if(line == 4 && KeyNum == 2)
//	{
//		OLED_Clear();
//		Menu3();
//	}
	
	while (1)
	{
		if(data_ready)
		{
			data_ready = 0;
			
			Angles_t angles = g_angles;
			
		 //Pitch
		 int16_t pitch_int = (int16_t)angles.pitch;
     if(pitch_int > 180) pitch_int = 180;
     if(pitch_int < -180) pitch_int = -180;
     pitch = pitch_int;
           
     //Roll
     int16_t roll_int = (int16_t)angles.roll;
     if(roll_int > 180) roll_int = 180;
     if(roll_int < -180) roll_int = -180;
     roll = roll_int;
            
     //Yaw
     int16_t yaw_int = (int16_t)angles.yaw;
     yaw = yaw_int;
		}
	}
}




void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
				
				Key_Tick();
			
				int16_t acc[3], gyro[3];
				MPU6050_GetData(&acc[0],&acc[1],&acc[2],
												&gyro[0],&gyro[1],&gyro[2]);
				
				gyro[0] -= gyro_bias[0];
				gyro[1] -= gyro_bias[1];
				gyro[2] -= gyro_bias[2];
			
				AHRS_Update(acc, gyro, 0.01f);
			
				g_angles = AHRS_GetAngles();		
			
				data_ready = 1;
				
			  Cursor();
				
				UpdateCurrentMenu();
		}
}

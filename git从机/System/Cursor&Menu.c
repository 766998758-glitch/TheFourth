#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "AD.h"
#include "MPU6050.h"
#include "Calibrate.h"
#include "AHRS.h"

uint8_t line = 2;
uint8_t current_menu;
uint8_t LongPress;
extern int32_t pitch,roll,yaw;

uint8_t current_menu = 0; 

void ShowMainMenu(void)
{
    OLED_Clear();
    OLED_ShowString(2, 3, "ADC");
    OLED_ShowString(3, 3, "Store");
    OLED_ShowString(4, 3, "IMU");
    OLED_ShowString(line, 2, ">");
}

void Menu1(void)
{
	current_menu = 1;
	OLED_Clear();
	OLED_ShowString(2, 3, "POT: ");
	OLED_ShowString(3, 3, "NTC: ");
	OLED_ShowString(4, 3, "LDR: ");
	
	OLED_ShowNum(2, 8, AD_Value[0], 4);
	OLED_ShowNum(3, 8, AD_Value[2], 4);
	OLED_ShowNum(4, 8, AD_Value[1], 4);
}

void Menu2(void)
{
	current_menu = 2;
	OLED_Clear();
	OLED_ShowString(2, 3, "POT: ");
	OLED_ShowString(3, 3, "NTC: ");
	OLED_ShowString(4, 3, "LDR: ");
	
	OLED_ShowNum(2, 8, AD_Value[0], 4);
	OLED_ShowNum(3, 8, AD_Value[2], 4);
	OLED_ShowNum(4, 8, AD_Value[1], 4);
}

void Menu3(void)
{
	current_menu = 3;
	OLED_Clear();
	OLED_ShowString(2, 3, "Pitch: ");
	OLED_ShowString(3, 3, "Roll: ");
	OLED_ShowString(4, 3, "Yaw: ");
	
	OLED_ShowNum(2, 8, pitch, 4);
	OLED_ShowNum(3, 8, roll, 4);
	OLED_ShowNum(4, 8, yaw, 4);
}

void UpdateCurrentMenu(void)
{
    switch(current_menu)
    {
        case 1:  
            OLED_ShowNum(2, 8, AD_Value[0], 4);
            OLED_ShowNum(3, 8, AD_Value[2], 4);
            OLED_ShowNum(4, 8, AD_Value[1], 4);
            break;
            
        case 2:  
            OLED_ShowNum(2, 8, AD_Value[0], 4);
            OLED_ShowNum(3, 8, AD_Value[2], 4);
            OLED_ShowNum(4, 8, AD_Value[1], 4);
            break;
            
        case 3:  
            OLED_ShowNum(2, 10, pitch, 4);
            OLED_ShowNum(3, 10, roll, 4);
            OLED_ShowNum(4, 10, yaw, 4);
            break;
            
        case 0:
            break;
    }
}


void Cursor(void)
{
	uint8_t LongPress = Key_GetLongPress();
	uint8_t current_key = Key_GetNum();
	
	if(current_key == 1)
	{
		 if(current_menu == 0)
     OLED_ShowString(line, 2, " ");
		 
		switch(line)
		{
			case 2: Menu1();
							break;
			case 3: Menu2();
							break;
			case 4: Menu3();
							break;
		}
	}
	else if(current_key == 2 && current_menu == 0)
	{
		OLED_ShowString(line, 2, " ");
		line = ((line - 2 + 1) % 3) + 2;  
		OLED_ShowString(line, 2, ">");
	}
	
	else if(current_key == 3)
	{
		OLED_Clear();
		current_menu = 0;
    ShowMainMenu();
	}
	
}

#ifndef __OLED_I2C_H
#define __OLED_I2C_H

#include "stm32f10x.h"


#define OLED_SCL_PIN     GPIO_Pin_6
#define OLED_SDA_PIN     GPIO_Pin_7
#define OLED_GPIO_PORT   GPIOB


void OLED_I2C_Init(void);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
uint8_t OLED_I2C_SendByte(uint8_t data);
uint8_t OLED_I2C_ReceiveByte(uint8_t ack);

#endif

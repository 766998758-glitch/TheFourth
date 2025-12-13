#ifndef __KEY_H
#define __KEY_H

void Key_Init(void);
uint8_t Key_GetNum(void);
void Key_Tick(void);
uint8_t Key_GetLongPress(void);      
uint8_t Key_GetLongPressActive(void);

#endif


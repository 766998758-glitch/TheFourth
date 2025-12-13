#ifndef __CURSOR_MENU_H
#define __CURSOR_MENU_H

#include "stm32f10x.h"

extern uint8_t line;
extern uint8_t current_menu;

void ShowMainMenu(void);
void Menu1(void);
void Menu2(void);
void Menu3(void);
void UpdateCurrentMenu(void);
void Cursor(void);

#endif

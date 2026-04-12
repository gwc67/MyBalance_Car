#ifndef __MENU_SHOW_H
#define __MENU_SHOW_H

#include "main.h"
#include "MENU.h"
#include "stdbool.h"
#include "Key.h"
#include "stdio.h"
#include "Store.h"
#include "st7735.h"
#include "font.h"




void Menu_Init(void);
void Menu_Show(void);
void Key_Up(void);
void Key_Down(void);
void Menu_Show_KeyEnter(void) ;
void Menu_Show_KeyBack(void);
void Menu_Show_KeySelect(void);
void Key_Enter(void);
void Menu_Choose(void);

#endif /* __MENU_SHOW_H */
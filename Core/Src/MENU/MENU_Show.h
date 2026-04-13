#ifndef __MENU_SHOW_H
#define __MENU_SHOW_H

#include "ALL.h"

#define menu_show_string(_x_, _y_, __string__) (OLED_ShowString((_x_), (_y_), (__string__), OLED_6X8))
#define menu_show_int(_x_, _y_, _int_, _len_) (OLED_ShowNum((_x_), (_y_), (_int_), (_len_), OLED_6X8))
#define menu_show_char(_x_, _y_, _char_) (OLED_ShowChar((_x_), (_y_), (_char_), OLED_6X8))
#define menu_show_float(_x_, _y_, _float_, _len_z_, _len_x_) (OLED_ShowFloatNum((_x_), (_y_), (_float_), (_len_z_), (_len_x_), OLED_6X8))


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
#ifndef __MENU_H
#define __MENU_H

#include "main.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "Store.h"


typedef enum MENU_KIND{
    MENU_Folder = 0 ,
    int32_Box,
    uint32_Box,
    int16_Box,
    uint16_Box,
    int8_Box,
    uint8_Box,
    float_Box,
    bool_Box
}MENU_KIND;

typedef struct MENU{
        
    uint8_t Sons;   //子节点数量
    uint8_t No;     //当前父节点下得第几个成员

    void *data;
    MENU_KIND kind; 

    bool select;

    const char *name; 

    struct MENU* parent;
    struct MENU* child;
    struct MENU* next;
    struct MENU* prev;

    bool isLimit;
    float limit_min;
    float limit_max;

    uint16_t data_index;  // 用于flash存储数据

}MENU;

/* void Create_Menu_Item(MENU* parent, MENU* me , const char name[],void *data,MENU_KIND kind);
 */
void Create_Menu_Folder(MENU* parent, MENU* me , const char name[]);
void Create_Menu_Number(MENU* parent, MENU* me , const char name[],void *data,MENU_KIND kind);
MENU* dynamicCreate_Menu_Folder(MENU* parent, const char name[]);
void dynamicCreate_Menu_Number(MENU* parent, const char name[],void *data,MENU_KIND kind);

MENU* dynamicCreate_Menu_LimitNumberBox(MENU* parent, const char name[], void* data, MENU_KIND kind, float limit_Min, float limit_Max);

void Circle_Menu(MENU * Menu);

#endif /* __MENU_H */
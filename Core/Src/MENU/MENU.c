#include "MENU.h"

#define MENU_MAX_SIZE (64)

 

static MENU Menu_Item_Arr[MENU_MAX_SIZE];
static uint8_t Menu_Arr_Index = 0;
static uint16_t flash_Index = 1;

static void __Create_Menu_Item(MENU *parent, MENU *me, const char name[], void *data, MENU_KIND kind, bool islimit, float limit_Min, float limit_Max)
{
    if (parent->kind != MENU_Folder)
        return;

    me->name = name;
    me->Sons = 0;

    me->select = false;

    me->parent = parent;
    me->child = NULL;
    me->next = NULL;
    me->prev = NULL;

    me->data = data;
    me->kind = kind;

    me->isLimit = islimit;
    me->limit_min = limit_Min;
    me->limit_max = limit_Max;

    if (kind == uint8_Box || kind == int8_Box)
    {
        me->data_index = flash_Index;
        flash_Index++;
    }
    else if (kind == uint16_Box || kind == int16_Box)
    {
        me->data_index = flash_Index;
        flash_Index += 1;
    }
    else if (kind == uint32_Box || kind == int32_Box || kind == float_Box)
    {
        me->data_index = flash_Index; // 两个uint16_t型变量
        flash_Index += 2;
    }
    else if (kind == bool_Box)
    {
        me->data_index = flash_Index;
        flash_Index++;
    }

    if (flag == 1)
    {
        switch (me->kind)
        {
        case int32_Box:
            Store_Data[me->data_index] = (uint16_t)*(int32_t *)(me->data);
            Store_Data[me->data_index + 1] = (uint16_t)(*(int32_t *)(me->data) >> 16);
            break;
        case float_Box:
            Store_Data[me->data_index] = *(uint16_t*)((uint8_t*)&(*( float *)(me->data)));
            Store_Data[me->data_index + 1] = *(uint16_t *)((uint8_t*)&(*(float *)(me->data)) + 2);
            break;
        case uint8_Box:
            Store_Data[me->data_index] = (uint16_t)*(uint8_t *)(me->data);
            break;
        case int8_Box:
            Store_Data[me->data_index] = (uint16_t)*(int8_t *)(me->data);
            break;
        case uint16_Box:
            Store_Data[me->data_index] = *(uint16_t *)(me->data);
            break;
        case int16_Box:
            Store_Data[me->data_index] = (uint16_t)*(int16_t *)(me->data);
            break;
        case uint32_Box:
            Store_Data[me->data_index] = (uint16_t)*(uint32_t *)(me->data);
            Store_Data[me->data_index + 1] = (uint16_t)(*(uint32_t *)(me->data) >> 16);
            break;
        default:
            break;
        }
        Store_Save();
    }

    if (parent->Sons == 0)
    {
        parent->child = me;
        // parent->No = 1; // Son有自增，因为当son = 1时 ， no 会 = 0；
    }
    else
    {
        MENU *p = parent->child;
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = me;
        me->prev = p;
    }

    
    parent->Sons++;
    me->No = parent->Sons;
}

#define Create_Menu_Item(parent, me, name, data, kind) __Create_Menu_Item(parent, me, name, data, kind, false, 0, 0)

void Create_Menu_Folder(MENU *parent, MENU *me, const char name[])
{
    Create_Menu_Item(parent, me, name, NULL, MENU_Folder);
}

void Create_Menu_Number(MENU *parent, MENU *me, const char name[], void *data, MENU_KIND kind)
{
    Create_Menu_Item(parent, me, name, data, kind);
}

MENU *dynamicCreate_Menu_Folder(MENU *parent, const char name[])
{
    MENU *me = &Menu_Item_Arr[Menu_Arr_Index++];

    Create_Menu_Item(parent, me, name, NULL, MENU_Folder);

    return me;
}

void dynamicCreate_Menu_Number(MENU *parent, const char name[], void *data, MENU_KIND kind)
{
    MENU *me = &Menu_Item_Arr[Menu_Arr_Index++];

    /* me->data_index = flash_Index++; */

    Create_Menu_Item(parent, me, name, data, kind);
}

void create_Menu_LimitNumberBox(MENU *father, MENU *me, const char *name, void *data, MENU_KIND kind, float limit_Min, float limit_Max)
{
    __Create_Menu_Item(father, me, name, data, kind, true, limit_Min, limit_Max);
}

MENU *dynamicCreate_Menu_LimitNumberBox(MENU *parent, const char name[], void *data, MENU_KIND kind, float limit_Min, float limit_Max)
{
    if (Menu_Arr_Index >= MENU_MAX_SIZE)
        return NULL;

    MENU *me = &Menu_Item_Arr[Menu_Arr_Index++]; // 实质：创建一个指向该静态内存池的指针

    create_Menu_LimitNumberBox(parent, me, name, data, kind, limit_Min, limit_Max);

    return me; // 该返回值的本质：获得文件夹的地址，从而可以在其下面进行创建文件
}

// 该函数的本质是将传入Menu下的（子菜单）首尾相接 , 因此如果要引入flash的话需要对最后的p.next == NULL 的情况的p本身进行处理，因为它没有经历过flash的初始化
void Circle_Menu(MENU *Menu)
{
    if (Menu->child == NULL) // 没有子菜单就退出
        return;

    MENU *hp = Menu->child;
    MENU *p = Menu->child;

    if (hp->next == NULL)
    {
        Circle_Menu(hp);
    }
    while (p->next != NULL) // 只有在while里面才会真正遍历每个文件，因此可以在此处进行数据的获取
    {
        if (p->kind == MENU_Folder)
        {
            Circle_Menu(p);
        }
        if (flag == 0)
        {

            if (p->kind != MENU_Folder)
            {
                switch (p->kind)
                {
                case uint8_Box:
                    *(uint8_t *)p->data = (uint8_t)Store_Data[p->data_index];
                    break;
                case int8_Box:
                    *(int8_t *)p->data = (int8_t)Store_Data[p->data_index];
                    break;
                case uint16_Box:
                    *(uint16_t *)p->data = (uint16_t)Store_Data[p->data_index];
                    break;
                case int16_Box:
                    *(int16_t *)p->data = (int16_t)Store_Data[p->data_index];
                    break;
                case uint32_Box:
                    *(uint32_t *)p->data = (uint32_t)Store_Data[p->data_index] | (uint32_t)Store_Data[p->data_index + 1] << 16;
                    break;
                case int32_Box:
                    *(int32_t *)p->data = (int32_t)Store_Data[p->data_index] | (int32_t)Store_Data[p->data_index + 1] << 16; // 看来是低位先行
                    break;
                case float_Box:
                    *(float *)p->data = *(float *)&Store_Data[p->data_index]; // 这步我没太看懂
                    break;
                case bool_Box:
                    *(bool *)p->data = (bool)Store_Data[p->data_index];
                    break;
                default:
                    break;
                }
            }
        }

        p = p->next;
    }

    if (hp->next != NULL)
    {
        Circle_Menu(p);
        switch (p->kind)
        {
        case uint8_Box:
            *(uint8_t *)p->data = (uint8_t)Store_Data[p->data_index];
            break;
        case int8_Box:
            *(int8_t *)p->data = (int8_t)Store_Data[p->data_index];
            break;
        case uint16_Box:
            *(uint16_t *)p->data = (uint16_t)Store_Data[p->data_index];
            break;
        case int16_Box:
            *(int16_t *)p->data = (int16_t)Store_Data[p->data_index];
            break;
        case uint32_Box:
            *(uint32_t *)p->data = (uint32_t)Store_Data[p->data_index] | (uint32_t)Store_Data[p->data_index + 1] << 16;
            break;
        case int32_Box:
            *(int32_t *)p->data = (int32_t)Store_Data[p->data_index] | (int32_t)Store_Data[p->data_index + 1] << 16; // 看来是低位先行
            break;
        case float_Box:
            *(float *)p->data = *(float *)&Store_Data[p->data_index]; // 这步我没太看懂
            break;
        case bool_Box:
            *(bool *)p->data = (bool)Store_Data[p->data_index];
            break;
        default:
            break;
        }

        hp->prev = p;
        p->next = hp;

        // 需要根据数据类型进行正确赋值
    }
}
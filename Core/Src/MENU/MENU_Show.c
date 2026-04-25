#include "MENU_Show.h"
#include "ui.h"


#if USE_WUWU_UI
#define CURSOR_UI_LEN (10) // UI光标长度(不要修改)
#define UI_CARTOON_SPD (3) // UI动画速度, 越大越慢1~10

#endif

#define Font_Width 6
#define Font_Hight 8
#define Show_Start_y (0)
#define Collum_Sum_len (21) // 还余了2个像素的宽度
#define Name_Len (10)
#define Number_Len (7)
#define EVERY_FOLDER_NUMBER (9) // 每页文件数量限制sssd

MENU head;
MENU *key;
#if USE_WUWU_UI
UI_ key_ui;
#endif

uint8_t u8_n = 0;

#define SETUP_LEN (7)
#define SETUP_NUMBER_LEN (7)
static float SetupNumber[SETUP_LEN] = {0.001, 0.01, 0.1, 1, 10, 100, 1000};
static uint8_t SetupIndex = 3;
uint8_t Setup_mode = 0;



void Menu_Init(void)
{ // metamorphosis
    head.data = NULL;
    head.child = NULL;
    head.prev = NULL;
    head.next = NULL;
    head.parent = NULL;
    head.Sons = 0;
    head.No = 0;
    head.name = "Menu";
    head.kind = MENU_Folder;
 
    MENU* AAA = dynamicCreate_Menu_Folder(&head,"Angle");
    MENU* BBB = dynamicCreate_Menu_Folder(&head,"Gyro");
    MENU* CCC = dynamicCreate_Menu_Folder(&head,"Speed");
    dynamicCreate_Menu_Number(AAA, "Kp", &AnglePID.Kp, float_Box);
    dynamicCreate_Menu_Number(AAA, "Ki", &AnglePID.Ki, float_Box);
    dynamicCreate_Menu_Number(AAA, "Kd", &AnglePID.Kd, float_Box);
    dynamicCreate_Menu_Number(AAA, "Target", &AnglePID.Target, float_Box);
    dynamicCreate_Menu_Number(AAA, "actual", &Angle , float_Box);
    dynamicCreate_Menu_Number(AAA, "out", &AnglePID.Out , float_Box);
    
    dynamicCreate_Menu_Number(BBB, "KpW", &GyroPID.Kp , float_Box);
    dynamicCreate_Menu_Number(BBB, "Kiw", &GyroPID.Ki , float_Box);
    dynamicCreate_Menu_Number(BBB, "Kdw", &GyroPID.Kd , float_Box);
    
    dynamicCreate_Menu_Number(CCC, "KpS", &SpeedPID.Kp , float_Box);
    dynamicCreate_Menu_Number(CCC, "KiS", &SpeedPID.Ki , float_Box);
    dynamicCreate_Menu_Number(CCC, "KdS", &SpeedPID.Kd , float_Box);



    Circle_Menu(&head);

    key = head.child;
#if USE_WUWU_UI
    key_ui.state = UI_BASE_IMG;
    key_ui.base.x = 0;
    key_ui.base.y = 8;
    key_ui.base.tarX = key_ui.base.x;
    key_ui.base.tarY = key_ui.base.y;
    key_ui.IMG.high = Font_Hight;
    key_ui.IMG.tarHigh = key_ui.IMG.high;
    key_ui.IMG.wide = strlen(key->name) * Font_Width;
    key_ui.IMG.img = NULL;
#endif
}

static void Menu_Show_Title(void)
{

    char tmpchar[Collum_Sum_len - SETUP_NUMBER_LEN + 1];
    for (uint8_t i = 0; i < Collum_Sum_len - SETUP_NUMBER_LEN + 1; i++)
    {
        tmpchar[i] = ' ';
    }
    sprintf(tmpchar, "%s/", key->parent->name);
    tmpchar[strlen(tmpchar)] = ' ';
    tmpchar[Collum_Sum_len - SETUP_NUMBER_LEN] = '\0';
    menu_show_string(0, Show_Start_y, tmpchar);
}

// 显示步进值
static void Menu_Show_Setup(void)
{
    char tmpchar[SETUP_NUMBER_LEN + 1];
    for (int i = 0; i < SETUP_NUMBER_LEN + 1; i++)
    {
        tmpchar[i] = ' ';
    }
    if (SetupNumber[SetupIndex] < 1)
    {
        sprintf(tmpchar, "%.3f", SetupNumber[SetupIndex]);
    }
    else
    {
        sprintf(tmpchar, "%.0f", SetupNumber[SetupIndex]);
    }
    tmpchar[strlen(tmpchar)] =   ' ';
    tmpchar[SETUP_NUMBER_LEN] = '\0';
    menu_show_string(Collum_Sum_len * Font_Width - Font_Width * SETUP_NUMBER_LEN, Show_Start_y, tmpchar);

    //设置步长这里
    if (Setup_mode == 1)
    {
        menu_show_char((Collum_Sum_len - SETUP_NUMBER_LEN - 1) * Font_Width  , Show_Start_y, '<');
        menu_show_char((Collum_Sum_len - 1) * Font_Width  , Show_Start_y, '>');
    }
    
}

void Menu_Show_Key(void)
{
    MENU *h = key->parent;
    MENU *s = h->child;

    for (int i = 1; i <= h->Sons; i++)
    {
#if !USE_WUWU_UI
        if (s == key)
        {
            menu_show_string(0, (i ) * Font_Hight, "->");
        }
        else
            menu_show_string(0, (i ) * Font_Hight, "  ");
#else
        if (Setup_mode != 1 && key->select != true)
        {
            if (s == key )
            {
                set_ui_tarXY(0, Font_Hight * i + Show_Start_y, &key_ui);
                set_ui_tarHW(Font_Hight, Font_Width * (strlen(key->name)) + CURSOR_UI_LEN, &key_ui);
            }
        }

#endif
        s = s->next;
    }
#if !USE_WUWU_UI // 这步还是用来去除上一页残存的-> ，当上一个页面的文件数大于这个页面时就会发生此bug
    for (int i = key->parent->Sons + 1; i <= EVERY_FOLDER_NUMBER; i++)
    {
        menu_show_string(0, Show_Start_y + Font_Hight * i, "  ");
    }

#else

    show_cursor_UI(&key_ui); // 将上面重新设置的key_ui 用于设置cursor_ui;
#endif
}

static void Menu_Show_task(void)
{

    MENU *son_p = key->parent->child;
    char tmpchar[Name_Len + 1];

    for (int i = 1; i <= key->parent->Sons; i++)
    {
        for (int j = 0; j < Name_Len + 1; j++)
        {
            tmpchar[j] = ' ';
        }

        if (son_p->kind == MENU_Folder)
        {
#if !USE_WUWU_UI
            sprintf(tmpchar, "%s[%d]", son_p->name, son_p->Sons);
#else
            show_Folder8x8_UI(0, Show_Start_y + Font_Hight * i);
            sprintf(tmpchar, "%s[%d]", son_p->name, son_p->Sons);
#endif
            tmpchar[strlen(tmpchar)] = ' ';
            tmpchar[Name_Len] = '\0';
        }
        else
        {
#if USE_WUWU_UI
            show_File8x8_UI(0, Show_Start_y + Font_Hight * i);
#endif
            sprintf(tmpchar, "%s", son_p->name);
            tmpchar[strlen(tmpchar)] = ' ';
            tmpchar[Name_Len] = '\0';
        }
#if !USE_WUWU_UI
        menu_show_string(Font_Width * 2, Show_Start_y + Font_Hight * i, tmpchar);
#else
        menu_show_string(CURSOR_UI_LEN, Show_Start_y + Font_Hight * i, tmpchar);
#endif
        son_p = son_p->next;
    }
#if !USE_WUWU_UI
    for (int j = 0; j < EVERY_FOLDER_NUMBER + 1; j++)
    {
        tmpchar[j] = ' ';
    }
    tmpchar[Name_Len] = '\0';
    for (int i = key->parent->Sons + 1; i <= EVERY_FOLDER_NUMBER; i++)
    {
        menu_show_string(Font_Width * 2, Show_Start_y + Font_Hight * i, tmpchar);
    }
#endif
}

// Number 原来是变量的意思
void Menu_Show_Number(void)
{
    MENU *h = key->parent;
    MENU *s = h->child;

    /* char buf[Name_Len + 3]; */
    for (int i = 1; i <= h->Sons; i++)
    {

        switch (s->kind)
        {

        case float_Box:
            if (*(float *)s->data > 10) // -1 依旧给'>'创建机会   -3 是为了给'.'以及后面两位创建机会
            {
                menu_show_float(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(float *)s->data, Number_Len - 3, 2);
            }
            else
                menu_show_float(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(float *)s->data, Number_Len - 4, 2);
            break;
        case bool_Box:
            if (*(bool *)(s->data) == true)
                menu_show_char(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, 'Y');
            else
                menu_show_char(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, 'N');
            break;
        case uint8_Box:
            menu_show_int(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(uint8_t *)s->data, Number_Len);
            break;
        case int8_Box:
            menu_show_int(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(int8_t *)s->data, Number_Len);

            break;
        case uint16_Box:
            menu_show_int(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(uint16_t *)s->data, Number_Len);

            break;
        case int16_Box:
            menu_show_int(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(int16_t *)s->data, Number_Len);

            break;
        case uint32_Box:
            menu_show_int(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(uint32_t *)s->data, Number_Len);

            break;
        case int32_Box:
            menu_show_int(Font_Width * (Collum_Sum_len - Number_Len - 1), i * Font_Hight + Show_Start_y, *(int32_t *)s->data, Number_Len);

            break;
        default:
            break;
        }
        if (s->select == true && Setup_mode != 1)
        {
            menu_show_char((Collum_Sum_len - 1) * Font_Width, i * Font_Hight + Show_Start_y, '>');
            menu_show_char((Collum_Sum_len - Number_Len - 2) * Font_Width, i * Font_Hight, '<');

        }
        else if (s->select == false && s->kind != MENU_Folder || s->select == true && Setup_mode == 1)
        {
            menu_show_char((Collum_Sum_len - 1) * Font_Width, i * Font_Hight + Show_Start_y, ' ');
            menu_show_char((Collum_Sum_len - Number_Len - 2) * Font_Width, i * Font_Hight, ' ');
        }
        s = s->next;
    }
}

void Menu_Show(void)
{

    Menu_Show_task();
    Menu_Show_Title();
    Menu_Show_Setup();
    Menu_Show_Number();
    Menu_Show_Key();
#if USE_WUWU_UI
    cartoon_move_ui(&key_ui, UI_CARTOON_SPD);
    cartoon_zoom_ui(&key_ui, UI_CARTOON_SPD);
#endif
}

void Menu_Show_KeyUp(void)
{
    if (key->prev != NULL)
    {
        key = key->prev;
    }
}

void Menu_Show_KeyDown(void)
{
    if (key->next != NULL)
    {
        key = key->next;
    }
}
void Menu_Show_KeyEnter(void)
{
    if (key->Sons > 0)
    {
        key = key->child;
    }
}

void Menu_Show_KeyBack(void)
{
    if (key->select != true)
    {
        if (key->parent->parent != NULL)
        {
            key = key->parent;
        }
    }
    else
    {
        // 此时为选中状态
        //  进入调整步进值的模式
        Setup_mode = 1;
    }
}

void Menu_Show_KeySelect(void)
{
    if (key->kind != MENU_Folder && key->kind != bool_Box)
    {
        key->select = !key->select; // 切换是否被选中
    }
}

void MenuNumber_Setup_Plus(void)
{
    SetupIndex = (SetupIndex + 1) % SETUP_LEN;
}

void MenuNumber_Setup_Sub(void)
{
    SetupIndex = (SetupIndex + SETUP_LEN - 1) % SETUP_LEN;
}

void Key_Plus(void)
{
    switch (key->kind)
    {
    case int32_Box:
        if (key->isLimit != true)
        {
            (*(int32_t *)(key->data)) = (*(int32_t *)(key->data)) + SetupNumber[SetupIndex];
        }
        else
        {
            (*(int32_t *)(key->data)) = (*(int32_t *)(key->data)) + SetupNumber[SetupIndex] > key->limit_max ? key->limit_max : (*(int32_t *)(key->data)) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(int32_t *)(key->data);
        Store_Data[key->data_index + 1] = (uint16_t)(*(int32_t *)(key->data) >> 16);
        break;
    case float_Box:
        if (key->isLimit != true)
        {
            (*(float *)(key->data)) = (*(float *)(key->data)) + SetupNumber[SetupIndex];
        }
        else
        {
            (*(float *)(key->data)) = (*(float *)(key->data)) + SetupNumber[SetupIndex] > key->limit_max ? key->limit_max : (*(float *)(key->data)) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = *(uint16_t *)((uint8_t *)&(*(float *)(key->data)));
        Store_Data[key->data_index + 1] = *(uint16_t *)((uint8_t *)&(*(float *)(key->data)) + 2);
        break;
    case uint8_Box:
        if (!key->isLimit)
        {
            *(uint8_t *)(key->data) = *(uint8_t *)(key->data) + SetupNumber[SetupIndex];
        }
        else
        {
            *(uint8_t *)(key->data) = *(uint8_t *)(key->data) + SetupNumber[SetupIndex] > key->limit_max ? key->limit_max : *(uint8_t *)(key->data) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(uint8_t *)(key->data);
        Store_Save();
        break;
    case int8_Box:
        if (!key->isLimit)
        {
            *(int8_t *)(key->data) = *(int8_t *)(key->data) + SetupNumber[SetupIndex];
        }
        else
        {
            *(int8_t *)(key->data) = *(int8_t *)(key->data) + SetupNumber[SetupIndex] > key->limit_max ? key->limit_max : *(int8_t *)(key->data) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(int8_t *)(key->data);
        break;
    case uint16_Box:
        if (!key->isLimit)
        {
            *(uint16_t *)(key->data) = *(uint16_t *)(key->data) + SetupNumber[SetupIndex];
        }
        else
        {
            *(uint16_t *)(key->data) = *(uint16_t *)(key->data) + SetupNumber[SetupIndex] > key->limit_max ? key->limit_max : *(uint16_t *)(key->data) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = *(uint16_t *)(key->data);
        break;
    case int16_Box:
        if (!key->isLimit)
        {
            *(int16_t *)(key->data) = *(int16_t *)(key->data) + SetupNumber[SetupIndex];
        }
        else
        {
            *(int16_t *)(key->data) = *(int16_t *)(key->data) + SetupNumber[SetupIndex] > key->limit_max ? key->limit_max : *(int16_t *)(key->data) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(int16_t *)(key->data);
        break;
    case uint32_Box:
        if (!key->isLimit)
        {
            *(uint32_t *)(key->data) = *(uint32_t *)(key->data) + SetupNumber[SetupIndex];
        }
        else
        {
            *(uint32_t *)(key->data) = *(uint32_t *)(key->data) + SetupNumber[SetupIndex] > key->limit_max ? key->limit_max : *(uint32_t *)(key->data) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(uint32_t *)(key->data);
        Store_Data[key->data_index + 1] = (uint16_t)(*(uint32_t *)(key->data) >> 16);
        Store_Save();
        break;
    default:
        break;
    }
    Store_Save();
}

void Key_Sub(void)
{
    switch (key->kind)
    {
    case int32_Box:
        if (key->isLimit != true)
        {
            (*(int32_t *)(key->data)) = (*(int32_t *)(key->data)) - SetupNumber[SetupIndex];
        }
        else
        {
            (*(int32_t *)(key->data)) = (*(int32_t *)(key->data)) - SetupNumber[SetupIndex] < key->limit_min ? key->limit_max : (*(int32_t *)(key->data)) + SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(int32_t *)(key->data);
        Store_Data[key->data_index + 1] = (uint16_t)(*(int32_t *)(key->data) >> 16);
        break;
    case float_Box:
        if (key->isLimit != true)
        {
            (*(float *)(key->data)) = (*(float *)(key->data)) - SetupNumber[SetupIndex];
        }
        else
        {
            (*(float *)(key->data)) = (*(float *)(key->data)) - SetupNumber[SetupIndex] < key->limit_min ? key->limit_min : (*(float *)(key->data)) - SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = *(uint16_t *)((uint8_t *)&(*(float *)(key->data)));
        Store_Data[key->data_index + 1] = *(uint16_t *)((uint8_t *)&(*(float *)(key->data)) + 2);
        break;
    case uint8_Box:
        if (!key->isLimit)
        {
            *(uint8_t *)(key->data) = *(uint8_t *)(key->data) - SetupNumber[SetupIndex];
        }
        else
        {
            *(uint8_t *)(key->data) = *(uint8_t *)(key->data) - SetupNumber[SetupIndex] < key->limit_min ? key->limit_min : *(uint8_t *)(key->data) - SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(uint8_t *)(key->data);
        break;
    case int8_Box:
        if (!key->isLimit)
        {
            *(int8_t *)(key->data) = *(int8_t *)(key->data) - SetupNumber[SetupIndex];
        }
        else
        {
            *(int8_t *)(key->data) = *(int8_t *)(key->data) - SetupNumber[SetupIndex] < key->limit_min ? key->limit_min : *(int8_t *)(key->data) - SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(int8_t *)(key->data);
        break;
    case uint16_Box:
        if (!key->isLimit)
        {
            *(uint16_t *)(key->data) = *(uint16_t *)(key->data) - SetupNumber[SetupIndex];
        }
        else
        {
            *(uint16_t *)(key->data) = *(uint16_t *)(key->data) - SetupNumber[SetupIndex] < key->limit_min ? key->limit_min : *(uint16_t *)(key->data) - SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = *(uint16_t *)(key->data);
        break;
    case int16_Box:
        if (!key->isLimit)
        {
            *(int16_t *)(key->data) = *(int16_t *)(key->data) - SetupNumber[SetupIndex];
        }
        else
        {
            *(int16_t *)(key->data) = *(int16_t *)(key->data) - SetupNumber[SetupIndex] < key->limit_min ? key->limit_min : *(int16_t *)(key->data) - SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(int16_t *)(key->data);
        break;
    case uint32_Box:
        if (!key->isLimit)
        {
            *(uint32_t *)(key->data) = *(uint32_t *)(key->data) - SetupNumber[SetupIndex];
        }
        else
        {
            *(uint32_t *)(key->data) = *(uint32_t *)(key->data) - SetupNumber[SetupIndex] < key->limit_min ? key->limit_min : *(uint32_t *)(key->data) - SetupNumber[SetupIndex];
        }
        Store_Data[key->data_index] = (uint16_t)*(uint32_t *)(key->data);
        Store_Data[key->data_index + 1] = (uint16_t)(*(uint32_t *)(key->data) >> 16);
        break;
    default:
        break;
    }
    Store_Save();
}

void Key_Up(void)
{
    switch (key->kind)
    {
    case MENU_Folder:
        Menu_Show_KeyUp();
        break;

    case bool_Box:
        Menu_Show_KeyUp();
        break;

    default:
        if (key->select != true)
        {
            Menu_Show_KeyUp();
        }
        else
        {
            switch (Setup_mode)
            {
            case 1:
                MenuNumber_Setup_Plus();
                break;
            default:
                Key_Plus();
                break;
            }
        }
        break;
    }
}
void Key_Down(void)

{
    switch (key->kind)
    {
    case MENU_Folder:
        Menu_Show_KeyDown();
        break;

    case bool_Box:
        Menu_Show_KeyDown();
        break;

    default:
        if (key->select != true)
        {
            Menu_Show_KeyDown();
        }
        else
        {
            switch (Setup_mode)
            {
            case 1:
                MenuNumber_Setup_Sub();
                break;

            default:
                Key_Sub();
                break;
            }
        }
        break;
    }
}

void Key_Enter(void)
{
    if (Setup_mode == 1)
    {
        Setup_mode = 0;
    }
    else
    {
        switch (key->kind)
        {
        case MENU_Folder:
            Menu_Show_KeyEnter();
            break;
        case bool_Box:
            *(bool *)(key->data) = !*(bool *)(key->data);
            Store_Data[key->data_index] = (uint16_t)*(bool *)(key->data);
            Store_Save();
            break;
        default:
            Menu_Show_KeySelect();
            break;
        }
    }
}

void Menu_Choose(void)
{

    if (Key_Check(KEY_1, KEY_SINGLE)|| Key_Check(KEY_1, KEY_REPEAT))
    {
        Key_Up();
    }
    else if (Key_Check(KEY_2, KEY_SINGLE) || Key_Check(KEY_2, KEY_REPEAT))
    {
        Key_Down();
    }
    else if (Key_Check(KEY_3, KEY_SINGLE)|| Key_Check(KEY_3, KEY_REPEAT))
    {
        Key_Enter();
    }
    // else if (Key_Check(KEY_2,KEY_DOUBLE) )  // 按键双击进入
    // {
    //     Key_Enter();
    // }
    else if (Key_Check(KEY_4, KEY_SINGLE)|| Key_Check(KEY_4, KEY_REPEAT))
    {
        Menu_Show_KeyBack();
    }

    Menu_Show();
}
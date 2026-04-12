#ifndef __UI_H_
#define __UI_H_
#include "ALL.h"

#define USE_WUWU_UI 0


extern const uint8_t IMG_Setting[];
extern const uint8_t IMG_Photo[];
extern const uint8_t IMG_Like[];
extern const uint8_t IMG_Choice[];
extern const uint8_t IMG_Folder[];
extern const uint8_t IMG_File[];


// void show_Folder8x8_UI(int16_t x ,int16_t y);
// void show_File8x8_UI(int16_t x ,int16_t y);

//UI基本类别
typedef enum UI_STATE {
    UI_CONST_IMG,
    UI_BASE_IMG,
} UI_STATE;

//UI基本参数
typedef struct UI_Base_Struct {
    int16_t x;          // 当前UI的左上角x坐标

    int16_t y;          // 当前UI的左上角y坐标

    int16_t tarX;       // 当前UI运动目标的左上角x坐标

    int16_t tarY;       // 目标UI运动目标的左上角y坐标
} UI_Base_Struct;

//图片类UI的基本参数
typedef struct UI_IMG_Base_Struct {
    uint8_t high;       // 当前图片的高

    uint8_t wide;       // 当前图片的宽

    uint8_t tarHigh;    // 当前图片目标高

    uint8_t tarWide;    // 当前图片目标宽

    uint8_t *img;       // 图片绑定的数组
} UI_IMG_Base_Struct;

//UI基本单位
typedef struct UI_ {
    UI_STATE state;
    UI_Base_Struct base;
    UI_IMG_Base_Struct IMG;
} UI_;

void set_ui_tarXY(int16_t x, int16_t y, UI_* ui);
void set_ui_tarHW(int16_t h, int16_t w, UI_* ui);

void cartoon_move_ui(UI_* ui, uint8_t spd);
void cartoon_zoom_ui(UI_* ui, uint8_t spd);

void show_Folder8x8_UI(int16_t x, int16_t y);
void show_File8x8_UI(int16_t x, int16_t y);
// void show_cursor_UI(UI_* cursor);


#endif
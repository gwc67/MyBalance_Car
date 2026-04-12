#ifndef __ST7735_H
#define __ST7735_H

#include <string.h>
#include "main.h"
 

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	
#define GRAY1   0x8410      
#define GRAY2   0x4208  


#define TFT180_DEFAULT_DISPLAY_DIR      (TFT180_PORTAIT)                        // 默认的显示方向
#define TFT180_DEFAULT_PENCOLOR         (0xffff)                          // 默认的画笔颜色
#define TFT180_DEFAULT_BGCOLOR          (0x0000)                          // 默认的背景颜色
#define TFT180_DEFAULT_DISPLAY_FONT     (TFT180_6X8_FONT)                      	// 默认的字体模式
#define LCD_W 128																// 屏幕尺寸W
#define LCD_H 160																// 屏幕尺寸H

#define TFT180_RES(x)					( (x) ? LL_GPIO_SetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin) : LL_GPIO_ResetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin) )
#define TFT180_DC(x)					( (x) ? LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin) : LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin) )
#define TFT180_CS(x)					( (x) ? LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin) : LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin) )

#define USE_BUFFER                      (1)                                     //此处置1使用内部显存

typedef enum
{
    TFT180_PORTAIT                      = 0,                                    // 竖屏模式
    TFT180_PORTAIT_180                  = 1,                                    // 竖屏模式  旋转180
    TFT180_CROSSWISE                    = 2,                                    // 横屏模式
    TFT180_CROSSWISE_180                = 3,                                    // 横屏模式  旋转180
}tft180_dir_enum;

typedef enum
{
    TFT180_6X8_FONT                     = 0,                                    // 6x8      字体
    TFT180_8X16_FONT                    = 1,                                    // 8x16     字体
    TFT180_16X16_FONT                   = 2,                                    // 16x16    字体 目前不支持
}tft180_font_size_enum;

// typedef struct SFont
// {
//    uint8_t height;
//    uint8_t width;
//    tft180_font_size_enum font;
// }SFont;

// SFont Font_8X16 = {.font = TFT180_8X16_FONT, .height = 16, .width = 8};
// SFont Font_6X8 = {.font = TFT180_6X8_FONT, .height = 8, .width = 6};

void tft180_SetBufferBit(uint16_t x, uint16_t y, int8_t bit);
void tft180_Buffer_Updata();
void tft180_init (void);
void tft180_clear(void);
void tft180_Buffer_Image(int16_t x, int16_t y, uint16_t high, uint16_t wide, const uint8_t *image); 
void tft180_set_color(uint16_t pen, uint16_t bgcolor);
void tft180_BufferShow_String(int16_t x , int16_t y , const char ch[] , tft180_font_size_enum size);
void tft180_BufferShow_int(int16_t x , int16_t y,const int32_t dat,uint8_t num, tft180_font_size_enum size);
void tft180_BufferShow_uint(int16_t x, int16_t y, const uint32_t dat, uint8_t num, tft180_font_size_enum size);
void tft180_BufferShow_float(int16_t x, int16_t y, const double dat, uint8_t num, uint8_t pointnum, tft180_font_size_enum size);    
void tft180_BufferDraw_rectangle_Rvs(int16_t x,int16_t y ,int16_t high, int16_t wide , int8_t round);
void tft180_BufferClean(void);

#endif
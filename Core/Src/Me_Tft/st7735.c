#include "spi.h"
#include "st7735.h"
#include "main.h"
#include "font.h"

#define Delay_ms(x) (LL_mDelay(x))

#define tft180_write_8bit_data(data) lcd_writeData(data)         //(ST7735_WriteByte(data))
#define tft180_write_16bit_data(data) lcd_writeData_16_Bit(data) //(ST7735_Write2Byte(data))

static uint8_t tft180_show_h = LCD_H;                                           // 屏幕高
static uint8_t tft180_show_w = LCD_W;                                           // 屏幕宽
static uint16_t tft180_pencolor = TFT180_DEFAULT_PENCOLOR;                      // 画笔颜色(字体色)
static uint16_t tft180_bgcolor = TFT180_DEFAULT_BGCOLOR;                        // 背景颜色
static tft180_font_size_enum tft180_display_font = TFT180_DEFAULT_DISPLAY_FONT; // 显示字体类型

// #define LCD_SCL_SET LL_GPIO_SetOutputPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin)
// #define LCD_SDA_SET LL_GPIO_SetOutputPin(LCD_MOSI_GPIO_Port, LCD_MOSI_Pin)
// #define LCD_CS_SET LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin)

// #define LCD_LED_SET LL_GPIO_SetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin)
// #define LCD_RS_SET LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin)
// #define LCD_RST_SET LL_GPIO_SetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin)

// #define LCD_SCL_CLR LL_GPIO_ResetOutputPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin)
// #define LCD_SDA_CLR LL_GPIO_ResetOutputPin(LCD_MOSI_GPIO_Port, LCD_MOSI_Pin)
// #define LCD_CS_CLR LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin)

// #define LCD_LED_CLR LL_GPIO_ResetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin)
// #define LCD_RST_CLR LL_GPIO_ResetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin)
// #define LCD_RS_CLR  LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin)

void Spi_WriteByte(uint8_t data)
{
    LL_SPI_TransmitData8(SPI1, data);
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
        ; // 发送寄存器空  置1
    while (LL_SPI_IsActiveFlag_BSY(SPI1))
        ; // busy下       置0
}

void tft180_write_index(uint8_t Index)
{
    TFT180_CS(0);
    TFT180_DC(0);
    Spi_WriteByte(Index);
    TFT180_CS(1);
    TFT180_DC(1);
}

void lcd_writeData(uint8_t Data)
{

    TFT180_CS(0);
    Spi_WriteByte(Data);
    TFT180_CS(1);
}

void lcd_writeData_16_Bit(uint16_t Data)
{
    TFT180_CS(0);
    LL_SPI_TransmitData8(SPI1, (uint8_t)((Data & 0xFF00) >> 8));
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
        ;
    LL_SPI_TransmitData8(SPI1, (uint8_t)(Data & 0x00FF));
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
        ;
    while (LL_SPI_IsActiveFlag_BSY(SPI1))
        ;
    TFT180_CS(1);
}

void tft180_set_color(uint16_t pen, uint16_t bgcolor)
{
    tft180_pencolor = pen;
    tft180_bgcolor = bgcolor;
}

static void tft180_set_region(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (tft180_show_h == LCD_H)
    {
        tft180_write_index(0x2a); // 对x进行设置地址
        tft180_write_8bit_data(0x00);
        tft180_write_8bit_data(x1);
        tft180_write_8bit_data(0x00);
        tft180_write_8bit_data(x2 + 2);

        tft180_write_index(0x2b); // 对y进行设置地址
        tft180_write_8bit_data(0x00);
        tft180_write_8bit_data(y1);
        tft180_write_8bit_data(0x00);
        tft180_write_8bit_data(y2 + 1);
    }
    else
    {
        tft180_write_index(0x2a);
        tft180_write_8bit_data(0x00);
        tft180_write_8bit_data(x1 + 1);
        tft180_write_8bit_data(0x0);
        tft180_write_8bit_data(x2 + 1);

        tft180_write_index(0x2b);
        tft180_write_8bit_data(0x00);
        tft180_write_8bit_data(y1 + 2);
        tft180_write_8bit_data(0x00);
        tft180_write_8bit_data(y2 + 2);
    }
    tft180_write_index(0x2c);
}

void tft180_clear(void)
{
    uint16_t i = tft180_show_w * tft180_show_h;
    TFT180_CS(0);
    tft180_set_region(0, 0, tft180_show_w - 1, tft180_show_h - 1);
    for (; i != 0; i--)
    {
        tft180_write_16bit_data(tft180_bgcolor);
    }
    TFT180_CS(1);
}

uint8_t AppBuffer[20][128];

void tft180_Buffer_Updata()
{
    TFT180_CS(0);
    tft180_set_region(0, 0, tft180_show_w - 1, tft180_show_h - 1);
    for (int j = 0; j < tft180_show_h; j++)
    {
            for (int i = 0; i < tft180_show_w; i++)
        {
            if (AppBuffer[j / 8][i] & (1 << j % 8))
            {
                tft180_write_16bit_data(tft180_pencolor);
            }
            else
            {
                tft180_write_16bit_data(tft180_bgcolor);
            }
        }
    }
    TFT180_CS(1);
}

void tft180_SetBufferBit(uint16_t x, uint16_t y, int8_t bit)
{
    if (x > 127 || y > 159)
    {
        return;
    }
    if (bit == 0)
    {
        AppBuffer[y / 8][x] &= ~(1 << (y % 8));
    }
    else if (bit == 1)
    {
        AppBuffer[y / 8][x] |= (1 << (y % 8));
    }
    else if (bit == -1)
    {
        AppBuffer[y / 8][x] ^= (1 << (y % 8)); // 异或等于，对此位进行翻转，1 变 0 ， 0 变 1 ； 因为任何位与1异或翻转，与0异或不变
    }
}

void tft180_BufferClean(void)
{
    memset(AppBuffer, 0, sizeof(AppBuffer));
}

void tft180_Bufferfull(uint8_t color) // color只能为0x00 , 0xff 因为目的:全部填充单个颜色 ， 要不 背景色 要不字体色
{
    memset(AppBuffer, color, sizeof(AppBuffer));
}

void tft180_init(void)
{

    tft180_BufferClean();

    tft180_set_color(tft180_pencolor, tft180_bgcolor);

    if (tft180_display_font == TFT180_16X16_FONT)
        tft180_display_font = TFT180_16X16_FONT;

    TFT180_RES(0);
    Delay_ms(10);

    TFT180_RES(1);
    Delay_ms(120);
    TFT180_CS(0);

    tft180_write_index(0x11);
    Delay_ms(120);

    tft180_write_index(0xB1);
    tft180_write_8bit_data(0x01);
    tft180_write_8bit_data(0x2C);
    tft180_write_8bit_data(0x2D);

    tft180_write_index(0xB2);
    tft180_write_8bit_data(0x01);
    tft180_write_8bit_data(0x2C);
    tft180_write_8bit_data(0x2D);

    tft180_write_index(0xB3);
    tft180_write_8bit_data(0x01);
    tft180_write_8bit_data(0x2C);
    tft180_write_8bit_data(0x2D);
    tft180_write_8bit_data(0x01);
    tft180_write_8bit_data(0x2C);
    tft180_write_8bit_data(0x2D);

    tft180_write_index(0xB4);
    tft180_write_8bit_data(0x07);

    tft180_write_index(0xC0);
    tft180_write_8bit_data(0xA2);
    tft180_write_8bit_data(0x02);
    tft180_write_8bit_data(0x84);
    tft180_write_index(0xC1);
    tft180_write_8bit_data(0xC5);

    tft180_write_index(0xC2);
    tft180_write_8bit_data(0x0A);
    tft180_write_8bit_data(0x00);

    tft180_write_index(0xC3);
    tft180_write_8bit_data(0x8A);
    tft180_write_8bit_data(0x2A);
    tft180_write_index(0xC4);
    tft180_write_8bit_data(0x8A);
    tft180_write_8bit_data(0xEE);

    tft180_write_index(0xC5);
    tft180_write_8bit_data(0x0E);

    tft180_write_index(0x36);
    switch (TFT180_DEFAULT_DISPLAY_DIR) // y x v
    {
    case TFT180_PORTAIT:
        tft180_write_8bit_data(1 << 7 | 1 << 6 | 0 << 5);
        break;
    case TFT180_PORTAIT_180:
        tft180_write_8bit_data(0 << 7 | 0 << 6 | 0 << 5);
        break;
    case TFT180_CROSSWISE:
        tft180_write_8bit_data(1 << 7 | 0 << 6 | 1 << 5);
        tft180_show_h = LCD_W;
        tft180_show_w = LCD_H;
        break;
    case TFT180_CROSSWISE_180:
        tft180_write_8bit_data(0 << 7 | 1 << 6 | 1 << 5);
        tft180_show_h = LCD_W;
        tft180_show_w = LCD_H;
        break;
    }

    tft180_write_index(0xe0);
    tft180_write_8bit_data(0x0f);
    tft180_write_8bit_data(0x1a);
    tft180_write_8bit_data(0x0f);
    tft180_write_8bit_data(0x18);
    tft180_write_8bit_data(0x2f);
    tft180_write_8bit_data(0x28);
    tft180_write_8bit_data(0x20);
    tft180_write_8bit_data(0x22);
    tft180_write_8bit_data(0x1f);
    tft180_write_8bit_data(0x1b);
    tft180_write_8bit_data(0x23);
    tft180_write_8bit_data(0x37);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x07);
    tft180_write_8bit_data(0x02);
    tft180_write_8bit_data(0x10);

    tft180_write_index(0xe1);
    tft180_write_8bit_data(0x0f);
    tft180_write_8bit_data(0x1b);
    tft180_write_8bit_data(0x0f);
    tft180_write_8bit_data(0x17);
    tft180_write_8bit_data(0x33);
    tft180_write_8bit_data(0x2c);
    tft180_write_8bit_data(0x29);
    tft180_write_8bit_data(0x2e);
    tft180_write_8bit_data(0x30);
    tft180_write_8bit_data(0x30);
    tft180_write_8bit_data(0x39);
    tft180_write_8bit_data(0x3f);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x07);
    tft180_write_8bit_data(0x03);
    tft180_write_8bit_data(0x10);

    tft180_write_index(0x2a);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x7f);

    tft180_write_index(0x2b);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x00);
    tft180_write_8bit_data(0x9f);

    tft180_write_index(0xF0);
    tft180_write_8bit_data(0x01);
    tft180_write_index(0xF6);
    tft180_write_8bit_data(0x00);

    tft180_write_index(0x3A);
    tft180_write_8bit_data(0x05);
    tft180_write_index(0x29);
    TFT180_CS(1);

    tft180_clear();
}

// Appbuffer 后面接|= 为重叠模式， 直接 = 为覆盖模式

void tft180_Buffer_Image(int16_t x, int16_t y, uint16_t high, uint16_t wide, const uint8_t *image)
{
    if (x > LCD_W - 1 || y > LCD_H - 1)
        return;
    if (x + wide <= 0 || y + high <= 0)
        return;

    int8_t offset_y = y % 8;
    int8_t offset_index_y = y / 8;

    uint16_t x_max = (x + wide <= LCD_W) ? wide : (LCD_W - 1 - x > 0) ? LCD_W - 1 - x
                                                                      : 0;

    for (int j = 0; j < high / 8 + ((offset_y == 0) ? 0 : 1); j++)
    {
        for (int i = 0; i < x_max; i++)
        {
            int8_t index_high = j + offset_index_y;
            if (index_high > 19) // 数组的索引为0 ~ 19
                return;
            if (i + x < 0)
                continue;
            if (index_high < 0) // 对显示部分不在屏幕上的处理方法
                continue;
            if (j == 0)
            {
                if (offset_y >= 0)
                {
                    AppBuffer[index_high][x + i] = image[i] << offset_y;
                }
                else
                {
                    AppBuffer[index_high][x + i] = (image[i] >> -offset_y) | (image[i + wide * (j + 1)] << 8 + offset_y);
                }
            }
            else if (offset_y == 0)
            {
                AppBuffer[index_high][x + i] = image[i + wide * j];
            }
            else if (j == high / 8 + ((offset_y == 0) ? 0 : 1) - 1)
            {
                if (offset_y >= 0)
                {
                    AppBuffer[index_high][x + i] = image[i + wide * (j - 1)] >> (8 - offset_y);
                }
            }
            else
            {
                if (offset_y >= 0)
                {
                    AppBuffer[index_high][x + i] = image[i + wide * j] << offset_y | image[i + wide * (j - 1)] >> 8 - offset_y;
                }
                else
                {
                    uint8_t color = (i + (j + 1) * wide < (high + 7) / 8 * wide) ? image[i + (j + 1) * wide] << 8 + offset_y : 0x00;
                    AppBuffer[index_high][x + i] = color | image[i + wide + j] >> -offset_y;
                }
            }
        }
    }
}

void tft180_BufferShow_String(int16_t x, int16_t y, const char ch[], tft180_font_size_enum size)
{
    uint8_t c = x;
    uint8_t j = 0;
    while (ch[j] != '\0')
    {
        switch (size)
        {
        case TFT180_6X8_FONT:
            tft180_Buffer_Image(c, y, 8, 6, ascii_font_6x8[ch[j] - 32]);
            c += 6;
            j++;
            break;
        case TFT180_8X16_FONT:
            tft180_Buffer_Image(c, y, 16, 8, ascii_font_8x16[ch[j] - 32]);
            c += 8;
            j++;
            break;
        default:
            break;
        }
    }
}

// 似乎该代码的执行效率更低，所以必须使用不同的显示函数

// void tft180_BufferShow_Printf(int16_t x,int16_t y, tft180_font_size_enum size , const char *fmt ,...)
// {
//     static char buffer[128];
//     va_list args;
//     va_start(args , fmt);
//     vsnprintf(buffer,sizeof(buffer),fmt,args);
//     va_end(args);
//     tft180_BufferShow_String(x,y,buffer,size);
// }

void func_int_to_str(char *str, int32_t number)
{
    uint8_t data_temp[16];
    uint8_t bit = 0;
    int32_t number_temp = 0;

    do
    {
        if (str == NULL)
        {
            break;
        }
        if (0 > number)
        {
            *str++ = '-'; // 将-加在number的前面
            number = -number;
        }
        else if (number == 0)
        {
            *str = '0';
            break;
        }

        while (number != 0)
        {
            number_temp = number % 10;
            data_temp[bit++] = number_temp;
            number /= 10;
        }
        while (bit != 0)
        {
            *str++ = data_temp[bit - 1] + 0x30; // 最后一次会多增加一次;
            bit--;
        }
    } while (0);
}

void tft180_BufferShow_int(int16_t x, int16_t y, const int32_t dat, uint8_t num, tft180_font_size_enum size)
{
    int32_t dat_temp = dat;
    int32_t offset = 1;
    char data_buffer[12];

    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num + 1);

    if (10 > num)
    {
        for (; 0 < num; num--)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_int_to_str(data_buffer, dat_temp);
    tft180_BufferShow_String(x, y, (const char *)&data_buffer, size);
}

static void func_uint_to_str(char *str, uint32_t data)
{
    uint32_t number = data;
    uint8_t buffer_temp[16]; // 作为存放数据每一位的数组
    uint8_t bit = 0;         // 作为数组的索引

    do
    {
        if (str == NULL)
        {
            return;
        }
        if (number == 0)
        {
            *str = '0';
            break;
        }

        while (number != 0)
        {
            uint32_t number_temp = number % 10;
            buffer_temp[bit++] = number_temp;
            number /= 10;
        }
        while (bit != 0)
        {

            *str++ = (buffer_temp[bit - 1] + 0x30);
            bit--;
        }
    } while (0);
}

void tft180_BufferShow_uint(int16_t x, int16_t y, const uint32_t dat, uint8_t num, tft180_font_size_enum size)
{
    char data_buffer[12];
    uint32_t dat_temp = dat;
    uint32_t offset = 1;
    memset(data_buffer, 0, sizeof(data_buffer)); // 对应char 类型来说 数字0就相当于什么都没有；
    memset(data_buffer, ' ', num);

    if (10 > num)
    {
        for (; num > 0; num--)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }

    func_uint_to_str(data_buffer, dat_temp);
    tft180_BufferShow_String(x, y, (const char *)&data_buffer, size);
}

void func_double_to_str(char *str, double number, uint8_t piont_bit)
{
    int data_temp;
    int data_buffer[12];
    int data_point;
    int data_pointbuffer[10];
    uint8_t bit = piont_bit;


    if (str == NULL)
    {
        return;
    }

    if (number == 0.0)
    {
        *str++ = '0';
        *str++ = '.';
        *str = '0';
        return;
    }
    else if (number < 0)
    {
        *str++ = '-';
        number = -number;
    }

    data_temp = (int)number;


    number = number - data_temp;
    while (bit != 0)
    {
        number = number * 10;
        bit--;
    }
    data_point = (int)number;


    do
    {
        while (data_temp != 0)
        {
            data_buffer[bit++] = data_temp % 10;
            data_temp /= 10;
        }
        while (bit != 0)
        {
            *str++ =( data_buffer[bit - 1] + 0x30);
            bit--;
        }
        bit = 0;
        if (piont_bit == 0)
        {
            *str++ = '.';
            *str = '0';
        }
        else
        {
            *str++ = '.';
            while (data_point != 0)
            {
                data_pointbuffer[bit++] = data_point % 10;
                data_point /= 10;
            }
            while (bit != 0)
            {
                *str++ = (data_pointbuffer[bit - 1] + 0x30 );
                bit--;
            }
        }

    } while (0);
}

void tft180_BufferShow_float(int16_t x, int16_t y, const double dat, uint8_t num, uint8_t pointnum, tft180_font_size_enum size)
{
    double dat_temp = dat;
    double offset = 1.0;
    char data_buffer[17];

    memset(data_buffer, 0, sizeof(data_buffer));
    memset(data_buffer, ' ', num + pointnum + 2);

    for (; num > 0; num--)
    {
        offset *= 10;
    }

    dat_temp = dat_temp - ((int)dat_temp / (int)offset) * offset;
    func_double_to_str(data_buffer,dat_temp,pointnum);
    tft180_BufferShow_String(x,y,data_buffer,size);


}

void tft180_BufferDraw_rectangle_Rvs(int16_t x,int16_t y ,int16_t high, int16_t wide , int8_t round)
{
    for(int i = 0 ; i < wide ; i++)
    {
        for (int j = 0; j < high; j++)
        {
            tft180_SetBufferBit(x + i,y + j, -1);
        }
    }
    if (round == 0)
    {
        return;
    }

    for (int i = 0; i < round; i++)   // 左上角
    {
        for (int j = 0; j < round -  i; j++)
        {
            tft180_SetBufferBit(x + i , y + j, 0);
        }
    }
    for (int i = 0; i < round; i++) // 右上角
    {
        for (int j = 0; j < round -  i; j++)
        {
            tft180_SetBufferBit(x + wide - i , y + j, 0);
        }
    }
    for (int i = 0; i < round; i++) // 左下角
    {
        for (int j = 0; j < round -  i; j++)
        {
            tft180_SetBufferBit(x + i , y + high - j, 0);
        }
    }
    for (int i = 0; i < round; i++)
    {
        for (int j = 0; j < round -  i; j++)
        {
            tft180_SetBufferBit(x + wide - i , y  + high - j, 0);
        }
    }
    

    
}
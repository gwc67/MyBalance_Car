#include "led_base.h"
#include "led_gpio.h"
#include "main.h"
#include "driver_registry.h"
LedBase *g_led_red;
LedBase *g_led_green;
LedBase *g_led_blue;
LedBase *g_led_ano;
LedBase *g_buzzer_pst;
DelayBase *pst_os_delay;  //句柄delay放后面，函数delay放前面
DelayBase *pst_hal_delay;  //句柄delay放后面，函数delay放前面
static LedGpio gpio_red;
static LedGpio gpio_green;
static LedGpio gpio_blue;
static LedGpio gpio_ano;
static LedGpio gpio_buzzer;

static LedGpio s_gpio_lazzer_st;
LedBase *g_lazzer_pst;

static stDelay st_os_delay;
static stDelay st_hal_delay;
void board_init(void)
{
    led_gpio_init(&gpio_red,"red",LED_R_GPIO_Port,LED_R_Pin,1);
    led_gpio_init(&gpio_green,"green",LED_G_GPIO_Port,LED_G_Pin,1);
    led_gpio_init(&gpio_blue,"blue",LED_B_GPIO_Port,LED_B_Pin,1);
    led_gpio_init(&gpio_ano,"ano",GPIOD,GPIO_PIN_7,1);
    led_gpio_init(&gpio_buzzer,"buzzer",GPIOB,GPIO_PIN_9,1);
    led_gpio_init(&s_gpio_lazzer_st,"lazzer",Lazer_GPIO_Port,Lazer_Pin,1);

    delay_os_init(&st_os_delay);
    delay_hal_init(&st_hal_delay);

    g_led_red = &gpio_red.base;
    g_led_green = &gpio_green.base;
    g_led_blue = &gpio_blue.base;
    g_led_ano  = &gpio_ano.base;
    g_buzzer_pst = &gpio_buzzer.base;
    g_lazzer_pst = &s_gpio_lazzer_st.base;

    pst_os_delay = &st_os_delay.base;
    pst_hal_delay = &st_hal_delay.base;
}
DRIVER_INIT_1(board_init);

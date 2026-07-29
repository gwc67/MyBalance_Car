D:\Downloads\stm32project\mydrone2\Core\Src\main.c

❯ @D:\Downloads\stm32project\mydrone2\Core\Src\main.c 我要复刻完这个项目的一些驱动，帮我完成，现在有很多报错


>> [  2%] CC '../Core/Src/OLED/LED.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\OLED\LED.c: 0 warnings, 1 error
>> [  5%] CC '../Core/Src/OLED/PID.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\OLED\PID.c: 0 warnings, 1 error
>> [  7%] CC '../Core/Src/Usart/Uart.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\Usart\Uart.c: 0 warnings, 1 error
>> [ 10%] CC '../Core/Src/Usart/BlueSerial.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\Usart\BlueSerial.c: 0 warnings, 1 error
>> [ 12%] CC '../Core/Src/I2C/MPU6050.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\I2C\MPU6050.c: 0 warnings, 1 error
>> [ 15%] CC '../Core/Src/OLED/Servo.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\OLED\Servo.c: 0 warnings, 1 error
>> [ 17%] CC '../Core/Src/MENU/OLED_Menu.c'
".\..\Core\Src\MENU\OLED_Menu.h", line 5: Error:  #5: cannot open source input file "Ano_Scheduler.h": No such file or directory
  #include "Ano_Scheduler.h"
                            ^
.\..\Core\Src\MENU\OLED_Menu.c: 0 warnings, 1 error
>> [ 20%] CC '../Core/Src/Usart/uart_device.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\Usart\uart_device.c: 0 warnings, 1 error
>> [ 23%] CC '../Core/Src/Key/Key_func.c'
".\..\Core\Src\Key\Key_func.c", line 2: Error:  #5: cannot open source input file "driver_registry.h": No such file or directory
  #include "driver_registry.h"
                              ^
.\..\Core\Src\Key\Key_func.c: 0 warnings, 1 error
>> [ 25%] CC '../Core/Src/LED/led_gpio.c'
".\..\Core\Src\LED\led_gpio.c", line 4: Error:  #5: cannot open source input file "cmsis_os2.h": No such file or directory
  #include "cmsis_os2.h"
                        ^
.\..\Core\Src\LED\led_gpio.c: 0 warnings, 1 error
>> [ 28%] CC '../Core/Src/MENU/MENU.c'
".\..\Core\Src\MENU\MENU.c", line 12: Error:  #5: cannot open source input file "oled_gfx.h": No such file or directory
  #include "oled_gfx.h"
                       ^
.\..\Core\Src\MENU\MENU.c: 0 warnings, 1 error
>> [ 30%] CC '../Core/Src/OLED/Encode.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\OLED\Encode.c: 0 warnings, 1 error
>> [ 33%] CC '../Core/Src/MENU/menu.c'
".\..\Core\Src\MENU\menu.c", line 12: Error:  #5: cannot open source input file "oled_gfx.h": No such file or directory
  #include "oled_gfx.h"
                       ^
.\..\Core\Src\MENU\menu.c: 0 warnings, 1 error
>> [ 35%] CC '../Core/Src/Key/Drv_Key.c'
".\..\Core\Src\Key\Drv_Key.c", line 2: Error:  #5: cannot open source input file "driver_registry.h": No such file or directory
  #include "driver_registry.h"
                              ^
.\..\Core\Src\Key\Drv_Key.c: 0 warnings, 1 error
>> [ 38%] CC '../Core/Src/I2C/MyI2C.c'
"../Core/ALL.h", line 13: Error:  #5: cannot open source input file "Key.h": No such file or directory
  #include "Key.h"
                  ^
.\..\Core\Src\I2C\MyI2C.c: 0 warnings, 1 error
>> [ 41%] CC '../Core/Src/control/ano_scheduler.c'
".\..\Core\Src\control\ano_scheduler.c", line 2: Error:  #5: cannot open source input file "FreeRTOS.h": No such file or directory
  #include "FreeRTOS.h"
                       ^
.\..\Core\Src\control\ano_scheduler.c: 0 warnings, 1 error
>> [ 43%] CC '../Core/Src/main.c'
".\..\Core\Src\main.c", line 21: Error:  #5: cannot open source input file "cmsis_os.h": No such file or directory
  #include "cmsis_os.h"
                       ^
.\..\Core\Src\main.c: 0 warnings, 1 error
>> [ 46%] CC '../Core/Src/freertos.c'
".\..\Core\Src\freertos.c", line 21: Error:  #5: cannot open source input file "FreeRTOS.h": No such file or directory
  #include "FreeRTOS.h"
                       ^
.\..\Core\Src\freertos.c: 0 warnings, 1 error

 ERROR  compilation failed at : "d:\Downloads\stm32project\new_car\Core\Src\freertos.c", exit code: 1
command: 
  "D:\Downloads\Core\ARM\ARMCC\bin\armcc.exe" -c --apcs=interwork -I. -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Dri
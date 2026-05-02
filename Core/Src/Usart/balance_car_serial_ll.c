/**
 * balance_car_serial_ll.c - 平衡车串口通信实现 (LL库版本)
 * 专为STM32平衡车项目与LLM-PID调参系统对接设计
 */

#include "balance_car_serial_ll.h"
#include "MENU.h"
#include <stdlib.h>
#include <stdarg.h>

// 串口接收缓冲区
#define CMD_BUFFER_SIZE 128
static char cmd_buffer[CMD_BUFFER_SIZE];
static uint16_t cmd_index = 0;

// 时间管理
static uint32_t last_report_time = 0;

// 使用您的USART定义
#define MyUSART USART1

 

void Serial_SendString_LL(const char* String) {
    for (uint16_t i = 0; String[i] != '\0'; i++) {
        Serial_SendByte_LL(String[i]);
    }
}

// 简化版printf实现
void Serial_Printf_LL(const char* format, ...) {
    char String[200];              // 定义字符数组
    va_list arg;                   // 定义可变参数列表数据类型的变量arg
    va_start(arg, format);         // 从format开始，接收参数列表到arg变量
    vsprintf(String, format, arg); // 使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);                   // 结束变量arg
    Serial_SendString_LL(String);  // 串口发送字符数组（字符串）
}

// 初始化串口通信
void Serial_Init_LL(void) {
 
    Serial_SendString_LL("# Balance Car PID Tuner Ready (LL Library)\\r\\n");
    Serial_SendString_LL("# Format: timestamp,angle,gyro,speed,diff,angle_p,angle_i,angle_d,gyro_p,gyro_i,gyro_d,speed_p,speed_i,speed_d,turn_p,turn_i,turn_d\\r\\n");
    
    last_report_time = 0; // 将在第一次调用时初始化
}

// 上报数据到上位机 (LLM-PID调参系统)
void Serial_ReportData_LL(void) {
    static uint32_t startup_time = 0;
    uint32_t current_time;
    
    // 初始化启动时间
    if (startup_time == 0) {
        startup_time = SysTick->VAL; // 使用系统滴答计时器
        last_report_time = startup_time;
        return;
    }
    
    current_time = SysTick->VAL;
    
    // 计算经过的时间（毫秒）
    uint32_t elapsed_ms = (startup_time - current_time) / (SystemCoreClock / 1000);
    
    // 按固定周期上报数据
    if (elapsed_ms - last_report_time >= DATA_REPORT_INTERVAL) {
        last_report_time = elapsed_ms;
        
        // CSV格式数据流
        // timestamp,angle,gyro,speed,diff,angle_p,angle_i,angle_d,gyro_p,gyro_i,gyro_d,speed_p,speed_i,speed_d,turn_p,turn_i,turn_d
        Serial_Printf_LL("%lu,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",
               elapsed_ms,
               Angle,                    // 当前角度
               GyroY_Actual,             // Y轴角速度
               AveSpeed,                 // 平均速度
               DifSpeed,                 // 速度差
               AnglePID.Kp, AnglePID.Ki, AnglePID.Kd,      // 角度环PID
               GyroPID.Kp, GyroPID.Ki, GyroPID.Kd,         // 角速度环PID
               SpeedPID.Kp, SpeedPID.Ki, SpeedPID.Kd,      // 速度环PID
               TurnPID.Kp, TurnPID.Ki, TurnPID.Kd          // 转向环PID
        );
    }
}

 
 
// 处理PID命令
void ProcessPIDCommand_LL(const char* command) {
    Serial_Printf_LL("# Received: %s\\r\\n", command);
    
    // 支持的命令格式：
    // SET ANGLE P:1.5 I:0.2 D:0.05
    // SET GYRO P:0.39 I:1.0 D:6.0
    // SET SPEED P:4.0 I:0.1 D:0.8
    // SET TURN P:1.0 I:0.05 D:0.2
    
    if (strncmp(command, "SET ANGLE", 9) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET ANGLE P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter_LL(&AnglePID, kp, ki, kd);
            Serial_Printf_LL("# Angle PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strncmp(command, "SET GYRO", 8) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET GYRO P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter_LL(&GyroPID, kp, ki, kd);
            Serial_Printf_LL("# Gyro PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strncmp(command, "SET SPEED", 9) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET SPEED P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter_LL(&SpeedPID, kp, ki, kd);
            Serial_Printf_LL("# Speed PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strncmp(command, "SET TURN", 8) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET TURN P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter_LL(&TurnPID, kp, ki, kd);
            Serial_Printf_LL("# Turn PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strcmp(command, "GET PID") == 0) {
        Serial_SendPIDParameters_LL();
    }
    else if (strcmp(command, "RESET") == 0) {
        // 重置所有PID参数为0
        UpdatePIDParameter_LL(&AnglePID, 0, 0, 0);
        UpdatePIDParameter_LL(&GyroPID, 0, 0, 0);
        UpdatePIDParameter_LL(&SpeedPID, 0, 0, 0);
        UpdatePIDParameter_LL(&TurnPID, 0, 0, 0);
        Serial_SendString_LL("# All PID parameters reset to 0\\r\\n");
    }
    else {
        Serial_SendString_LL("# ERROR: Unknown command\\r\\n");
    }
}

// 更新PID参数
void UpdatePIDParameter_LL(PID_t* pid, float kp, float ki, float kd) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;

    // 重置积分项，防止积分饱和
    pid->ErrorInt = 0;
    pid->Error0 = 0;
    pid->Error1 = 0;

    // 同步修改到flash，使外部修改也能持久化
    Menu_SyncVarToFlash(&pid->Kp);
    Menu_SyncVarToFlash(&pid->Ki);
    Menu_SyncVarToFlash(&pid->Kd);
    Menu_FlashSave();
}

// 发送当前PID参数
void Serial_SendPIDParameters_LL(void) {
    Serial_SendString_LL("# Current PID Parameters:\\r\\n");
    Serial_Printf_LL("# ANGLE: P=%.3f, I=%.3f, D=%.3f\\r\\n", AnglePID.Kp, AnglePID.Ki, AnglePID.Kd);
    Serial_Printf_LL("# GYRO:  P=%.3f, I=%.3f, D=%.3f\\r\\n", GyroPID.Kp, GyroPID.Ki, GyroPID.Kd);
    Serial_Printf_LL("# SPEED: P=%.3f, I=%.3f, D=%.3f\\r\\n", SpeedPID.Kp, SpeedPID.Ki, SpeedPID.Kd);
    Serial_Printf_LL("# TURN:  P=%.3f, I=%.3f, D=%.3f\\r\\n", TurnPID.Kp, TurnPID.Ki, TurnPID.Kd);
}

void USART1_IRQHandler(void) {
    // 检查是否接收到数据
    if (LL_USART_IsActiveFlag_RXNE(USART1)) {
        // 读取接收到的数据
        LL_USART_ClearFlag_RXNE(USART1);
        char received_char = (char)LL_USART_ReceiveData8(USART1);
        
        // 处理回车和换行
        if (received_char == '\r') {
            return; // 忽略回车
        }
        
        if (received_char == '\n') {
            // 命令结束，处理完整命令
            if (cmd_index > 0) {
                cmd_buffer[cmd_index] = '\0'; // 字符串结束符
                Serial_SendString(cmd_buffer);
                
                ProcessPIDCommand_LL(cmd_buffer);
                cmd_index = 0; // 重置缓冲区
            }
        } else {
            // 存储字符到缓冲区
            if (cmd_index < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_index++] = received_char;
            } else {
                // 缓冲区溢出
                Serial_SendString_LL("# ERROR: Command too long\\r\\n");
                cmd_index = 0;
            }
        }
    }
}
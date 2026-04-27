/**
 * balance_car_serial.c - 平衡车串口通信实现
 * 专为STM32平衡车项目与LLM-PID调参系统对接设计
 */

#include "balance_car_serial.h"
#include <stdlib.h>
// 串口接收缓冲区
#define CMD_BUFFER_SIZE 128
static char cmd_buffer[CMD_BUFFER_SIZE];
static uint16_t cmd_index = 0;

// 时间管理
static uint32_t last_report_time = 0;

// 初始化串口通信
void Serial_Init(void) {
    // 初始化USART1 (PA9-TX, PA10-RX)
    // 根据您的硬件配置修改
    
    // 示例：使用HAL库初始化
    // huart1.Instance = USART1;
    // huart1.Init.BaudRate = SERIAL_BAUD_RATE;
    // huart1.Init.WordLength = UART_WORDLENGTH_8B;
    // huart1.Init.StopBits = UART_STOPBITS_1;
    // huart1.Init.Parity = UART_PARITY_NONE;
    // huart1.Init.Mode = UART_MODE_TX_RX;
    // huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    // HAL_UART_Init(&huart1);
    
    // 发送初始化信息
    printf("# Balance Car PID Tuner Ready\\r\\n");
    printf("# Format: timestamp,angle,gyro,speed,diff,angle_p,angle_i,angle_d,gyro_p,gyro_i,gyro_d,speed_p,speed_i,speed_d,turn_p,turn_i,turn_d\\r\\n");
    
    last_report_time = HAL_GetTick();
}

// 上报数据到上位机 (LLM-PID调参系统)
void Serial_ReportData(void) {
    uint32_t current_time = HAL_GetTick();
    
    // 按固定周期上报数据
    if (current_time - last_report_time >= DATA_REPORT_INTERVAL) {
        last_report_time = current_time;
        
        // CSV格式数据流
        // timestamp,angle,gyro,speed,diff,angle_p,angle_i,angle_d,gyro_p,gyro_i,gyro_d,speed_p,speed_i,speed_d,turn_p,turn_i,turn_d
        printf("%lu,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\\r\\n",
               current_time,
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

// 处理串口接收到的命令
void Serial_ProcessCommand(void) {
    // 检查是否有数据可读
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) {
        char received_char = (char)huart1.Instance->DR;
        
        // 处理回车和换行
        if (received_char == '\\r') {
            return; // 忽略回车
        }
        
        if (received_char == '\\n') {
            // 命令结束，处理完整命令
            if (cmd_index > 0) {
                cmd_buffer[cmd_index] = '\\0'; // 字符串结束符
                ProcessPIDCommand(cmd_buffer);
                cmd_index = 0; // 重置缓冲区
            }
        } else {
            // 存储字符到缓冲区
            if (cmd_index < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_index++] = received_char;
            } else {
                // 缓冲区溢出
                printf("# ERROR: Command too long\\r\\n");
                cmd_index = 0;
            }
        }
    }
}

// 处理PID命令
void ProcessPIDCommand(const char* command) {
    printf("# Received: %s\\r\\n", command);
    
    // 支持的命令格式：
    // SET ANGLE P:1.5 I:0.2 D:0.05
    // SET GYRO P:0.39 I:1.0 D:6.0
    // SET SPEED P:4.0 I:0.1 D:0.8
    // SET TURN P:1.0 I:0.05 D:0.2
    
    if (strncmp(command, "SET ANGLE", 9) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET ANGLE P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter(&AnglePID, kp, ki, kd);
            printf("# Angle PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strncmp(command, "SET GYRO", 8) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET GYRO P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter(&GyroPID, kp, ki, kd);
            printf("# Gyro PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strncmp(command, "SET SPEED", 9) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET SPEED P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter(&SpeedPID, kp, ki, kd);
            printf("# Speed PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strncmp(command, "SET TURN", 8) == 0) {
        float kp = 0, ki = 0, kd = 0;
        if (sscanf(command, "SET TURN P:%f I:%f D:%f", &kp, &ki, &kd) == 3) {
            UpdatePIDParameter(&TurnPID, kp, ki, kd);
            printf("# Turn PID updated: P=%.3f, I=%.3f, D=%.3f\\r\\n", kp, ki, kd);
        }
    }
    else if (strcmp(command, "GET PID") == 0) {
        Serial_SendPIDParameters();
    }
    else if (strcmp(command, "RESET") == 0) {
        // 重置所有PID参数为0
        UpdatePIDParameter(&AnglePID, 0, 0, 0);
        UpdatePIDParameter(&GyroPID, 0, 0, 0);
        UpdatePIDParameter(&SpeedPID, 0, 0, 0);
        UpdatePIDParameter(&TurnPID, 0, 0, 0);
        printf("# All PID parameters reset to 0\\r\\n");
    }
    else {
        printf("# ERROR: Unknown command\\r\\n");
    }
}

// 更新PID参数
void UpdatePIDParameter(PID_t* pid, float kp, float ki, float kd) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    
    // 重置积分项，防止积分饱和
    // 根据您的PID实现，可能需要额外的重置逻辑
}

// 发送当前PID参数
void Serial_SendPIDParameters(void) {
    printf("# Current PID Parameters:\\r\\n");
    printf("# ANGLE: P=%.3f, I=%.3f, D=%.3f\\r\\n", AnglePID.Kp, AnglePID.Ki, AnglePID.Kd);
    printf("# GYRO:  P=%.3f, I=%.3f, D=%.3f\\r\\n", GyroPID.Kp, GyroPID.Ki, GyroPID.Kd);
    printf("# SPEED: P=%.3f, I=%.3f, D=%.3f\\r\\n", SpeedPID.Kp, SpeedPID.Ki, SpeedPID.Kd);
    printf("# TURN:  P=%.3f, I=%.3f, D=%.3f\\r\\n", TurnPID.Kp, TurnPID.Ki, TurnPID.Kd);
}

// 重定向printf到串口（如果尚未配置）
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}
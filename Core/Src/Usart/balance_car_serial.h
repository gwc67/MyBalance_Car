/**
 * balance_car_serial.h - 平衡车串口通信模块
 * 专为STM32平衡车项目与LLM-PID调参系统对接设计
 */

#ifndef BALANCE_CAR_SERIAL_H
#define BALANCE_CAR_SERIAL_H

#include "main.h"
#include <stdio.h>
#include <string.h>
#include "ALL.h"

// 串口配置
#define SERIAL_BAUD_RATE 115200
#define SERIAL_TIMEOUT_MS 100

// 数据上报周期 (ms)
#define DATA_REPORT_INTERVAL 50  // 50ms上报一次

// PID参数结构体（与您的项目匹配）
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float OutMax;
    float OutMin;
    float ErrorIntMax;
    float ErrorIntMin;
    float OutOffset;
    float Target;
    float Actual;
    float Out;
} PID_t;

// 外部声明的PID控制器（根据您的main.c）
extern PID_t AnglePID;
extern PID_t GyroPID;
extern PID_t SpeedPID;
extern PID_t TurnPID;

// 外部声明的传感器数据
extern float Angle;           // 当前角度
extern float GyroY_Actual;    // Y轴角速度
extern float AveSpeed;        // 平均速度
extern float DifSpeed;        // 速度差

// 函数声明
void Serial_Init(void);
void Serial_ReportData(void);
void Serial_ProcessCommand(void);
void Serial_SendPIDParameters(void);

// 指令处理函数
void ProcessPIDCommand(const char* command);
void UpdatePIDParameter(PID_t* pid, float kp, float ki, float kd);

#endif
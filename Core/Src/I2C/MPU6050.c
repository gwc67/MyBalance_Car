#include "MPU6050.h"

I2CBus_Struct MPU6050;

#define MPU6050_ADDRESS 0x68 // i2c address
#define MPU6050_SMPLRT_DIV 0x19
#define MPU6050_CONFIG 0x1A
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_FIFO_EN 0x23

#define MPU6050_INTBP_CFG_REG 0X37
#define MPU6050_INT_ENABLE 0x38

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40
#define MPU6050_TEMP_OUT_H 0x41
#define MPU6050_TEMP_OUT_L 0x42
#define MPU6050_GYRO_XOUT_H 0x43
#define MPU6050_GYRO_XOUT_L 0x44
#define MPU6050_GYRO_YOUT_H 0x45
#define MPU6050_GYRO_YOUT_L 0x46
#define MPU6050_GYRO_ZOUT_H 0x47
#define MPU6050_GYRO_ZOUT_L 0x48
#define MPU6050_SIGNAL_PATH_RESET 0x68
// 校准地址不列出
#define MPU6050_USER_CTRL 0x6A
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_PWR_MGMT_2 0x6C

#define MPU6050_WHO_AM_I 0x75

#define MPU6050_WRITE_REG(reg, data) MyI2C_WriteReg(&MPU6050, reg, data)
#define MPU6050_READ_REG(reg) MyI2C_ReadReg(&MPU6050, reg)
#define MPU6050_READ_REG_CONTINUE(reg, len, P_data) MyI2C_ReadReg_Continue(&MPU6050, reg, len, P_data)





typedef enum
{
    Band_256HZ = 0x00,
    Band_186HZ,
    Band_94HZ,
    Band_44HZ,
    Band_21HZ,
    Band_10HZ,
    Band_5HZ,
} Filiter_TypeDef;

typedef enum
{
    gyro_250 = 0x00,
    gyro_500 = 0x08,
    gyro_1000 = 0x10,
    gyro_2000 = 0x18
} GYRO_CONFIG_Typedef;

typedef enum
{
    acc_2g = 0x00,
    acc_4g = 0x08,
    acc_8g = 0x10,
    acc_16g = 0x18
} ACCEL_CONFIG_Typedef;
typedef enum
{
    FIFO_Disable,
    Acc_Out = 0x08,
    Gyro_Zout = 0x10,
    Gyro_Yout = 0x20,
    Gyro_Xout = 0x40,
    Temp_Out = 0x80,
} FIFO_EN_Typedef;

typedef struct MPU6050_InitTypeDef
{
    uint8_t SMPLRT_DIV;
    Filiter_TypeDef Filiter;
    GYRO_CONFIG_Typedef gyro_range;
    ACCEL_CONFIG_Typedef acc_range;
    FIFO_EN_Typedef FIFO_EN;
} MPU6050_InitTypeDef;


static float gyroScale = 1 /65.5  ;  // 角速度量程250°/s→131，500°/s→65.5，1000°/s→32.8，2000°/s→16.4
static float accelScale = 1.0f /  4096; // 加速度计量程2g→16384，4g→8192，8g→4096，16g→2048
static float mpu6050_dt =0.01f; // 采样时间间隔(s)
static float angle_yaw   = 0;
static float angle_roll  = 0;
static float angle_pitch = 0;


void MPU6050_Register_Init(MPU6050_InitTypeDef *MPU6050_StructureInit)
{
    MPU6050_WRITE_REG(MPU6050_PWR_MGMT_1, 0x00);
    MPU6050_WRITE_REG(MPU6050_PWR_MGMT_2, 0x00);
    MPU6050_WRITE_REG(MPU6050_SMPLRT_DIV, MPU6050_StructureInit->SMPLRT_DIV);
    MPU6050_WRITE_REG(MPU6050_CONFIG, MPU6050_StructureInit->Filiter);
    MPU6050_WRITE_REG(MPU6050_GYRO_CONFIG, MPU6050_StructureInit->gyro_range);
    MPU6050_WRITE_REG(MPU6050_ACCEL_CONFIG, MPU6050_StructureInit->acc_range);
    MPU6050_WRITE_REG(MPU6050_FIFO_EN, MPU6050_StructureInit->FIFO_EN);
}

void MPU6050_Get_Raw(MPU6050_raw *this)
{

    uint8_t temp_buffer[14];
    MPU6050_READ_REG_CONTINUE(MPU6050_ACCEL_XOUT_H,14,temp_buffer);
    this->AccX    = ((int16_t)temp_buffer[0] << 8) | temp_buffer[1];
    this->AccY    = ((int16_t)temp_buffer[2] << 8) | temp_buffer[3];
    this->AccZ    = ((int16_t)temp_buffer[4] << 8) | temp_buffer[5];
    this->temp = ((int16_t)temp_buffer[6] << 8) | temp_buffer[7];
    this->GyroX   = ((int16_t)temp_buffer[8] << 8) | temp_buffer[9];
    this->GyroY   = ((int16_t)temp_buffer[10] << 8) | temp_buffer[11];
    this->GyroZ   = ((int16_t)temp_buffer[12] << 8) | temp_buffer[13];

}


float MPU6050_GetTemp()
{
    uint8_t temp =  ((uint16_t)MPU6050_READ_REG(MPU6050_TEMP_OUT_H)) << 8 | MPU6050_READ_REG(MPU6050_TEMP_OUT_L);
    float temperture = (float)temp / 340 + 36.53;
    return temperture;
}
void MPU6050_Init(GPIO_TypeDef *SCL_Port, uint16_t SCL_Pin, GPIO_TypeDef *SDA_Port, uint16_t SDA_Pin)
{
    MyI2C_Init(&MPU6050, SCL_Port, SCL_Pin, SDA_Port, SDA_Pin, MPU6050_ADDRESS, 20);
    MPU6050_InitTypeDef MPU6050_Structure_init;
    MPU6050_Structure_init.SMPLRT_DIV = 1;
    MPU6050_Structure_init.Filiter = Band_256HZ;
    MPU6050_Structure_init.gyro_range = gyro_2000;
    MPU6050_Structure_init.acc_range = acc_8g;
    MPU6050_Structure_init.FIFO_EN = FIFO_Disable;

    MPU6050_Register_Init(&MPU6050_Structure_init);
}
uint8_t MPU6050_ID(void)
{
    return MPU6050_READ_REG(MPU6050_WHO_AM_I);
}

void MPU6050_Get_Angle(MPU6050_raw *this)
{
    float Ax, Ay, Az;
    float Gx, Gy, Gz;
    MPU6050_Get_Raw(this);
    // 读取加速度计数据
    Ax = this->AccX * accelScale; // 假设加速度计量程为±2g
    Ay = this->AccY * accelScale;
    Az = this->AccZ * accelScale;
    // 读取陀螺仪数据
    Gx = this->GyroX * gyroScale * mpu6050_dt;
    Gy = this->GyroY * gyroScale * mpu6050_dt;
    Gz = this->GyroZ * gyroScale * mpu6050_dt ;

    // 计算加速度的绝对值
    float absAcc = sqrt(Ax * Ax + Ay * Ay + Az * Az);
    // ACC_abs = absAcc;
    //  动态调整权重
    float weight;
    if (absAcc > 1.2) {
        // 快速运动或剧烈振动状态，减小加速度计权重
        weight = 0.8f;
    } else {
        // 正常运动状态，强烈信任加速度计
        weight = 0.98f;
    }

    static float Gyroscope_roll  = 0.0f;
    static float Gyroscope_pitch = 0.0f;
    Gyroscope_roll += Gy;
    Gyroscope_pitch += Gx;
    this->roll  = weight * atan2(Ay, Az) / 3.1415926f * 180.f + (1 - weight) * Gyroscope_roll;
    this->pitch = -(weight * atan2(Ax, Az) / 3.1415926f * 180.f + (1 - weight) * Gyroscope_pitch);

    
    // if(Gz > - 0.018f && Gz < 0.018f)  // 类似pid算法的优化
    // {
    //     Gz = 0;
    // }
    this->yaw += Gz *57.2958f  ;
    // printf("%.3f,%.3f,%.3f,%d\n", this->roll, this->pitch, this->yaw,temp); // 通过串口打印数据(用于检查驱动是否正常)
}

static inline float invSqrt(float x)
{
    float halfx = 0.5f * x;
    float y     = x;
    long i      = *(long *)&y;
    i           = 0x5f3759df - (i >> 1);
    y           = *(float *)&i;
    y           = y * (1.5f - (halfx * y * y)); // 一次牛顿迭代
    return y;
}

// 四元素法+动态互补滤波
void MPU6050_Get_Angle_Plus(MPU6050_raw *this)
{
    static uint16_t times = 0;
    static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
    static float Kp, Ki;
    static float integralX = 0.0f, integralY = 0.0f, integralZ = 0.0f;

    // 读取原始数据
    MPU6050_Get_Raw(this);

    // 转换为物理量
    float ax = (float)this->AccX * accelScale;
    float ay = (float)this->AccY * accelScale;
    float az = (float)this->AccZ * accelScale;
    float gx = (float)this->GyroX * gyroScale;
    float gy = (float)this->GyroY * gyroScale;
    float gz = (float)this->GyroZ * gyroScale;

    // 加速度幅值（用于自适应增益）
    float accMag = ax * ax + ay * ay + az * az;

    // 初始化阶段参数
    if (times < 400) {
        times++;
        Kp = 8.f;
        Ki = 0.002f;
    } else {
        // 动态调整增益
        Kp = (accMag > 1.44f || accMag < 0.64f) ? 3.6f : 4.8f;
        Ki = (accMag > 1.44f || accMag < 0.64f) ? 0.001f : 0.0015f;
    }

    // 加速度计有效时校正姿态
    if (accMag > 0.01f) {
        float recipNorm = invSqrt(accMag);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // 计算重力方向误差
        float vx = 2.0f * (q1 * q3 - q0 * q2);
        float vy = 2.0f * (q0 * q1 + q2 * q3);
        float vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

        float ex = ay * vz - az * vy;
        float ey = az * vx - ax * vz;
        float ez = ax * vy - ay * vx;

        // 积分项校正
        if (Ki > 0.0f) {
            integralX += ex * mpu6050_dt;
            integralY += ey * mpu6050_dt;
            integralZ += ez * mpu6050_dt;
            gx += Ki * integralX;
            gy += Ki * integralY;
            gz += Ki * integralZ;
        }

        // 比例项校正
        gx += Kp * ex;
        gy += Kp * ey;
        gz += Kp * ez;
    }

    // 四元数更新（核心计算）
    float qDot0 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    float qDot1 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
    float qDot2 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
    float qDot3 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

    q0 += qDot0 * mpu6050_dt;
    q1 += qDot1 * mpu6050_dt;
    q2 += qDot2 * mpu6050_dt;
    q3 += qDot3 * mpu6050_dt;

    // 四元数归一化
    float norm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= norm;
    q1 *= norm;
    q2 *= norm;
    q3 *= norm;

    this->q0 = q0;
    this->q1 = q1;
    this->q2 = q2;
    this->q3 = q3;

    // 计算欧拉角（四元数转欧拉角核心）
    this->roll        = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2)) * 57.29578f;
    this->pitch       = asinf(2.0f * (q0 * q2 - q3 * q1)) * 57.29578f;
    float current_yaw = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3)) * 57.29578f;

    // 最快的yaw角解包裹处理
    static float unwrapped_yaw = 0.0f;
    static uint8_t first_run   = 1;

    if (first_run) {
        unwrapped_yaw = current_yaw; // 初始化
        first_run     = 0;
    } else {
        // 仅用一次差值判断跳变，无累计计算
        float diff = current_yaw - unwrapped_yaw;
        if (diff > 180.0f) {
            unwrapped_yaw += diff - 360.0f; // 正向跳变修正
        } else if (diff < -180.0f) {
            unwrapped_yaw += diff + 360.0f; // 负向跳变修正
        } else {
            unwrapped_yaw = current_yaw; // 无跳变直接更新
        }
    }

    // 应用校准
    this->roll -= angle_roll;
    this->pitch -= angle_pitch;
    this->yaw = unwrapped_yaw - angle_yaw; // 输出连续yaw角
}
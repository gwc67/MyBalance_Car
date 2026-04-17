#include "MPU6050.h"

I2CBus_Struct MPU6050;

#define MPU6050_ADDRESS 0xD0 // i2c address
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
#define	MPU6050_PWR_MGMT_2		0x6C

#define MPU6050_WHO_AM_I 0x75

#define MPU6050_W_Reg(reg, data) MyI2C_WriteReg(&MPU6050, reg, data)
#define MPU6050_READ_REG(reg) MyI2C_ReadReg(&MPU6050, reg)
#define MPU6050_READ_REG_CONTINUE(reg, len, P_data) MyI2C_ReadReg_Continue(&MPU6050, reg, len, P_data)
#define MPU6050_DELAY_MS(ms) LL_mDelay(ms)

// uint8_t MPU6050_READ_REG(uint8_t reg)   //这步有问题
// {
//     uint8_t Data ; 
	
// 	//指定地址的操作
// 	 MyI2C_Start();
// 	MyI2C_SendByte(MPU6050_ADDRESS);
// 	MyI2C_ReceiveACK();
// 	MyI2C_SendByte(reg);
// 	MyI2C_ReceiveACK();
	
// 	//重新设置写读位
// 	MyI2C_Start();
// 	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
// 	MyI2C_ReceiveACK();
// 	Data = MyI2C_ReceiveByte();
// 	//  1 ： 从机不再发送数据  0 ： 从机继续发送数据
// 	MyI2C_SendACK(1);
// 	MyI2C_Stop();
	
// 	return Data;
// }
// void MPU6050_W_Reg(uint8_t Reg_ADDRESS,uint8_t Data)
// {
// 	MyI2C_Start();
// 	MyI2C_SendByte(MPU6050_ADDRESS);
// //	Note  ： 可以if 条件展开
// 	MyI2C_ReceiveACK();
// 	MyI2C_SendByte(Reg_ADDRESS);
// 	MyI2C_ReceiveACK();
// 	MyI2C_SendByte(Data);
// 	MyI2C_ReceiveACK();
// 	MyI2C_Stop();
	
// //扩展  ： 指定地址写多个字节  千问
// }
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

void MPU6050_Register_Init()
{
    MPU6050_W_Reg(MPU6050_PWR_MGMT_1,0x01);
	MPU6050_W_Reg(MPU6050_PWR_MGMT_2,0x00);

//	配置采样率分频寄存器  值越小输出数据越快  10分频
	MPU6050_W_Reg(MPU6050_SMPLRT_DIV,0x09);
//	配置 配置寄存器  110 对应平滑滤波
	MPU6050_W_Reg(MPU6050_CONFIG,0x06);
//	配置 陀螺仪寄存器  000 11 000 中间两个满量程位配置
	MPU6050_W_Reg(MPU6050_GYRO_CONFIG,0x18);
//	配置 加速度计寄存器  000 11 000 中间两个满量程位配置
	MPU6050_W_Reg(MPU6050_ACCEL_CONFIG,0x18);
}

void MPU6050_Get_Raw(MPU6050_raw *this)
{

    this->AccX = ((int16_t)(MPU6050_READ_REG(MPU6050_ACCEL_XOUT_H)) << 8) | MPU6050_READ_REG(MPU6050_ACCEL_XOUT_L);
    this->AccY = ((int16_t)(MPU6050_READ_REG(MPU6050_ACCEL_YOUT_H)) << 8) | MPU6050_READ_REG(MPU6050_ACCEL_YOUT_L);
    this->AccZ = ((int16_t)(MPU6050_READ_REG(MPU6050_ACCEL_ZOUT_H)) << 8) | MPU6050_READ_REG(MPU6050_ACCEL_ZOUT_L);
    this->Temp = ((uint16_t)(MPU6050_READ_REG(MPU6050_TEMP_OUT_H)) << 8) | MPU6050_READ_REG(MPU6050_TEMP_OUT_L);
    this->GyroX = ((int16_t)(MPU6050_READ_REG(MPU6050_GYRO_XOUT_H)) << 8) | MPU6050_READ_REG(MPU6050_GYRO_XOUT_L);
    this->GyroY = ((int16_t)(MPU6050_READ_REG(MPU6050_GYRO_YOUT_H)) << 8) | MPU6050_READ_REG(MPU6050_GYRO_YOUT_L);
    this->GyroZ = ((int16_t)(MPU6050_READ_REG(MPU6050_GYRO_ZOUT_H)) << 8) | MPU6050_READ_REG(MPU6050_GYRO_ZOUT_L);
}

void MPU6050_Init()
{
    MyI2C_Init();
    // MPU6050_InitTypeDef MPU6050_Structure_init;
    // MPU6050_Structure_init.SMPLRT_DIV = 1;
    // MPU6050_Structure_init.Filiter = Band_186HZ;
    // MPU6050_Structure_init.gyro_range = gyro_500;
    // MPU6050_Structure_init.acc_range = acc_8g;
    // MPU6050_Structure_init.FIFO_EN = FIFO_Disable;

    MPU6050_Register_Init( );
}
uint8_t MPU6050_ID(void)
{
    return MPU6050_READ_REG(MPU6050_WHO_AM_I);
}
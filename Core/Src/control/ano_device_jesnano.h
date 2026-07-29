#ifndef __ANO_DEVICE_JESNANO_H
#define __ANO_DEVICE_JESNANO_H

#include "main.h"

// Jesnano 雷达模块数据结构

struct Radar_Qua_t {
    float qX_f;
    float qY_f;
    float qZ_f;
    float qW_f;
};

struct Radar_Pos_t {
    int16_t x_s;
    int16_t y_s;
    int16_t z_s;
};

struct Radar_Speed_t {
    int16_t speed_x_s;
    int16_t speed_y_s;
    int16_t speed_z_s;
};

struct Jesnano_cam_raw_t {
    int16_t cam_error_x_s;
    int16_t cam_error_y_s;
};

typedef struct {
    uint32_t dummy;
} AnoBaseJesnano;

extern AnoBaseJesnano *pstAnobase_Jesnano;

void jesnano_radar_qua_copy(struct Radar_Qua_t *dst);
void jesnano_radar_pos_copy(struct Radar_Pos_t *dst);
void jesnano_radar_speed_copy(struct Radar_Speed_t *dst);
void jesnano_cam_raw_copy(struct Jesnano_cam_raw_t *dst);
void jesnano_copy_type(uint8_t *type);
void vano_cmd_send_v(AnoBaseJesnano *base, uint8_t head, uint8_t cmd, uint8_t *data, uint8_t len);

#endif

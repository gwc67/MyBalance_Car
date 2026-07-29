#ifndef __ANO_H
#define __ANO_H

#include "main.h"

// ANO 飞控数据结构

struct fc_bat_t {
    uint16_t voltage_100;
    uint16_t current_100;
};

void fc_bat_copy(struct fc_bat_t *dst);

#endif

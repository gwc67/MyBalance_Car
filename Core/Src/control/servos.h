#ifndef __SERVOS_H
#define __SERVOS_H

#include "main.h"

struct servo_t {
    uint8_t angle;
};

extern struct servo_t *cammer_servo_pst;

void servo_set_angle(struct servo_t *servo, uint8_t angle);

#endif

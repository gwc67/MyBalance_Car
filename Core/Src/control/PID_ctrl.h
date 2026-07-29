#ifndef __PID_CTRL_H
#define __PID_CTRL_H

#include "main.h"

enum { X_em = 0, Y_em };

void pid_reset_v(void *pid);
void *pid_get_loc_xyz(int axis);
void PID_Init(void);
void pid_set_xy_kp(float kp);
void pid_set_xy_ki(float ki);
void pid_set_xy_kd(float kd);

#endif

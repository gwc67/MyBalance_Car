#ifndef __FLY_TASK_H
#define __FLY_TASK_H

#include "main.h"

enum fly_task_phase_e {
    FLY_PHASE_IDLE_em = 0,
};

enum {
    QUESTION_MODE_1_em = 1,
    QUESTION_MODE_2_em = 2,
};

void fly_task_set_question_mode(uint8_t mode);
void fly_task_set_phase_v(enum fly_task_phase_e phase);
enum fly_task_phase_e fly_task_get_phase_em(void);
void fly_task_reset_v(void);

#endif

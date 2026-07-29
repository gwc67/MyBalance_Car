#include "ano_scheduler.h"
#include "FreeRTOS.h"
#include "AutoInit\driver_registry.h"
#include "task.h"


extern osThreadId_t task_10ms_highHandle;

extern osThreadId_t task_10_ms_lowHandle;


void task_1ms_fun(void *argument)
{

    driver_init_all();

  for(;;)
  {

  }

}


void task_10ms_high_fun(void *argument)
{

  for(;;)
  {
    osDelay(1);
  }

}



void task_10ms_low_fun(void *argument)
{

  for(;;)
  {
    osDelay(1);
  }

}


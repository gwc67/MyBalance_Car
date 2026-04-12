#ifndef __STORE_H
#define __STORE_H

#include "main.h"
#include "MyFlash.h"

extern uint8_t flag;


extern uint16_t Store_Data[512];
void Store_Init(void);
void Store_Save(void);


#endif /* __STORE_H */
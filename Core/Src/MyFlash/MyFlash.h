#ifndef __MYFLASH_H
#define __MYFLASH_H

#include "main.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "stdio.h"
uint32_t MyFlash_ReadWord(uint32_t addr);
uint16_t MyFlash_ReadHalfWord(uint32_t addr);
uint8_t MyFlash_ReadByte(uint32_t addr);
HAL_StatusTypeDef MyFlash_ErasePage(uint32_t PageAddress);

void MyFlash_Write(uint32_t TypeProgram, uint32_t Address, uint64_t Data);

#endif /* __MYFLASH_H */
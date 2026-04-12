#include "MyFlash.h"

extern void    FLASH_PageErase(uint32_t PageAddress);

uint32_t MyFlash_ReadWord(uint32_t addr)
{
    return *((__IO uint32_t*)(addr));
}

uint16_t MyFlash_ReadHalfWord(uint32_t addr)
{
    return *((__IO uint16_t*)(addr));
}

uint8_t MyFlash_ReadByte(uint32_t addr)
{
    return *((__IO uint8_t*)(addr));
}

/* void MyFlash_ErasePage(uint32_t address,uint32_t ErasePageNbpageCount)
{
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .PageAddress = address,
        .NbPages = ErasePageNbpageCount
    };
    uint32_t PageError = 0;
    __disable_irq();
    if (HAL_FLASHEx_Erase(&EraseInitStruct,&PageError) == HAL_OK)
    {   
        printf("擦除成功\r\n");
    }
    __enable_irq();
    HAL_FLASH_Lock();
} */

void MyFlash_Write(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
    HAL_FLASH_Lock();
    HAL_FLASH_Program(TypeProgram, Address, Data);
    HAL_FLASH_Unlock();
}

HAL_StatusTypeDef MyFlash_ErasePage(uint32_t PageAddress)
{
    HAL_StatusTypeDef status = HAL_OK;
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t sectorError = 0;

    HAL_FLASH_Unlock();

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = PageAddress;
    eraseInit.NbPages = 1;

    status = HAL_FLASHEx_Erase(&eraseInit, &sectorError);

    HAL_FLASH_Lock();

    if (status != HAL_OK)
    {
        return status;
    }
    return HAL_OK;
    

}
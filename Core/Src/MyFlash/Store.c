#include "Store.h"

extern void FLASH_PageErase(uint32_t PageAddress);

uint16_t Store_Data[512];

// 每次上电均会执行该代码 if --  只有 0xA5A5 ,for 将所有数据读到Store_Data里面
void Store_Init(void)
{
    if(MyFlash_ReadHalfWord(0x0800FC00) != 0xA5A5)
    {
        MyFlash_ErasePage(0x0800FC00);

        HAL_FLASH_Unlock();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800FC00, 0xA5A5);
        HAL_FLASH_Lock();

        for(int i = 1; i < 512; i++)
        {
            HAL_FLASH_Unlock();
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800FC00 + i*2, 0x0000);
            HAL_FLASH_Lock();
        }
        flag = 1;
    }
    for(uint16_t i = 0; i < 512; i++)
    {
        Store_Data[i] = MyFlash_ReadHalfWord(0x0800FC00 + i*2);  //每个指针指向一个8位故加二
    }
}

void Store_Save(void)
{
    MyFlash_ErasePage(0x0800FC00);

    for(uint16_t i = 0; i < 512; i++)
    {
        HAL_FLASH_Unlock();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800FC00 + i*2, Store_Data[i]);
        HAL_FLASH_Lock();
    }
}
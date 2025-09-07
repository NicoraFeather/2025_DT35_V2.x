//
// Created by lak19 on 2025/9/7.
//

#include "../Inc/flash_param.h"


ParamBlock_t param = {0};  // RAM 镜像

/* 根据地址算出 F4 扇区号 */
static uint32_t Flash_GetSector(uint32_t addr)
{
    if (addr < 0x08020000) return (addr - 0x08000000) >> 14;
    if (addr < 0x08080000) return 4 + ((addr - 0x08020000) >> 16);
    return 8 + ((addr - 0x08080000) >> 17);
}

/* 上电加载 */
void PARAM_Load(void)
{
    ParamBlock_t *p = (ParamBlock_t *)PARAM_FLASH_ADDR;
    if (p->magic == PARAM_MAGIC) {
        memcpy(&param, p, sizeof(param));
    } else {
        /* 首次运行，赋默认值 */
        param.magic  = PARAM_MAGIC;
        for (int i = 0; i < 4; i++) {
            param.k[i]     = 1.0f;
            param.b[i]     = 0.0f;
            param.can_id[i] = 100 + i;   // 缺省 100 101 102 103
        }
    }
}

/* 整扇区擦除 + 写入 */
void PARAM_Save(void)
{
    uint32_t sector = Flash_GetSector(PARAM_FLASH_ADDR);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {
        .TypeErase    = FLASH_TYPEERASE_SECTORS,
        .Sector       = sector,
        .NbSectors    = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t pageError = 0;
    HAL_FLASHEx_Erase(&erase, &pageError);

    /* 按字（32 bit）写入 */
    uint32_t *src = (uint32_t *)&param;
    uint32_t  cnt = sizeof(param) / 4;
    for (uint32_t i = 0; i < cnt; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          PARAM_FLASH_ADDR + i * 4,
                          src[i]);
    }

    HAL_FLASH_Lock();
}
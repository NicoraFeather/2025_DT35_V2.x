//
// Created by lak19 on 2025/9/13.
//

#ifndef USR_FLASH_H
#define USR_FLASH_H

#include "flash_F4xxxG.h"
#define  COEFFICIENT_ADDR 0x080E0000 // 存储线性系数的起始地址
#define  CAN_ID_ADDR     0x080E0080 // 存储 CAN ID 的起始地址

void STMFLASH_ReadFloatArray(uint32_t start_addr, float* float_array, int array_size);
void STMFLASH_ReadUint16Array(uint32_t start_addr, uint16_t* uint16_array, int array_size);
void WriteFloatArrayToFlash(uint32_t start_addr, float* float_array, int array_size);
void WriteUint16ArrayToFlash(uint32_t start_addr, uint16_t* uint16_array, int array_size);
void DT35_Flash_Update();
void DT35_Flash_Init();

#endif //USR_FLASH_H

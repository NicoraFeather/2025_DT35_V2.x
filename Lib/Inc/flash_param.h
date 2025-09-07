//
// Created by lak19 on 2025/9/7.
//

#ifndef FLASH_PARAM_H
#define FLASH_PARAM_H

#include <stdint.h>
#include "main.h"
#include "stm32f4xx.h"
#include <string.h>

/* 参数扇区：F4 1 MB 版  扇区 11  128 kB  首地址 */
#define PARAM_FLASH_ADDR     0x081E0000UL
#define PARAM_MAGIC          0x1234BEEFUL

/* 一次性存全部参数 */
typedef struct {
    uint32_t magic;      // 校验字
    float    k[4];       // 线性系数
    float    b[4];
    uint16_t can_id[4];  // 4 路 CAN ID
    uint16_t _pad;       // 8 字节对齐
} ParamBlock_t;

void PARAM_Load(void);   // 上电调用
void PARAM_Save(void);   // 修改后调用

/* 全局实例，业务代码直接读写 */
extern ParamBlock_t param;

#endif //FLASH_PARAM_H

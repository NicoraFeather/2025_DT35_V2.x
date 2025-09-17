//
// Created by lak19 on 2025/9/13.
//

#include "../Inc/usr_flash.h"

#include <string.h>
extern uint16_t can_id[4]; // 4 路 DT35 的 CAN ID
extern float calib_k[4]; // 每个 DT35 的线性系数
extern float calib_b[4]; // 每个 DT35 的线性系数
/**
 * 从指定地址读取浮点数组
 * @param start_addr 开始地址
 * @param float_array 浮点数组地址
 * @param array_size 数组大小
 */
void STMFLASH_ReadFloatArray(uint32_t start_addr, float* float_array, int array_size)
{
    for (int i = 0; i < array_size; i++) {
        // 计算每个float的起始地址
        uint32_t word_addr = start_addr + (i * sizeof(float));
        // 读取32位数据
        uint32_t word = STMFLASH_ReadWord(word_addr);
        // 将读取的32位数据转换为float
        float_array[i] = *(float*)&word;
    }
}

/**
 * 从指定地址读取uint16_t数组
 * @param start_addr 开始地址
 * @param uint16_array uint16_t数组地址
 * @param array_size 数组大小
 */
void STMFLASH_ReadUint16Array(uint32_t start_addr, uint16_t* uint16_array, int array_size) {
    for (int i = 0; i < array_size; i++) {
        // 计算每个uint16_t的起始地址
        uint32_t word_addr = start_addr + (i * sizeof(uint16_t));
        // 读取32位数据
        uint32_t word = STMFLASH_ReadWord(word_addr);
        // 将读取的32位数据转换为uint16_t
        // 注意：由于STMFLASH_ReadWord读取的是32位数据，而uint16_t是16位，需要正确处理字节序
        uint16_array[i] = (uint16_t)(word & 0xFFFF); // 取低16位
    }
}

/**
 * 将浮点数组写入Flash
 * @param start_addr 开始地址
 * @param float_array 待写入的浮点数组地址
 * @param array_size 数组元素个数
 */
void WriteFloatArrayToFlash(uint32_t start_addr, float* float_array, int array_size) {
    uint8_t buffer[array_size * sizeof(float)];
    memcpy(buffer, float_array, sizeof(buffer)); // 将float数组转换为uint8_t数组
    WriteFlashData(start_addr, buffer, sizeof(buffer)); // 写入Flash
}

/**
 * 将uint16_t数组写入Flash
 * @param start_addr 开始地址
 * @param uint16_array 待写入的uint16_t数组地址
 * @param array_size 数组大小
 */
void WriteUint16ArrayToFlash(uint32_t start_addr, uint16_t* uint16_array, int array_size) {
    uint8_t buffer[array_size * sizeof(uint16_t)];
    memcpy(buffer, uint16_array, sizeof(buffer)); // 将uint16_t数组转换为uint8_t数组
    WriteFlashData(start_addr, buffer, sizeof(buffer)); // 写入Flash
}

/**
 * DT35 参数从 Flash 读取
 */
void DT35_Flash_Init()
{
    // 从Flash读取参数
    STMFLASH_ReadFloatArray(COEFFICIENT_ADDR, calib_k, 4);
    STMFLASH_ReadFloatArray(COEFFICIENT_ADDR + 30, calib_b, 4);
    STMFLASH_ReadUint16Array(CAN_ID_ADDR, can_id, 4);
    HAL_Delay(10);
}

/**
 * DT35 参数更新到 Flash
 * @note 包含延时，调用后需等待
 */
void DT35_Flash_Update()
{
    //float test[4] = {0.002f, 0.002f, 0.101f, 0.101f};
    WriteFloatArrayToFlash(COEFFICIENT_ADDR, calib_k, 4);
    HAL_Delay(2000); // 擦除整个扇区的时间
    WriteFloatArrayToFlash(COEFFICIENT_ADDR + 30, calib_b, 4);
    HAL_Delay(100);  //由于已经擦除，写入时间很短
    WriteUint16ArrayToFlash(CAN_ID_ADDR, can_id, 4); //写入CAN ID
    HAL_Delay(100);  //由于已经擦除，写入时间很短
}
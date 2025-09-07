//
// Created by lak19 on 2025/9/7.
//

#ifndef CAN_FIFO_H
#define CAN_FIFO_H

#include <stdint.h>
#include "stm32f4xx.h"

#define FIFO_SIZE 32 //循环队列大小

// CAN 帧结构体
typedef struct {
    uint16_t can_id;   /* 100..103 */
    uint8_t  data[8];  /* 4 字节 float + 4 字节保留 */
} CanFrame_t;

/* 环形 FIFO，主循环消费，定时器中断生产 */
typedef struct {
    CanFrame_t frame[FIFO_SIZE];
    uint8_t head;
    uint8_t tail;
} CanFifo_t;

uint8_t fifo_get(CanFrame_t *f);
uint8_t fifo_put(const CanFrame_t *f);
void can_send_from_fifo();
#endif //CAN_FIFO_H

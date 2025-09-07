//
// Created by lak19 on 2025/9/7.
//

#include "can_fifo.h"

#include "can.h"

CanFifo_t can_fifo = {0};

/**
 * 入队函数
 * @param f can帧指针
 * @return 1表示成功入队
 */
uint8_t fifo_put(const CanFrame_t *f)
{
    uint8_t next = (can_fifo.head + 1) % FIFO_SIZE;
    if (next == can_fifo.tail) return 0; /* full */
    can_fifo.frame[can_fifo.head] = *f;
    can_fifo.head = next;
    return 1;
}

/**
 * 出队函数
 * @param f can帧指针
 * @return 1表示成功出队
 */
uint8_t fifo_get(CanFrame_t *f)
{
    if (can_fifo.head == can_fifo.tail) return 0; /* empty */
    *f = can_fifo.frame[can_fifo.tail];
    can_fifo.tail = (can_fifo.tail + 1) % FIFO_SIZE;
    return 1;
}

/**
 * 在主循环中调用，尝试从FIFO发送CAN报文，发送失败就跳过，自带延时
 */
void can_send_from_fifo()
{
    CanFrame_t f;
    for (uint8_t i = 0; i < 3; ++i)
    {
        if (!fifo_get(&f)) break;
        if (CAN_SendMessage(&hcan1, f.can_id, f.data, 8) != CAN_TX_OK)
        {
            /* 头插回去，下轮再发 */
            uint8_t prev = (can_fifo.head + FIFO_SIZE - 1) % FIFO_SIZE;
            if (prev != can_fifo.tail) {
                can_fifo.head = prev;
                can_fifo.frame[prev] = f;
            }
            break;   /* 本轮不再尝试 */
        }
    }
    HAL_Delay(1);
}
//
// Created by lak19 on 2025/9/25.
//

#ifndef CAN_FIFO_H
#define CAN_FIFO_H

#include "main.h"
#include "can.h"

/* 软件 FIFO 深度，大于 4 即可，这里给 8 */
#define CAN_SW_TX_FIFO_SIZE 8

typedef struct
{
    uint16_t id; //CAN ID
    uint8_t len; //数据长度
    uint8_t data[8]; //数组数据
} CanSwTxMsg_t; //CAN报文结构体

typedef struct
{
    CanSwTxMsg_t buf[CAN_SW_TX_FIFO_SIZE]; //CAN报文FIFO队列缓存
    uint8_t head; //队头
    uint8_t tail; //队尾
    uint8_t used; //已用空间
} CanSwTxFifo_t; //CAN报文FIFO队列结构体

uint8_t CanTxFifo_Put(uint16_t id, const uint8_t *data, uint8_t len);
void MX_CAN_StartWithIT(CAN_HandleTypeDef *hcan);//开启CAN并开启CAN中断
void CAN_SendFromFifo(CAN_HandleTypeDef *hcan);//尝试从FIFO发送CAN报文
#endif //CAN_FIFO_H

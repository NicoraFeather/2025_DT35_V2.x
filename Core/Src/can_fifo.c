//
// Created by lak19 on 2025/9/25.
//

#include "../Inc/can_fifo.h"

#include <string.h>

static CanSwTxFifo_t g_canTxFifo; // 我们的软件FIFO
static CAN_HandleTypeDef *g_hcan; // 保存句柄，中断里要用

/* 入队，返回 0 成功 */
uint8_t CanTxFifo_Put(uint16_t id, const uint8_t *data, uint8_t len)
{
    if (g_canTxFifo.used >= CAN_SW_TX_FIFO_SIZE) return 1; //如果FIFO满了就返回1，但是一般不会满
    CanSwTxMsg_t *p = &g_canTxFifo.buf[g_canTxFifo.head]; //确定头指针的位置
    p->id = id; //新入队消息的ID
    p->len = len > 8 ? 8 : len; //新入队消息的长度，不能超过8
    memcpy(p->data, data, p->len); //复制内存

    g_canTxFifo.head = (g_canTxFifo.head + 1) % CAN_SW_TX_FIFO_SIZE; //重新定义头指针位置
    __disable_irq();
    g_canTxFifo.used++; //占用的位置数目++
    __enable_irq();
    return 0;
}

/* 出队，返回 0 成功 */
static uint8_t CanTxFifo_Get(CanSwTxMsg_t *out)
{
    if (g_canTxFifo.used == 0) return 1; //如果没有数据直接返回
    *out = g_canTxFifo.buf[g_canTxFifo.tail]; //找到尾指针的位置并赋值
    g_canTxFifo.tail = (g_canTxFifo.tail + 1) % CAN_SW_TX_FIFO_SIZE; //重新定义尾指针位置
    __disable_irq();
    g_canTxFifo.used--; //占用的位置数目--
    __enable_irq();
    return 0;
}

void CAN_SendFromFifo(CAN_HandleTypeDef *hcan)
{
    CanSwTxMsg_t msg; //定义要发送的消息结构体
    while (g_canTxFifo.used && HAL_CAN_GetTxMailboxesFreeLevel(hcan)) //循环，只要FIFO有数据、邮箱有空闲就发，没有空闲就跳过
    {
        if (CanTxFifo_Get(&msg))
            break; //如果出队失败，直接返回
        CAN_SendMessage(hcan, msg.id, msg.data, msg.len); //否则就塞邮箱
    }
}

void MX_CAN_StartWithIT(CAN_HandleTypeDef *hcan) //开启CAN并开启CAN中断
{
    memset(&g_canTxFifo, 0, sizeof(g_canTxFifo));
    HAL_CAN_ActivateNotification(hcan, CAN_IT_TX_MAILBOX_EMPTY);

}

//只要任意一个发送完毕中断触发，立即塞消息
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) { CAN_SendFromFifo(hcan); }
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) { CAN_SendFromFifo(hcan); }
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) { CAN_SendFromFifo(hcan); }

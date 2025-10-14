//
// Created by lak19 on 2025/8/30.
//

#include "../Inc/callback.h"
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "can.h"
#include "can_fifo.h"
#include "../Inc/uart_command.h"
#include "stm32f4xx.h"
#include "functional.h"

extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t Com_Buff[50]; //存储串口接收数据
extern uint16_t ADC_values[4]; //0~4095的整形数值
extern uint8_t Flash_flag;

float calib_k[4] = {0.0f, 0.00115245f, 0.0f, 0.0f}; // 每个 DT35 的线性系数
float calib_b[4] = {0.0f, 0.04327176f, 0.0f, 0.0f}; // 每个 DT35 的线性系数
int i = 0;//定时器计数
uint8_t run_flag = 1;
uint8_t calibration_flag = 1;
uint8_t calib_id = 0; // 当前正在标定的 DT35 编号
uint16_t can_id[4] = {0x060, 0x061, 0x062, 0x063}; // 4 路 DT35 的 CAN ID

/**
 * 将一个float类型变量转换为一条can消息
 * @param val 待转换变量
 * @param id can消息ID
 */
void Float_PutOne(float val, uint32_t id)
{
    uint8_t buf[8] = {0};
    memcpy(buf, &val, 4);
    CanTxFifo_Put(id, buf, 8);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //10ms定时器回调函数
{

    if (htim->Instance == TIM10) //10ms定时器
    {
        i++;
        float distance[4];
        for (uint8_t j = 0; j < 4; j++)
        {
            distance[j] = calib_k[j] * ADC_values[j] + calib_b[j];
        }
        if (i >= 5)
        {
            i = 0;

            uint8_t msg[20];
            sprintf((char *) msg, "%f %f %f %f\r\n", distance[0], distance[1], distance[2], distance[3]); //距离数据

            if (calibration_flag == 1)
            {
                sprintf((char *) msg, "%d\r\n", ADC_values[calib_id]); //检测到标志位后直接覆盖数据
            }

            if (run_flag == 1)
            {
                HAL_UART_Transmit(&huart1, msg, strlen(msg),HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart3, msg, strlen(msg),HAL_MAX_DELAY);
            }
        }
        for (uint8_t j = 0; j < 4; j++)
        {
            Float_PutOne(distance[j], can_id[j]);
        }
        CAN_SendFromFifo(&hcan1); //发送can
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3)
    {
        uint8_t message[50] = {0};
        sprintf(message, ">>>%s\r\n", Com_Buff);
        HAL_UART_Transmit(huart, message, strlen(message), HAL_MAX_DELAY); //回显指令

        USART_Parse_Command(Com_Buff); //解析指令

        //重新开启DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(huart, Com_Buff, COM_BUFF_SIZE);
        __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
    }
    else if (huart->Instance == USART1)
    {
        uint8_t message[50] = {0};
        sprintf(message, ">>>%s\r\n", Com_Buff);
        HAL_UART_Transmit(huart, message, strlen(message), HAL_MAX_DELAY); //回显指令

        USART_Parse_Command(Com_Buff); //解析指令

        //重新开启DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(huart, Com_Buff, COM_BUFF_SIZE);
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    }
}

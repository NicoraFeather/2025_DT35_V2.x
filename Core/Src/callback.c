//
// Created by lak19 on 2025/8/30.
//

#include "../Inc/callback.h"
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "can.h"
#include "can_fifo.h"
#include "stm32f4xx.h"
#include "functional.h"
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t Com_Buff[50]; //存储串口接收数据
extern uint16_t ADC_values[4]; //0~4095的整形数值

float calib_k[4] = {0.00047822f, 0.00052173f, 0.0f, 0.0f}; // 每个 DT35 的系数
float calib_b[4] = {0.01875017f, 0.04893762f, 0.0f, 0.0f}; // 每个 DT35 的系数
int i = 0; //定时器计数
uint8_t run_flag = 1;
uint8_t calibration_flag = 0;
uint16_t can_id[4] = {0x60, 0x61, 0x62, 0x63}; // 4 路 DT35 的 CAN ID
float can_data[4] = {0.0f, 0.0f, 0.0f, 0.0f};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //10ms定时器回调函数
{
    if (htim->Instance == TIM10) //10ms定时器
    {
        i++;
        float distance[4];
        for (uint8_t j = 0; j < 4; j++)
        {
            distance[j] = calib_k[j] * ADC_values[j] + calib_b[j];
            can_data[j] = distance[j];
        }
        if (i >= 10)
        {
            i = 0;
            uint8_t msg[20];
            sprintf((char *) msg, "%f %f %f %f\r\n", distance[0], distance[1], distance[2], distance[3]); //距离数据

            if (calibration_flag == 1)
            {
                sprintf((char *) msg, "%d %d %d %d\r\n", ADC_values[0], ADC_values[1], ADC_values[2],
                        ADC_values[3]); //检测到标志位后直接覆盖数据
            }

            if (run_flag == 1)
            {
                //UART发送距离数据
                HAL_UART_Transmit(&huart1, msg, strlen(msg),HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart3, msg, strlen(msg),HAL_MAX_DELAY);
            }
            // CAN消息塞进队列
        }
        for (int i = 0; i < 2; i++)
        {
            uint8_t data[8] = {0};
            float_to_u8(can_data[i], data);
            CAN_SendMessage(&hcan1, can_id[i], data, 8);
        }
        //HAL_Delay(10); // 10ms 发送一次
    }
}

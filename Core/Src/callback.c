//
// Created by lak19 on 2025/8/30.
//

#include "callback.h"
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "can.h"
#include "uart_command.h"
#include "stm32f4xx.h"
#include "functional.h"

extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t Com_Buff[50];
extern uint16_t ADC_values[4];
//ADC0
extern float k_0;
extern float b_0;

//ADC1
extern float k_1;
extern float b_1;

//ADC2
extern float k_2;
extern float b_2;

//ADC3
extern float k_3;
extern float b_3;
int i = 0;
uint8_t run_flag = 0; //DT35运行标志位

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //10ms定时器回调函数
{
    if (htim->Instance == TIM10) //10ms定时器
    {
        i++;
        float distance[4];
        float script_data[4];
        for (int i = 0; i < 4; i++)
        {
            script_data[i] = MappingProp(ADC_values[i], 4095, 0, 65535, 0);
        }
        distance[0] = k_0 * script_data[0] + b_0;
        distance[1] = k_1 * script_data[1] + b_1;
        distance[2] = k_2 * script_data[2] + b_2;
        distance[3] = k_3 * script_data[3] + b_3;
        if (i >= 10)
        {
            i = 0;

            uint8_t msg[20];
            sprintf((char *) msg, "%f %f %f %f\r\n", distance[0], distance[1], distance[2], distance[3]);
            //sprintf((char *) msg, "%d %d %d %d\r\n", ADC_values[0], ADC_values[1], ADC_values[2], ADC_values[3]);
            //sprintf((char *) msg, "%d %d %d %d\r\n", (uint32_t)script_data[0], (uint32_t)script_data[1], (uint32_t)script_data[2], (uint32_t)script_data[3]);
            //sprintf((char *) msg, "%d\r\n", (uint32_t)script_data[1]);
            //if (run_flag == 1)
            if (1)
            {
                //UART发送距离数据
                HAL_UART_Transmit(&huart1, msg, strlen(msg),HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart3, msg, strlen(msg),HAL_MAX_DELAY);
                // CAN消息发送
                // uint8_t canData[8]; // CAN数据缓冲区（标准帧最大8字节）
                // uint16_t baseID = 0x100; // 基础消息ID
                //
                // // 发送第一个距离（distance[0]）
                // memcpy(canData, &distance[0], 4); // 复制浮点数到字节数组
                // CAN_SendMessage(&hcan1, baseID, canData, 4); // 发送4字节
                //
                // // 发送第二个距离（distance[1]）
                // memcpy(canData, &distance[1], 4);
                // CAN_SendMessage(&hcan1, baseID + 1, canData, 4);
                //
                // // 发送第三个距离（distance[2]）
                // memcpy(canData, &distance[2], 4);
                // CAN_SendMessage(&hcan1, baseID + 2, canData, 4);
                //
                // // 发送第四个距离（distance[3]）
                // memcpy(canData, &distance[3], 4);
                // CAN_SendMessage(&hcan1, baseID + 3, canData, 4);
            }
        }
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
        HAL_UARTEx_ReceiveToIdle_DMA(huart, Com_Buff, 50);
        __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
    }

    else if (huart->Instance == USART1)
    {
        uint8_t message[50] = {0};
        sprintf(message, ">>>%s\r\n", Com_Buff);
        HAL_UART_Transmit(huart, message, strlen(message), HAL_MAX_DELAY); //回显指令

        USART_Parse_Command(Com_Buff); //解析指令

        //重新开启DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(huart, Com_Buff, 50);
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    }
}

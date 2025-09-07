//
// Created by lak19 on 2025/8/30.
//

#include "callback.h"
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "can.h"
#include "can_fifo.h"
#include "uart_command.h"
#include "stm32f4xx.h"
#include "functional.h"
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t Com_Buff[50]; //存储串口接收数据
extern uint16_t ADC_values[4]; //0~4095的整形数值

//0x060
float k_0 = 0.0f;
float b_0 = 0.0f;

//0x061
float k_1 = 0.00115245f;
float b_1 = 0.04327176f;

//0x062
float k_2 = 0.0f;
float b_2 = 0.0f;

//0x063
float k_3 = 0.0f;
float b_3 = 0.0f;

int i = 0;
uint8_t run_flag = 1;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //10ms定时器回调函数
{
    if (htim->Instance == TIM10) //10ms定时器
    {
        i++;
        float distance[4];
        distance[0] = k_0 * ADC_values[0] + b_0;
        distance[1] = k_1 * ADC_values[1] + b_1;
        distance[2] = k_2 * ADC_values[2] + b_2;
        distance[3] = k_3 * ADC_values[3] + b_3;
        if (i >= 10)
        {
            i = 0;

            uint8_t msg[20];
            sprintf((char *) msg, "%f %f %f %f\r\n", distance[0], distance[1], distance[2], distance[3]);  //距离数据
            //sprintf((char *) msg, "%d %d %d %d\r\n", ADC_values[0], ADC_values[1], ADC_values[2], ADC_values[3]);  //ADC数据
            //sprintf((char *) msg, "%d %d %d %d\r\n", (uint32_t)script_data[0], (uint32_t)script_data[1], (uint32_t)script_data[2], (uint32_t)script_data[3]);
            //sprintf((char *) msg, "%d\r\n", ADC_values[1]);
            if (run_flag == 1)
            {
                //UART发送距离数据
                HAL_UART_Transmit(&huart1, msg, strlen(msg),HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart3, msg, strlen(msg),HAL_MAX_DELAY);
                // CAN消息塞进队列
                for (int i = 0; i < 4; i++)
                {
                    CanFrame_t f;
                    f.can_id = 0x60 + i;
                    memcpy(&f.data[0], &distance[i], 4); /* float */
                    memset(&f.data[4], 0, 4);
                    fifo_put(&f); /* 非阻塞，失败就丢帧 */
                }
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
    } else if (huart->Instance == USART1)
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

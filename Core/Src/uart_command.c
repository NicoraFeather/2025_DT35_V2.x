//
// Created by lak19 on 2025/8/30.
//

#include "uart_command.h"

#include <stdio.h>
#include <string.h>

#include "usart.h"
extern uint8_t run_flag;

void USART_Parse_Command(char *str)
{
    if (strcmp(str, "help") == 0)
    {
        uint8_t message[128];
        sprintf(message, "施工中...\r\n");
        HAL_UART_Transmit(&huart3, message, strlen(message), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, message, strlen(message), HAL_MAX_DELAY);
    }
    else if (strcmp(str, "run") == 0)
    {
        run_flag = 1;
        uint8_t message[128];
        sprintf(message, "DT35 runs...\r\n");
        HAL_UART_Transmit(&huart3, message, strlen(message), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, message, strlen(message), HAL_MAX_DELAY);
    }
    else if (strcmp(str, "stop") == 0)
    {
        run_flag = 0;
        uint8_t message[128];
        sprintf(message, "DT35 stops\r\n");
        HAL_UART_Transmit(&huart3, message, strlen(message), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, message, strlen(message), HAL_MAX_DELAY);
    }
    else
    {
        uint8_t message[128];
        sprintf(message, "unknown command\r\n");
        HAL_UART_Transmit(&huart3, message, strlen(message), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, message, strlen(message), HAL_MAX_DELAY);
    }
    memset(str, 0, sizeof(str));
}

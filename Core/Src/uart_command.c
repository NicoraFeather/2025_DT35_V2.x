//
// Created by lak19 on 2025/8/30.
//

#include "uart_command.h"

#include <stdio.h>
#include <string.h>

#include "usart.h"

void USART_Parse_Command(char *str)
{
    if (strcmp(str, "help") == 0);
    {
        uint8_t message[128];
        sprintf(message, "施工中...\r\n");
        HAL_UART_Transmit(&huart3, message, strlen(message), HAL_MAX_DELAY);
    }
}

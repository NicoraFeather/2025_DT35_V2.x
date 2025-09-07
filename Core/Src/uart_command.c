//
// Created by lak19 on 2025/8/30.
//

#include "../Inc/uart_command.h"

#include <stdio.h>
#include <string.h>

#include "flash_param.h"
#include "usart.h"
extern uint8_t run_flag;
extern uint8_t calibration_flag;
extern uint8_t param_flag = 0;
extern uint8_t  calib_id;              // 当前正在标定的 DT35 编号
extern uint16_t can_id[4];             // 4 路 DT35 的 CAN ID
extern float calib_k[4], calib_b[4]; // 每个 DT35 的线性系数
void USART_Parse_Command(char *str)
{
    char resp[512] = {0};

    /* help -------------------------------------------------------------*/
    if (strcmp(str, "help") == 0)
    {
        const char *msg =
            "help                     -- show this help\r\n"
            "run                      -- start message upload\r\n"
            "stop                     -- stop  message upload\r\n"
            "calibration_mode <id>    -- enter linear calib mode (id 0~3)\r\n"
            "calibration_mode_off     -- exit calibration mode\r\n"
            "calibration_write <id> <k> <b> -- write k,b for id\r\n"
            "change_can_id <id0> <id1> <id2> <id3> -- set CAN IDs\r\n"
            "show                     -- display ADC0~3 CAN addr, k, b\r\n";
        strcpy(resp, msg);
    }
    /* run --------------------------------------------------------------*/
    else if (strcmp(str, "run") == 0)
    {
        run_flag = 1;
        strcpy(resp, "DT35 runs\r\n");
    }
    /* stop -------------------------------------------------------------*/
    else if (strcmp(str, "stop") == 0)
    {
        run_flag = 0;
        strcpy(resp, "DT35 stops\r\n");
    }
    /* calibration_mode <id> -------------------------------------------*/
    else if (strncmp(str, "calibration_mode ", 17) == 0)
    {
        int id;
        if (sscanf(str + 17, "%d", &id) == 1 && id >= 0 && id <= 3)
        {
            calibration_flag = 1;
            calib_id         = (uint8_t)id;
            sprintf(resp, "Enter calibration mode for DT35-%d\r\n", id);
        }
        else
            strcpy(resp, "Error: calibration_mode needs id 0~3\r\n");
    }
    /* calibration_write <id> <k> <b> ----------------------------------*/
    else if (strncmp(str, "calibration_write ", 18) == 0)
    {
        int id = 0;
        float k = 0.0f, b = 0.0f;
        if (sscanf(str + 18, "%d %f %f", &id, &k, &b) == 3 && id >= 0 && id <= 3)
        {
            calib_k[id] = k;
            calib_b[id] = b;
            param_flag = 1; // 标记参数已改，需要落盘
            sprintf(resp, "DT35-%d  k=%.4f  b=%.4f  written\r\n", id, k, b);
        }
        else
            strcpy(resp, "Error: calibration_write <id 0~3> <k> <b>\r\n");
    }
    /* change_can_id <id0> <id1> <id2> <id3> --------------------------*/
    else if (strncmp(str, "change_can_id ", 14) == 0)
    {
        int i0, i1, i2, i3;
        if (sscanf(str + 14, "%d %d %d %d", &i0, &i1, &i2, &i3) == 4)
        {
            can_id[0] = (uint16_t)i0;
            can_id[1] = (uint16_t)i1;
            can_id[2] = (uint16_t)i2;
            can_id[3] = (uint16_t)i3;
            param_flag = 1; // 标记参数已改，需要落盘
            sprintf(resp, "CAN IDs  %d  %d  %d  %d  set\r\n", i0, i1, i2, i3);
        }
        else
            strcpy(resp, "Error: change_can_id needs 4 integers\r\n");
    }
    /* 新增：关闭标定模式 -------------------------------------------*/
    else if (strcmp(str, "calibration_mode_off") == 0)
    {
        calibration_flag = 0;
        strcpy(resp, "Calibration mode OFF\r\n");
    }
    /* 展示 ADC0~ADC3 的 CAN 地址、k、b -----------------------*/
    else if (strcmp(str, "show") == 0)
    {
        /* 一次拼完再发，减少 DMA 次数 */
        int n = 0;
        n += sprintf(resp + n, "ADC  CAN   k          b\r\n");
        for (int i = 0; i < 4; i++)
        {
            n += sprintf(resp + n, "ADC%d %04X  %f  %f\r\n",
                         i, can_id[i], calib_k[i], calib_b[i]);
        }
    }
    /* unknown ----------------------------------------------------------*/
    else
    {
        strcpy(resp, "Unknown command, type 'help'\r\n");
    }

    HAL_UART_Transmit(&huart1, (uint8_t *)resp, strlen(resp), HAL_MAX_DELAY);

    /* 清空接收缓冲区 */
    memset(str, 0, 128);
}

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  /* 配置无过滤的CAN过滤器，所有CAN报文都被接收到FIFO0 */
  CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 0;//过滤器编号
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;//过滤器模式
  sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;//标识符位宽度
  sFilterConfig.FilterIdHigh = 0x0000;//过滤器高位标识符
  sFilterConfig.FilterIdLow = 0x0000;//过滤器低位标识符
  sFilterConfig.FilterMaskIdHigh = 0x0000;//过滤器高位标识符掩码
  sFilterConfig.FilterMaskIdLow = 0x0000;//过滤器低位标识符掩码
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;//过滤器匹配的 CAN 帧将被分配到的 FIFO0
  sFilterConfig.FilterActivation = ENABLE;//过滤器启用使能
  sFilterConfig.SlaveStartFilterBank = 14;//选择从属 CAN 实例的起始过滤器组
  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* 启动CAN */
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* 使能CAN的FIFO0接收通知（中断） */
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


CanTxResult_e CAN_SendMessage(CAN_HandleTypeDef *hcan,
                              uint16_t ID,
                              const uint8_t *DataBuff,
                              uint8_t Len)
{
  if (Len > 8) return CAN_TX_FAIL;

  CAN_TxHeaderTypeDef txHeader = {
    .StdId = ID,
    .IDE   = CAN_ID_STD,
    .RTR   = CAN_RTR_DATA,
    .DLC   = Len
};

  uint32_t txMailbox;
  /* 尝试拿一个空邮箱，非阻塞 */
  if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0)
    return CAN_TX_BUSY;

  /* 真正投递，如果返回HAL_OK说明硬件已接管 */
  if (HAL_CAN_AddTxMessage(hcan, &txHeader, (uint8_t *)DataBuff, &txMailbox) != HAL_OK)
    return CAN_TX_FAIL;

  return CAN_TX_OK;
}
/* USER CODE END 1 */

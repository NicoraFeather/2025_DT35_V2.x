//
// Created by lak19 on 2025/8/30.
//

#include "../Inc/adc_scan.h"

uint16_t ADC_values[4]={0};

void ADC_Init(void)
{
    //与F103不同，F405的校准功能由硬件执行
    HAL_ADC_Start_DMA(&hadc1, ADC_values, 4);
}
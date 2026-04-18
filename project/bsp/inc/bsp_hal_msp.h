#ifndef __BSP_HAL_MSP_H
#define __BSP_HAL_MSP_H

#include "stm32f1xx_hal.h"

void HAL_MspInit(void);
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);

#endif

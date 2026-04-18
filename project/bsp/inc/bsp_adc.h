#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f1xx_hal.h"

#ifndef ADC_AVG_SAMPLES
#define ADC_AVG_SAMPLES 16
#endif

HAL_StatusTypeDef BSP_ADC_Init(void);
HAL_StatusTypeDef BSP_ADC_ReadAvg(uint16_t *out);

#endif

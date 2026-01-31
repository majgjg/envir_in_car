#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f1xx_hal.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

void adc_init(void);

#endif

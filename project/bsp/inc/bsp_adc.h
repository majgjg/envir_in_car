#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f1xx_hal.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

#ifndef ADC_BUF_LEN
#define ADC_BUF_LEN 128
#endif

extern uint32_t adc_buf[ADC_BUF_LEN];

HAL_StatusTypeDef BSP_ADC_Dual_Start(void);

void BSP_ADC_Dual_Stop(void);

uint8_t BSP_ADC_Dual_GetLatest(uint16_t* adc1 , uint16_t* adc2);


void adc_init(void);

#endif

#ifndef __TELEMETRY_H
#define __TELEMETRY_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

void TEL_Init(UART_HandleTypeDef *huart);
void TEL_SendLine(uint32_t ts, uint16_t adc_raw, uint16_t adc_filt, uint8_t idx, uint8_t alarm);
const char *TEL_GetLastLine(void);

#endif

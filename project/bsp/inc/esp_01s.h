#ifndef __ESP_01_H
#define __ESP_01_H

#include "stm32f1xx_hal.h"
#include "stdint.h"

typedef enum{
	ESP_OK = 0,
	ESP_ERR,
	ESP_TIMEOUT,
}esp_status_t;

void ESP_Init(UART_HandleTypeDef *huart);
void ESP_Task(void);

esp_status_t ESP_SendLine(const char* line);	

#endif


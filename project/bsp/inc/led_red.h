#ifndef __LED_RED_H
#define __LED_RED_H

#include "stm32f1xx_hal.h"

#ifndef LEDR_GPIO_PORT
#define LEDR_GPIO_PORT GPIOB
#endif

#ifndef LEDR_GPIO_CLK_ENABLE
#define LEDR_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#endif

#ifndef LEDR_PIN
#define LEDR_PIN GPIO_PIN_5
#endif

#ifndef LEDR_ACTIVE_LEVEL
#define LEDR_ACTIVE_LEVEL GPIO_PIN_SET
#endif

void LEDR_Init(void);
void LEDR_Set(uint8_t on);

#endif

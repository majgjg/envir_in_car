#ifndef __BSP_DMA_H
#define __BSP_DMA_H

#include "stm32f1xx_hal.h"

extern DMA_HandleTypeDef hdma_adc1;

void dma_init(void);

#endif

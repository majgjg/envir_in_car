#include "stm32f1xx_it.h"
#include "bsp_dma.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_adc1);
}


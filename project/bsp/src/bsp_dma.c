#include "bsp_dma.h"

DMA_HandleTypeDef hdma_adc1;

void dma_init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment =  DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;

    if(HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
        while(1){}
    }
    
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

#include "bsp_adc.h"
#include "bsp_dma.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

static void adc1_init(void);
static void adc2_init(void);
static void adc_dualmode_init(void);

void adc_init(void)
{
    adc1_init();
    adc2_init();
    adc_dualmode_init();
}

static void adc1_init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE; 
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;

    if(HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        while(1){}
    }

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
	ADC_ChannelConfTypeDef ch = {0};
	ch.Channel = ADC_CHANNEL_0;
	ch.Rank = ADC_REGULAR_RANK_1;
	ch.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	
	if (HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK)
    {
        while(1){}
    }

}

static void adc2_init(void)
{
	__HAL_RCC_ADC2_CLK_ENABLE();

	hadc2.Instance = ADC2;
    hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc2.Init.ContinuousConvMode = DISABLE;
    hadc2.Init.DiscontinuousConvMode = DISABLE;
    hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc2.Init.NbrOfConversion = 1;

    if (HAL_ADC_Init(&hadc2) != HAL_OK)
    {
        while (1) {}
    }

    ADC_ChannelConfTypeDef ch = {0};
    ch.Channel = ADC_CHANNEL_1;                      
    ch.Rank = ADC_REGULAR_RANK_1;
    ch.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc2, &ch) != HAL_OK)
    {
        while (1) {}
    }
}

static void adc_dualmode_init(void)
{
    ADC_MultiModeTypeDef multimode = {0};

	multimode.Mode = ADC_DUALMODE_REGSIMULT;

	if(HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		while(1){}
	}
}

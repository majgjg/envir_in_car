#include "bsp_adc.h"
#include "bsp_dma.h"
#include "bsp_tim.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

uint32_t adc_buf[ADC_BUF_LEN];

static volatile uint16_t adc1_latest = 0; 
static volatile uint16_t adc2_latest = 0; 
static volatile uint8_t  has_new_data = 0;

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

HAL_StatusTypeDef BSP_ADC_Dual_Start(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) return HAL_ERROR;
    if (HAL_ADCEx_Calibration_Start(&hadc2) != HAL_OK) return HAL_ERROR;

	if (HAL_ADC_Start(&hadc2) != HAL_OK) return HAL_ERROR;

	if (HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUF_LEN) != HAL_OK) return HAL_ERROR;
	
	if (HAL_TIM_Base_Start(&htim3) != HAL_OK) return HAL_ERROR;
	
	return HAL_OK;
}

void BSP_ADC_Dual_Stop(void)
{
    HAL_TIM_Base_Stop(&htim3);
    HAL_ADC_Stop(&hadc2);
    HAL_ADCEx_MultiModeStop_DMA(&hadc1);
}

uint8_t BSP_ADC_Dual_GetLatest(uint16_t *adc1,uint16_t *adc2)
{
	if (!adc1 | !adc2) return 0;

	if (has_new_data == 0) return 0;

    *adc1 = adc1_latest;
    *adc2 = adc2_latest;

	has_new_data = 0;
	
	return 1;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance != ADC1) return;

	uint32_t v = adc_buf[(ADC_BUF_LEN / 2) - 1];

	adc1_latest = (uint16_t)(v & 0xFFFF);
	adc2_latest = (uint16_t)(v >> 16);
	
	has_new_data = 1;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance != ADC1) return;

	uint32_t v = adc_buf[ADC_BUF_LEN - 1];
	
	adc1_latest = (uint16_t)(v & 0xFFFF);
	adc2_latest = (uint16_t)(v >> 16);

	has_new_data = 1;
}













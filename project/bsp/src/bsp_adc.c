#include "bsp_adc.h"

ADC_HandleTypeDef hadc1;

static HAL_StatusTypeDef adc1_single_init(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE();

	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;

	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		return HAL_ERROR;
	}

	ADC_ChannelConfTypeDef ch = {0};
	ch.Channel = ADC_CHANNEL_0;
	ch.Rank = ADC_REGULAR_RANK_1;
	ch.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;

	if (HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef BSP_ADC_Init(void)
{
	/* 单路 ADC 初始化 + 校准 */
	if (adc1_single_init() != HAL_OK) return HAL_ERROR;
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef BSP_ADC_ReadAvg(uint16_t *out)
{
	uint32_t sum = 0;
	uint32_t i;

	if (!out) return HAL_ERROR;

	/* N 次采样取平均 */
	for (i = 0; i < ADC_AVG_SAMPLES; i++)
	{
		if (HAL_ADC_Start(&hadc1) != HAL_OK) return HAL_ERROR;
		if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) return HAL_ERROR;
		sum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}

	*out = (uint16_t)(sum / ADC_AVG_SAMPLES);
	return HAL_OK;
}













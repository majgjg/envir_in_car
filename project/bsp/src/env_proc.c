#include "env_proc.h"

static uint16_t filter_buf[ENV_FILTER_WINDOW];
static uint32_t filter_sum = 0;
static uint8_t filter_idx = 0;
static uint8_t filter_count = 0;

static uint8_t alarm_state_latched = 0;

void ENV_Init(void)
{
	filter_sum = 0;
	filter_idx = 0;
	filter_count = 0;

	alarm_state_latched = 0;
}

void ENV_Update(uint16_t adc_raw, uint16_t *adc_filt, uint8_t *idx, uint8_t *alarm_state)
{
	uint16_t filt;
	uint8_t out_idx;

	/* 滑动平均滤波 */
	if (filter_count < ENV_FILTER_WINDOW)
	{
		filter_buf[filter_idx] = adc_raw;
		filter_sum += adc_raw;
		filter_count++;
	}
	else
	{
		filter_sum -= filter_buf[filter_idx];
		filter_buf[filter_idx] = adc_raw;
		filter_sum += adc_raw;
	}

	filter_idx++;
	if (filter_idx >= ENV_FILTER_WINDOW) filter_idx = 0;

	filt = (uint16_t)(filter_sum / (filter_count ? filter_count : 1));

	/* 依据滤波值分级，并生成报警状态 */
	if (filt < ENV_ADC_LEVEL_GOOD_MAX)
	{
		out_idx = ENV_IDX_GOOD;
	}
	else if (filt <= ENV_ADC_LEVEL_WARN_MAX)
	{
		out_idx = ENV_IDX_WARN;
	}
	else
	{
		out_idx = ENV_IDX_BAD;
	}

	/* 报警滞回：避免频繁抖动 */
	if (alarm_state_latched == 0)
	{
		if (filt > ENV_ADC_LEVEL_WARN_MAX) alarm_state_latched = 1;
	}
	else
	{
		if (filt <= (ENV_ADC_LEVEL_WARN_MAX - ENV_ALARM_HYST)) alarm_state_latched = 0;
	}

	if (adc_filt) *adc_filt = filt;
	if (idx) *idx = out_idx;
	if (alarm_state) *alarm_state = alarm_state_latched;
}

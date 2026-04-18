#include "app.h"
#include "bsp_adc.h"
#include "env_proc.h"
#include "buzzer.h"
#include "telemetry.h"
#include "led_red.h"

static uint32_t ts_counter = 0;
static uint8_t last_alarm = 0;

void APP_Init(void)
{
	/* 初始化环境处理与蜂鸣器/红灯 */
	ENV_Init();
	BUZ_Init();
	LEDR_Init();
}

void APP_Task_1Hz(void)
{
	uint16_t adc_raw = 0;
	uint16_t adc_filt = 0;
	uint8_t idx = 0;
	uint8_t alarm = 0;

	/* 采样：16次平均 */
	if (BSP_ADC_ReadAvg(&adc_raw) != HAL_OK)
	{
		adc_raw = 0;
	}

	/* 处理：滤波 + 分级 + 报警状态 */
	ENV_Update(adc_raw, &adc_filt, &idx, &alarm);

	/* 报警上升沿触发三声 */
	if (alarm && !last_alarm)
	{
		BUZ_Trigger3Beep();
	}
	last_alarm = alarm;

	/* 红灯：超过阈值常亮 */
	LEDR_Set((adc_filt > ENV_ADC_LEVEL_WARN_MAX) ? 1u : 0u);

	/* 串口上报 */
	TEL_SendLine(ts_counter, adc_raw, adc_filt, idx, alarm);
	ts_counter++;
}

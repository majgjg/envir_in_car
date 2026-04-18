#include "led_red.h"

static void led_off(void)
{
	HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_PIN,
		(LEDR_ACTIVE_LEVEL == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void LEDR_Init(void)
{
	GPIO_InitTypeDef gpio = {0};

	/* 红灯引脚初始化 */
	LEDR_GPIO_CLK_ENABLE();

	gpio.Pin = LEDR_PIN;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LEDR_GPIO_PORT, &gpio);

	led_off();
}

void LEDR_Set(uint8_t on)
{
	if (on)
	{
		/* 亮灯 */
		HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_PIN, LEDR_ACTIVE_LEVEL);
	}
	else
	{
		/* 灭灯 */
		led_off();
	}
}

#include "buzzer.h"

static volatile uint8_t buz_active = 0;
static volatile uint8_t buz_phase_on = 0;
static volatile uint8_t buz_left = 0;
static volatile uint16_t buz_timer = 0;

static void buz_on(void)
{
	HAL_GPIO_WritePin(BUZ_GPIO_PORT, BUZ_PIN, BUZ_ACTIVE_LEVEL);
}

static void buz_off(void)
{
	HAL_GPIO_WritePin(BUZ_GPIO_PORT, BUZ_PIN,
		(BUZ_ACTIVE_LEVEL == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void BUZ_Init(void)
{
	GPIO_InitTypeDef gpio = {0};

	/* 蜂鸣器引脚初始化（有源） */
	BUZ_GPIO_CLK_ENABLE();

	gpio.Pin = BUZ_PIN;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BUZ_GPIO_PORT, &gpio);

	buz_off();
	buz_active = 0;
	buz_phase_on = 0;
	buz_left = 0;
	buz_timer = 0;
}

void BUZ_Trigger3Beep(void)
{
	if (buz_active) return;

	/* 启动三声：ON/OFF 交替 */
	buz_active = 1;
	buz_phase_on = 1;
	buz_left = BUZ_BEEP_COUNT;
	buz_timer = BUZ_ON_MS;
	buz_on();
}

void BUZ_Tick_1ms(void)
{
	if (!buz_active) return;

	/* 1ms 节拍驱动状态机 */
	if (buz_timer > 0) buz_timer--;
	if (buz_timer != 0) return;

	if (buz_phase_on)
	{
		buz_off();
		buz_phase_on = 0;
		buz_timer = BUZ_OFF_MS;
	}
	else
	{
		if (buz_left > 0) buz_left--;
		if (buz_left == 0)
		{
			buz_active = 0;
			return;
		}
		buz_phase_on = 1;
		buz_timer = BUZ_ON_MS;
		buz_on();
	}
}

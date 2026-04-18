#include <main.h>
#include "bsp_clock.h"
#include "gpio.h"
#include "bsp_adc.h"
#include "bsp_usart.h"
#include "telemetry.h"
#include "app.h"
#include "esp_01s.h"



int main()
{
	HAL_Init();
	SYSTEM_CLOCK_HSE8M_PLL72M();

	/* 外设初始化 */
	gpio_init();
	usart1_Init(); 

	/* 业务模块初始化 */
	BSP_ADC_Init();
	TEL_Init(&huart1);
	APP_Init();

	/* ESP AT 状态机（如暂不使用可注释） */
	ESP_Init(&huart1);


	/* 1Hz 调度 */
	uint32_t last_1hz = HAL_GetTick();
	while (1)
	{
		/* 维持 ESP 状态机 */
		ESP_Task();
		uint32_t now = HAL_GetTick();
		while ((now - last_1hz) >= 1000u)
		{
			last_1hz += 1000u;
			APP_Task_1Hz();
		}
	}

}

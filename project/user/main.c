#include <stdio.h>
#include <main.h>
#include "bsp_clock.h"
#include "gpio.h"
#include "bsp_dma.h"
#include "bsp_tim.h"
#include "bsp_adc.h"


int main()
{
	HAL_Init();
	SYSTEM_CLOCK_HSE8M_PLL72M();
	gpio_init();
	dma_init();
	tim3_init();
	adc_init();

	while(1)
	{

	}

}

#include <stdio.h>
#include <main.h>
#include "bsp_clock.h"
#include "gpio.h"


int main()
{
	HAL_Init();
	SYSTEM_CLOCK_HSE8M_PLL72M();
	
	

	return 0;
}
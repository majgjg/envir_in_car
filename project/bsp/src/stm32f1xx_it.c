#include "stm32f1xx_it.h"
#include "bsp_usart.h"
#include "buzzer.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	BUZ_Tick_1ms();
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}


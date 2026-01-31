#include "bsp_tim.h"

TIM_HandleTypeDef htim3;

void tim3_init(void)
{
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 71;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if(HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
      while(1){}
    }

    TIM_MasterConfigTypeDef master = {0};
    master.MasterOutputTrigger = TIM_TRGO_UPDATE;
    master.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if(HAL_TIMEx_MasterConfigSynchronization(&htim3, &master) != HAL_OK)
    {
      while(1){}
    }
    
}

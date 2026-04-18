#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f1xx_hal.h"

#ifndef BUZ_GPIO_PORT
#define BUZ_GPIO_PORT GPIOB
#endif

#ifndef BUZ_GPIO_CLK_ENABLE
#define BUZ_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#endif

#ifndef BUZ_PIN
#define BUZ_PIN GPIO_PIN_12
#endif

#ifndef BUZ_ACTIVE_LEVEL
#define BUZ_ACTIVE_LEVEL GPIO_PIN_SET
#endif

#ifndef BUZ_ON_MS
#define BUZ_ON_MS 200
#endif

#ifndef BUZ_OFF_MS
#define BUZ_OFF_MS 200
#endif

#ifndef BUZ_BEEP_COUNT
#define BUZ_BEEP_COUNT 3
#endif

void BUZ_Init(void);
void BUZ_Trigger3Beep(void);
void BUZ_Tick_1ms(void);

#endif

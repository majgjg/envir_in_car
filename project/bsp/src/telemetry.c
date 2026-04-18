#include "telemetry.h"
#include "stdio.h"

static UART_HandleTypeDef *tel_uart = NULL;
static char tel_last_line[80];

void TEL_Init(UART_HandleTypeDef *huart)
{
	tel_uart = huart;
	tel_last_line[0] = '\0';
}

void TEL_SendLine(uint32_t ts, uint16_t adc_raw, uint16_t adc_filt, uint8_t idx, uint8_t alarm)
{
	/* CSV 格式输出：T,<ts>,<raw>,<filt>,<idx>,<alarm> */
	int n = snprintf(tel_last_line, sizeof(tel_last_line), "T,%lu,%u,%u,%u,%u\n",
		(unsigned long)ts,
		(unsigned)adc_raw,
		(unsigned)adc_filt,
		(unsigned)idx,
		(unsigned)alarm);

	(void)n;
}

const char *TEL_GetLastLine(void)
{
	return tel_last_line;
}

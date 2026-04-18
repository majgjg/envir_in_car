#ifndef __ENV_PROC_H
#define __ENV_PROC_H

#include "stdint.h"

#ifndef ENV_BASELINE_SAMPLES
#define ENV_BASELINE_SAMPLES 10
#endif

#ifndef ENV_FILTER_WINDOW
#define ENV_FILTER_WINDOW 5
#endif

#ifndef ENV_ALARM_THRESHOLD
#define ENV_ALARM_THRESHOLD 20
#endif

#ifndef ENV_ALARM_HYST
#define ENV_ALARM_HYST 5
#endif

/* Level thresholds based on filtered ADC value */
#ifndef ENV_ADC_LEVEL_GOOD_MAX
#define ENV_ADC_LEVEL_GOOD_MAX 1000
#endif

#ifndef ENV_ADC_LEVEL_WARN_MAX
#define ENV_ADC_LEVEL_WARN_MAX 1100
#endif

/* Mapped index values for reporting */
#ifndef ENV_IDX_GOOD
#define ENV_IDX_GOOD 20
#endif

#ifndef ENV_IDX_WARN
#define ENV_IDX_WARN 60
#endif

#ifndef ENV_IDX_BAD
#define ENV_IDX_BAD 90
#endif

void ENV_Init(void);
void ENV_Update(uint16_t adc_raw, uint16_t *adc_filt, uint8_t *idx, uint8_t *alarm_state);

#endif

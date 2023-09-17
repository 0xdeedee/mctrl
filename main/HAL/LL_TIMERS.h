#ifndef LL_TIMERS_H
#define LL_TIMERS_H

#include "driver/mcpwm_prelude.h"

#define LL_TIMERS_TIMEDIFF(a,b)   (((a) >= (b)) ? ((a)-(b)) : (((a)+((b)^0xFFFFFFFF)+1)))		// absolute unsigned difference; immune to overflows; use for u32_t only!

extern void LL_TIMERS_Init(void);
extern esp_err_t LL_TIMERS_SetDutyPhA(uint16_t duty);
extern esp_err_t LL_TIMERS_SetDutyPhB(uint16_t duty);
extern esp_err_t LL_TIMERS_SetDutyPhC(uint16_t duty);

#endif


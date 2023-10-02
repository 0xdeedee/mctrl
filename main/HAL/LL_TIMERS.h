#ifndef LL_TIMERS_H
#define LL_TIMERS_H

#include "driver/mcpwm_prelude.h"

// absolute unsigned difference; immune to overflows; use for u32_t only!
#define LL_TIMERS_TIMEDIFF(a,b)   (((a) >= (b)) ? ((a)-(b)) : (((a)+((b)^0xFFFFFFFF)+1)))

extern void LL_TIMERS_Init(void);

#define MCPWM_IDX_PHASE_A				( 0 )
#define MCPWM_IDX_PHASE_B				( 1 )
#define MCPWM_IDX_PHASE_C				( 2 )


esp_err_t ll_timers_set_duty( uint8_t idx, uint16_t duty );

#define LL_TIMERS_SetDutyPhA( duty ) { ll_timers_set_duty( MCPWM_IDX_PHASE_A, duty ); }
#define LL_TIMERS_SetDutyPhB( duty ) { ll_timers_set_duty( MCPWM_IDX_PHASE_B, duty ); }
#define LL_TIMERS_SetDutyPhC( duty ) { ll_timers_set_duty( MCPWM_IDX_PHASE_C, duty ); }


#endif


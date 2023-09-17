#include "LL_ISR.h"

uint32_t LL_ISR_SysTicks = 0u;

/**
  * @brief none
  * @param none
  * @retval none
  */
void LL_ISR_SysTick(void * ptr)
{
	LL_ISR_SysTicks++;
}



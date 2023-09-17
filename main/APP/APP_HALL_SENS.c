#include "APP_HALL_SENS.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"
#include "BOARD_CFG.h"
#include "LL_ISR.h"
#include "LL_TIMERS.h"

#define HALL_STATE_DEB_TIME		10u
#define SECTOR_1				0u
#define SECTOR_2				1u
#define SECTOR_3				2u
#define SECTOR_4				3u
#define SECTOR_5				4u
#define SECTOR_6				5u
#define INVALID_SECTOR			0xFFu

typedef struct HallData_t
{
  uint32_t Ticks;
  uint8_t GpioState;
  uint8_t OldGpioState;
  uint8_t CurrSector;
  uint8_t OldSector;
  uint8_t SecTransOccurred;
  uint8_t Initialized;
}HallData_t;

static HallData_t HallData;

static void HallSensUpd(void);

/**
  * @brief
  * @param none
  * @retval none
  */
void APP_HALL_SENS_Init(void)
{
	HallData.Ticks = 0u;
    HallData.GpioState = 0u;
    HallData.OldGpioState = 0u;
    HallData.CurrSector = SECTOR_1;
    HallData.OldSector = SECTOR_1;
    HallData.SecTransOccurred = 0u;
    HallData.Initialized = 1u;
}

/**
  * @brief
  * @param none
  * @retval none
  */
void APP_HALL_SENS_Task(void)
{
	if (HallData.Initialized == 1u)
	{
		HallSensUpd();
	}
	else
	{
		/* First initialization function must be called */
	}
}

/**
  * @brief
  * @param none
  * @retval none
  */
static void HallSensUpd(void)
{
	HallData.GpioState = ((gpio_get_level(BOARD_CFG_GPIO_HALL_CAP_U) << 2u) |\
					(gpio_get_level(BOARD_CFG_GPIO_HALL_CAP_V) << 1u) |\
					 gpio_get_level(BOARD_CFG_GPIO_HALL_CAP_W));

	if (HallData.GpioState != HallData.OldGpioState)
	{
		HallData.OldGpioState = HallData.GpioState;
		HallData.Ticks = LL_ISR_SysTicks;
	}

    switch(HallData.GpioState)
    {
    	case 0x01u:
    		HallData.CurrSector = SECTOR_6;
    		if (HallData.CurrSector != HallData.OldSector)
			{
    			if (HALL_STATE_DEB_TIME <= (LL_TIMERS_TIMEDIFF(LL_ISR_SysTicks, HallData.Ticks)))
    			{
					HallData.OldSector = HallData.CurrSector;
					HallData.SecTransOccurred = 1u;
					printf("HallData.CurrSector:%d\n", HallData.CurrSector);
    			}
			}
    	break;
    	case 0x03u:
    		HallData.CurrSector = SECTOR_5;
    		if (HallData.CurrSector != HallData.OldSector)
			{
    			if (HALL_STATE_DEB_TIME <= (LL_TIMERS_TIMEDIFF(LL_ISR_SysTicks, HallData.Ticks)))
    			{
					HallData.OldSector = HallData.CurrSector;
					HallData.SecTransOccurred = 1u;
					printf("HallData.CurrSector:%d\n", HallData.CurrSector);
    			}
			}
    	break;
    	case 0x02u:
    		HallData.CurrSector = SECTOR_4;
    		if (HallData.CurrSector != HallData.OldSector)
			{
    			if (HALL_STATE_DEB_TIME <= (LL_TIMERS_TIMEDIFF(LL_ISR_SysTicks, HallData.Ticks)))
    			{
					HallData.OldSector = HallData.CurrSector;
					HallData.SecTransOccurred = 1u;
					printf("HallData.CurrSector:%d\n", HallData.CurrSector);
    			}
			}
    	break;
    	case 0x06u:
			HallData.CurrSector = SECTOR_3;
			if (HallData.CurrSector != HallData.OldSector)
			{
				if (HALL_STATE_DEB_TIME <= (LL_TIMERS_TIMEDIFF(LL_ISR_SysTicks, HallData.Ticks)))
				{
					HallData.OldSector = HallData.CurrSector;
					HallData.SecTransOccurred = 1u;
					printf("HallData.CurrSector:%d\n", HallData.CurrSector);
				}
			}
		break;
    	case 0x04u:
    		HallData.CurrSector = SECTOR_2;
    		if (HallData.CurrSector != HallData.OldSector)
			{
    			if (HALL_STATE_DEB_TIME <= (LL_TIMERS_TIMEDIFF(LL_ISR_SysTicks, HallData.Ticks)))
    			{
					HallData.OldSector = HallData.CurrSector;
					HallData.SecTransOccurred = 1u;
					printf("HallData.CurrSector:%d\n", HallData.CurrSector);
    			}
			}
    	break;
    	case 0x05u:
    		HallData.CurrSector = SECTOR_1;
    		if (HallData.CurrSector != HallData.OldSector)
			{
    			if (HALL_STATE_DEB_TIME <= (LL_TIMERS_TIMEDIFF(LL_ISR_SysTicks, HallData.Ticks)))
    			{
					HallData.OldSector = HallData.CurrSector;
					HallData.SecTransOccurred = 1u;
					printf("HallData.CurrSector:%d\n", HallData.CurrSector);
    			}
			}
    	break;
    	default:
    		HallData.CurrSector = INVALID_SECTOR;
    	break;
    }
}

/**
  * @brief
  * @param none
  * @retval none
  */
uint8_t APP_HALL_SENS_GetSector(void)
{
  uint8_t State = INVALID_SECTOR;

  	if (HallData.Initialized == 1u)
  	{
		if (HallData.OldSector/*HallData.CurrSector*/ != INVALID_SECTOR)
		{
			//State = HallData.CurrSector;
			State = HallData.OldSector;
		}
  	}
  	else
  	{
  		/* First initialization function must be called */
  	}

	return State;
}

/**
  * @brief
  * @param none
  * @retval none
  */
uint8_t APP_HAL_SENS_IsHallTransOccurred(void)
{
  uint8_t Status = 0u;

  	if (HallData.Initialized == 1u)
    {
  		if (HallData.SecTransOccurred == 1u)
  		{
  			Status = 1u;
  			HallData.SecTransOccurred = 0u;
  		}
    }
  	else
  	{
  		/* First initialization function must be called */
  	}

    return Status;
}



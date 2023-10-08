#include "APP_HALL_SENS.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"
#include "BOARD_CFG.h"
#include "LL_ISR.h"
#include "LL_TIMERS.h"

#define HALL_STATE_DEB_TIME			10u
//#define SECTOR_1				0u
//#define SECTOR_2				1u
//#define SECTOR_3				2u
//#define SECTOR_4				3u
//#define SECTOR_5				4u
//#define SECTOR_6				5u
//#define INVALID_SECTOR				0xFFu

typedef enum{
	SECTOR_1	= 0u,
	SECTOR_2	= 1u,
	SECTOR_3	= 2u,
	SECTOR_4	= 3u,
	SECTOR_5	= 4u,
	SECTOR_6	= 5u,
	INVALID_SECTOR	= 0xFFu,
} sectors_t;



typedef struct HallData_t
{
	uint32_t	Ticks;
	uint8_t		GpioState;
	uint8_t		OldGpioState;
	uint8_t		CurrSector;
	uint8_t		OldSector;
	uint8_t		SecTransOccurred;
	uint8_t		Initialized;
}HallData_t;

static HallData_t	HallData;

static void HallSensUpd(void);

/**
  * @brief
  * @param none
  * @retval none
  */
void APP_HALL_SENS_Init(void)
{
	HallData.Ticks			= 0u;
	HallData.GpioState		= 0u;
	HallData.OldGpioState		= 0u;
	HallData.CurrSector		= SECTOR_1;
	HallData.OldSector		= SECTOR_1;
	HallData.SecTransOccurred	= 0u;
	HallData.Initialized		= 1u;
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


static inline void __hall_data_update( HallData_t *__hall_data, sectors_t sector )
{
	__hall_data->CurrSector = sector;
	if ( __hall_data->CurrSector != __hall_data->OldSector )
	{
		if ( HALL_STATE_DEB_TIME <= ( LL_TIMERS_TIMEDIFF( LL_ISR_SysTicks, __hall_data->Ticks ) ) )
		{
			__hall_data->OldSector = __hall_data->CurrSector;
			__hall_data->SecTransOccurred = 1u;
			printf( "HallData.CurrSector:%d\n", __hall_data->CurrSector );
		}
	}
}

/**
  * @brief
  * @param none
  * @retval none
  */
static void HallSensUpd(void)
{
	HallData.GpioState = (	(	gpio_get_level( BOARD_CFG_GPIO_HALL_CAP_U ) << 2u ) |	\
				(	gpio_get_level( BOARD_CFG_GPIO_HALL_CAP_V ) << 1u ) |	\
					gpio_get_level( BOARD_CFG_GPIO_HALL_CAP_W ) );

	if ( HallData.GpioState != HallData.OldGpioState )
	{
		HallData.OldGpioState = HallData.GpioState;
		HallData.Ticks = LL_ISR_SysTicks;
	}

	switch( HallData.GpioState )
	{
		case 0x01u:
    			__hall_data_update( &HallData, SECTOR_6 );
		break;

		case 0x03u:
    			__hall_data_update( &HallData, SECTOR_5 );
		break;

		case 0x02u:
			__hall_data_update( &HallData, SECTOR_4 );
		break;

		case 0x06u:
			__hall_data_update( &HallData, SECTOR_3 );
		break;

		case 0x04u:
			__hall_data_update( &HallData, SECTOR_2 );
		break;

		case 0x05u:
			__hall_data_update( &HallData, SECTOR_1 );
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
	if ( ( HallData.Initialized == 1u ) && ( HallData.OldSector != INVALID_SECTOR ) )
	{
		return HallData.OldSector;
  	}

	return INVALID_SECTOR;
}

/**
  * @brief
  * @param none
  * @retval none
  */
uint8_t APP_HAL_SENS_IsHallTransOccurred(void)
{
	if ( ( HallData.Initialized == 1u ) && ( HallData.SecTransOccurred == 1u ) )
	{
		HallData.SecTransOccurred = 0u;
		return 1u;
	}

	return 0u;
}



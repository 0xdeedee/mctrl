#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "hal/wdt_hal.h"
#include "LL_PORTS.h"
#include "LL_TIMERS.h"
#include "LL_ADC.h"
#include "APP_MOT_CTRL.h"
#include "APP_HALL_SENS.h"


void app_main(void)
{
	TaskHandle_t		__adc_task = NULL;

	__adc_task = adc_init();

	LL_PORTS_Init();
	LL_TIMERS_Init( __adc_task );
	APP_MOT_CTRL_Init();
	APP_HALL_SENS_Init();
	printf( "ESC started\n" );

	while ( true )
	{
		APP_MOT_CTRL_Task();
		APP_HALL_SENS_Task();
		vTaskDelay( 500 );
	}
}

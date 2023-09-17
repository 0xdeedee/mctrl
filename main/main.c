#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "LL_PORTS.h"
#include "LL_TIMERS.h"
#include "hal/wdt_hal.h"
#include "APP_MOT_CTRL.h"
#include "APP_HALL_SENS.h"

void app_main(void)
{
	LL_PORTS_Init();
	LL_TIMERS_Init();
	APP_MOT_CTRL_Init();
	APP_HALL_SENS_Init();
	printf("ESC started\n");

	while (true)
    {
		APP_MOT_CTRL_Task();
		APP_HALL_SENS_Task();
    }
}

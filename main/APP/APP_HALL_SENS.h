#ifndef APP_HALL_SENS_H
#define APP_HALL_SENS_H

#include <stdint.h>

extern void APP_HALL_SENS_Init(void);
extern void APP_HALL_SENS_Task(void);
extern uint8_t APP_HALL_SENS_GetSector(void);
extern uint8_t APP_HAL_SENS_IsHallTransOccurred(void);

#endif

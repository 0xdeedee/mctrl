#include "esp_timer.h"
#include "driver/mcpwm_prelude.h"
#include "LL_TIMERS.h"
#include "LL_ISR.h"
#include <stdio.h>
#include "BOARD_CFG.h"
#include "BLDC_CFG.h"

#define OUT_STATE_PWM_MODE				-1
#define MCPWM_TIMER_RESOLUTION_HZ 		31200000u /* 1 tick = 0.032us */
#define MCPWM_PERIOD              		1560u      /* 50us, 20KHz */
#define SYS_TICK_PERIOD			    	1000   /* 1ms */

#define MCPWM_GEN_IDX_HIGH 				0
#define MCPWM_GEN_IDX_LOW  				1
#define MCPWM_IDX_PHASE_A				0
#define MCPWM_IDX_PHASE_B				1
#define MCPWM_IDX_PHASE_C				2
#define MCPWM_MAX_DUTY					MCPWM_PERIOD

static mcpwm_cmpr_handle_t comparators[BLDC_CFG_NMBR_OF_PHASES];
static mcpwm_dead_time_config_t dt_config;

static void InitTimMcpwm(void);
static void InitTimSysTick(void);

/**
  * @brief
  * @param none
  * @retval none
  */
void LL_TIMERS_Init(void)
{
	InitTimMcpwm();
	InitTimSysTick();
}

/**
  * @brief Initialize motor control timer
  * @param none
  * @retval none
  */
static void InitTimMcpwm(void)
{
  mcpwm_timer_handle_t timer = NULL;
  mcpwm_timer_config_t timer_config;
  mcpwm_oper_handle_t operators[BLDC_CFG_NMBR_OF_PHASES];
  mcpwm_operator_config_t operator_config;
  mcpwm_comparator_config_t compare_config;
  mcpwm_gen_handle_t generators[BLDC_CFG_NMBR_OF_PHASES][2] = {};
  mcpwm_generator_config_t gen_config = {};

  const int gen_gpios[BLDC_CFG_NMBR_OF_PHASES][2] =
  {
	{BOARD_CFG_GPIO_PWM0A_OUT, BOARD_CFG_GPIO_PWM0B_OUT},
	{BOARD_CFG_GPIO_PWM1A_OUT, BOARD_CFG_GPIO_PWM1B_OUT},
	{BOARD_CFG_GPIO_PWM2A_OUT, BOARD_CFG_GPIO_PWM2B_OUT},
  };

	timer_config.group_id = 0;
	timer_config.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
	timer_config.resolution_hz = MCPWM_TIMER_RESOLUTION_HZ;
	timer_config.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
	timer_config.period_ticks = MCPWM_PERIOD;

	ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

	operator_config.group_id = 0;
	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operators[i]));
	}

	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators[i], timer));
	}

	compare_config.flags.update_cmp_on_tez = true;
	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		ESP_ERROR_CHECK(mcpwm_new_comparator(operators[i], &compare_config, &comparators[i]));
		ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators[i], 0));
	}

	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			gen_config.gen_gpio_num = gen_gpios[i][j];
			ESP_ERROR_CHECK(mcpwm_new_generator(operators[i], &gen_config, &generators[i][j]));
		}
	}

	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators[i][MCPWM_GEN_IDX_HIGH],
						MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
		ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generators[i][MCPWM_GEN_IDX_HIGH],
						MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators[i], MCPWM_GEN_ACTION_LOW)));
		ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(generators[i][MCPWM_GEN_IDX_HIGH],
						MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
		ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(generators[i][MCPWM_GEN_IDX_HIGH],
						MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
	}

	dt_config.posedge_delay_ticks = 5;
	/******/
	dt_config.flags.invert_output = true; /* It must be here due to DRV8300 specifics(MODE) */
	/******/
	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generators[i][MCPWM_GEN_IDX_HIGH], generators[i][MCPWM_GEN_IDX_HIGH], &dt_config));
	}
	dt_config.negedge_delay_ticks = 5;
	/*dt_config.flags.invert_output = true;*/
	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generators[i][MCPWM_GEN_IDX_HIGH], generators[i][MCPWM_GEN_IDX_LOW], &dt_config));
	}

	for (int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generators[i][j], OUT_STATE_PWM_MODE, true));
		}
	}

	ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
	ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
}

/**
  * @brief Init timer for system ticks
  * @param none
  * @retval none
  */
static void InitTimSysTick(void)
{
  esp_timer_handle_t periodic_timer = NULL;
  const esp_timer_create_args_t periodic_timer_args =
  {
    .callback = LL_ISR_SysTick,
	.arg = NULL,
  };

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, SYS_TICK_PERIOD));
}

/**
  * @brief Set duty cycle for phase A
  * @param duty[0 - 500]
  * @retval ESP_OK, ESP_FAIL
  */
esp_err_t LL_TIMERS_SetDutyPhA(uint16_t duty)
{
  esp_err_t stat = ESP_FAIL;

    if (MCPWM_MAX_DUTY >= duty)
    {
	    stat = mcpwm_comparator_set_compare_value(comparators[MCPWM_IDX_PHASE_A], duty);
    }
    else
    {
    	/* Duty cycle is outside the range */
    }

    return stat;
}

/**
  * @brief Set duty cycle for phase B
  * @param duty[0 - 500]
  * @retval ESP_OK, ESP_FAIL
  */
esp_err_t LL_TIMERS_SetDutyPhB(uint16_t duty)
{
  esp_err_t stat = ESP_FAIL;

	if (MCPWM_MAX_DUTY >= duty)
	{
	    stat = mcpwm_comparator_set_compare_value(comparators[MCPWM_IDX_PHASE_B], duty);
	}
	else
	{
		/* Duty cycle is outside the range */
	}

	return stat;
}

/**
  * @brief Set duty cycle for phase C
  * @param duty[0 - 500]
  * @retval ESP_OK, ESP_FAIL
  */
esp_err_t LL_TIMERS_SetDutyPhC(uint16_t duty)
{
  esp_err_t stat = ESP_FAIL;

	if (MCPWM_MAX_DUTY >= duty)
	{
		stat = mcpwm_comparator_set_compare_value(comparators[MCPWM_IDX_PHASE_C], duty);
	}
	else
	{
		/* Duty cycle is outside the range */
	}

	return stat;
}

















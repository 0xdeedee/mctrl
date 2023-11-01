#include <stdio.h>
#include <string.h>
#include "esp_timer.h"
#include "driver/mcpwm_prelude.h"
#include "LL_TIMERS.h"
#include "LL_ISR.h"
#include "BOARD_CFG.h"
#include "BLDC_CFG.h"

#define OUT_STATE_PWM_MODE				-1
#define MCPWM_TIMER_RESOLUTION_HZ	 		31200000u	/* 1 tick = 0.032us */
#define MCPWM_PERIOD					1560u		/* 50us, 20KHz */
#define SYS_TICK_PERIOD					1000		/* 1ms */

#define MCPWM_GEN_IDX_HIGH				0
#define MCPWM_GEN_IDX_LOW				1
#define MCPWM_MAX_DUTY					MCPWM_PERIOD

static mcpwm_cmpr_handle_t		comparators[BLDC_CFG_NMBR_OF_PHASES];
static mcpwm_dead_time_config_t		dt_config;

//extern QueueHandle_t			x_queue_adc;


typedef struct user_data
{
	unsigned char		idx;
	TaskHandle_t		th;
} ud_t;




static void InitTimMcpwm( TaskHandle_t adc_task );
static void InitTimSysTick(void);


static bool IRAM_ATTR mcpwm_timer_updated(	mcpwm_timer_handle_t timer, 
						const mcpwm_timer_event_data_t *edata, 
						void *user_ctx )
/*
static bool IRAM_ATTR bldc_hall_updated(        mcpwm_unit_t mcpwm,
                                                mcpwm_capture_channel_id_t cap_channel,
                                                const cap_event_data_t *edata,
                                                void *user_data )
*/
{
	ud_t				*__ud = ( ud_t * ) user_ctx;
        TaskHandle_t                    task_to_notify = ( TaskHandle_t ) __ud->th;
        BaseType_t                      high_task_wakeup = pdFALSE;

//	vTaskNotifyGiveIndexedFromISR( task_to_notify, __ud->idx, &high_task_wakeup );
//        vTaskNotifyGiveFromISR( task_to_notify, &high_task_wakeup );
        return high_task_wakeup == pdTRUE;
}

/**
  * @brief
  * @param none
  * @retval none
  */
void LL_TIMERS_Init( TaskHandle_t adc_task )
{
	InitTimMcpwm( adc_task );
	InitTimSysTick();
}

/**
  * @brief Initialize motor control timer
  * @param none
  * @retval none
  */
static void InitTimMcpwm( TaskHandle_t adc_task )
{
	mcpwm_timer_handle_t		timer = NULL;
	mcpwm_timer_config_t		timer_config;
	mcpwm_oper_handle_t		operators[BLDC_CFG_NMBR_OF_PHASES];
	mcpwm_operator_config_t		operator_config;
	mcpwm_comparator_config_t	compare_config;
	mcpwm_gen_handle_t		generators[BLDC_CFG_NMBR_OF_PHASES][2];
	mcpwm_generator_config_t	gen_config;

	const int			gen_gpios[BLDC_CFG_NMBR_OF_PHASES][2] =
	{
		{ BOARD_CFG_GPIO_PWM0A_OUT, BOARD_CFG_GPIO_PWM0B_OUT },
		{ BOARD_CFG_GPIO_PWM1A_OUT, BOARD_CFG_GPIO_PWM1B_OUT },
		{ BOARD_CFG_GPIO_PWM2A_OUT, BOARD_CFG_GPIO_PWM2B_OUT },
	};


	memset( &generators, 0, sizeof( generators ) );
	memset( &operators, 0, sizeof( operators ) );
	memset( &timer_config, 0, sizeof( timer_config ) );


	timer_config.group_id		= 0;
	timer_config.clk_src		= MCPWM_TIMER_CLK_SRC_DEFAULT;
	timer_config.resolution_hz	= MCPWM_TIMER_RESOLUTION_HZ;
	timer_config.count_mode		= MCPWM_TIMER_COUNT_MODE_UP;
	timer_config.period_ticks	= MCPWM_PERIOD;
	timer_config.intr_priority	= 0;

	ESP_ERROR_CHECK( mcpwm_new_timer( &timer_config, &timer ) );

	memset( &operator_config, 0, sizeof( mcpwm_operator_config_t ) );
	operator_config.group_id = 0;
	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
		ESP_ERROR_CHECK( mcpwm_new_operator( &operator_config, &operators[i] ) );
	}

	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
//		mcpwm_timer_event_callbacks_t		__timer_cb;
//		ud_t					__ud;
//
//		__timer_cb.on_full = mcpwm_timer_updated;
//		memset( &__ud, 0, sizeof( __ud ) );
//
//		__ud.idx = i;
//		__ud.th = adc_task;
//		mcpwm_timer_register_event_callbacks( timer, &__timer_cb, ( void * )&__ud );
		ESP_ERROR_CHECK( mcpwm_operator_connect_timer( operators[i], timer ) );
	}

	memset( &compare_config, 0, sizeof( mcpwm_comparator_config_t ) );
	compare_config.flags.update_cmp_on_tez = true;
	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
		ESP_ERROR_CHECK( mcpwm_new_comparator( operators[i], &compare_config, &comparators[i] ) );
		ESP_ERROR_CHECK( mcpwm_comparator_set_compare_value( comparators[i], 0 ) );
	}

	memset( &gen_config, 0, sizeof( mcpwm_generator_config_t ) );
	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			gen_config.gen_gpio_num = gen_gpios[i][j];
			ESP_ERROR_CHECK( mcpwm_new_generator( operators[i], &gen_config, &generators[i][j] ) );
		}
	}

	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
		ESP_ERROR_CHECK( mcpwm_generator_set_action_on_timer_event( 	generators[i][MCPWM_GEN_IDX_HIGH],
										MCPWM_GEN_TIMER_EVENT_ACTION( 	MCPWM_TIMER_DIRECTION_UP, 
														MCPWM_TIMER_EVENT_EMPTY, 
														MCPWM_GEN_ACTION_HIGH ) ) );
		ESP_ERROR_CHECK( mcpwm_generator_set_action_on_compare_event(	generators[i][MCPWM_GEN_IDX_HIGH],
										MCPWM_GEN_COMPARE_EVENT_ACTION(	MCPWM_TIMER_DIRECTION_UP, 
														comparators[i], 
														MCPWM_GEN_ACTION_LOW ) ) );
		ESP_ERROR_CHECK( mcpwm_generator_set_action_on_brake_event(	generators[i][MCPWM_GEN_IDX_HIGH],
										MCPWM_GEN_BRAKE_EVENT_ACTION(	MCPWM_TIMER_DIRECTION_UP, 
														MCPWM_OPER_BRAKE_MODE_CBC, 
														MCPWM_GEN_ACTION_LOW ) ) );
		ESP_ERROR_CHECK( mcpwm_generator_set_action_on_brake_event(	generators[i][MCPWM_GEN_IDX_HIGH],
										MCPWM_GEN_BRAKE_EVENT_ACTION(	MCPWM_TIMER_DIRECTION_UP, 
														MCPWM_OPER_BRAKE_MODE_CBC, 
														MCPWM_GEN_ACTION_LOW ) ) );
	}

	memset( &dt_config, 0, sizeof( dt_config ) );
	dt_config.posedge_delay_ticks = 5;
	dt_config.flags.invert_output = true; /* It must be here due to DRV8300 specifics(MODE) */
	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
		ESP_ERROR_CHECK( mcpwm_generator_set_dead_time(			generators[i][MCPWM_GEN_IDX_HIGH], 
										generators[i][MCPWM_GEN_IDX_HIGH], 
										&dt_config ) );
	}

	memset( &dt_config, 0, sizeof( dt_config ) );
	dt_config.negedge_delay_ticks = 5;
	dt_config.flags.invert_output = true;
	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
		ESP_ERROR_CHECK( mcpwm_generator_set_dead_time(			generators[i][MCPWM_GEN_IDX_HIGH], 
										generators[i][MCPWM_GEN_IDX_LOW], 
										&dt_config ) );
	}

	for ( int i = 0; i < BLDC_CFG_NMBR_OF_PHASES; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			ESP_ERROR_CHECK( mcpwm_generator_set_force_level( generators[i][j], OUT_STATE_PWM_MODE, true ) );
		}
	}

	ESP_ERROR_CHECK( mcpwm_timer_enable( timer ) );
	ESP_ERROR_CHECK( mcpwm_timer_start_stop( timer, MCPWM_TIMER_START_NO_STOP ) );
}

/**
  * @brief Init timer for system ticks
  * @param none
  * @retval none
  */
static void InitTimSysTick( void )
{
	esp_timer_handle_t		periodic_timer = NULL;
	const esp_timer_create_args_t	periodic_timer_args =
	{
		.callback	= LL_ISR_SysTick,
		.arg		= NULL,
	};

	ESP_ERROR_CHECK( esp_timer_create( &periodic_timer_args, &periodic_timer ) );
	ESP_ERROR_CHECK( esp_timer_start_periodic( periodic_timer, SYS_TICK_PERIOD ) );
}



esp_err_t ll_timers_set_duty( uint8_t idx, uint16_t duty )
{
	return ( MCPWM_MAX_DUTY >= duty )? mcpwm_comparator_set_compare_value( comparators[idx], duty ) : ESP_FAIL;
}













#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "LL_ADC.h"


/*
//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_EXAMPLE_CHAN0          ADC1_CHANNEL_6
#define ADC2_EXAMPLE_CHAN0          ADC2_CHANNEL_0
static const char *TAG_CH[2][10] = {{"ADC1_CH6"}, {"ADC2_CH0"}};
#else
#define ADC1_EXAMPLE_CHAN0          ADC1_CHANNEL_2
#define ADC2_EXAMPLE_CHAN0          ADC2_CHANNEL_0
static const char *TAG_CH[2][10] = {{"ADC1_CH2"}, {"ADC2_CH0"}};
#endif

//ADC Attenuation
#define ADC_EXAMPLE_ATTEN           ADC_ATTEN_DB_11

//ADC Calibration
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP_FIT


#define SEN_GVDD				( ADC_CHANNEL_0 )       // 01
#define SEN_PVDD				( ADC_CHANNEL_1 )       // 02
#define TEMP					( ADC_CHANNEL_3 )       // 04
#define I_SEN_A					( ADC_CHANNEL_4 )       // 05
#define I_SEN_B					( ADC_CHANNEL_5 )       // 06
#define I_SEN_C					( ADC_CHANNEL_6 )       // 07
#define V_SEN_A					( ADC_CHANNEL_7 )       // 08
#define V_SEN_B					( ADC_CHANNEL_8 )       // 09
#define V_SEN_C					( ADC_CHANNEL_9 )       // 10
#define ADC_CHANNEL_MAX				( 9 )





Register Timer Event Callbacks


The MCPWM timer can generate different events at runtime. 
If you have some function that should be called when a 
particular event happens, you should hook your function 
to the interrupt service routine by calling mcpwm_timer_register_event_callbacks(). 
The callback function prototype is declared in mcpwm_timer_event_cb_t. 
All supported event callbacks are listed in the mcpwm_timer_event_callbacks_t:

mcpwm_timer_event_callbacks_t::on_full sets the callback function for the timer when it counts to peak value.
mcpwm_timer_event_callbacks_t::on_empty sets the callback function for the timer when it counts to zero.
mcpwm_timer_event_callbacks_t::on_stop sets the callback function for the timer when it is stopped.
The callback functions above are called within the ISR context, 
so they should not attempt to block. For example, you may make 
sure that only FreeRTOS APIs with the ISR suffix are called within the function.


static int adc_raw[2][10];
static const char *TAG = "ADC SINGLE";

static esp_adc_cal_characteristics_t adc1_chars;
static esp_adc_cal_characteristics_t adc2_chars;

static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

*/


static void __adc_ctrl_handler( void *arg )
{

}


TaskHandle_t adc_init(void)
{
/*
    esp_err_t ret = ESP_OK;
    uint32_t voltage = 0;
    bool cali_enable = adc_calibration_init();

    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));
    //ADC2 config
    ESP_ERROR_CHECK(adc2_config_channel_atten(ADC2_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));

    while (1) {
        adc_raw[0][0] = adc1_get_raw(ADC1_EXAMPLE_CHAN0);
        ESP_LOGI(TAG_CH[0][0], "raw  data: %d", adc_raw[0][0]);
        if (cali_enable) {
            voltage = esp_adc_cal_raw_to_voltage(adc_raw[0][0], &adc1_chars);
            ESP_LOGI(TAG_CH[0][0], "cali data: %d mV", voltage);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));

        do {
            ret = adc2_get_raw(ADC2_EXAMPLE_CHAN0, ADC_WIDTH_BIT_DEFAULT, &adc_raw[1][0]);
        } while (ret == ESP_ERR_INVALID_STATE);
        ESP_ERROR_CHECK(ret);

        ESP_LOGI(TAG_CH[1][0], "raw  data: %d", adc_raw[1][0]);
        if (cali_enable) {
            voltage = esp_adc_cal_raw_to_voltage(adc_raw[1][0], &adc2_chars);
            ESP_LOGI(TAG_CH[1][0], "cali data: %d mV", voltage);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
*/
	TaskHandle_t			__adc_ctrl_task;

	xTaskCreatePinnedToCore( __adc_ctrl_handler, "adc_ctrl_handler", 4096, NULL, 10, &__adc_ctrl_task, 0 );

	return __adc_ctrl_task;
}









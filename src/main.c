#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define LOOP_FREQ_MS        500
#define LED_PIN             6
#define BUTTON_PIN          5
#define SENSOR_PIN          4
#define SENSOR_ADC_UNIT     ADC_UNIT_1
#define SENSOR_ADC_CHANNEL  ADC_CHANNEL_3
#define SENSOR_ADC_ATTEN    ADC_ATTEN_DB_12
#define SENSOR_ADC_BITWIDTH ADC_BITWIDTH_12

typedef struct 
{
    adc_unit_t unit;
    adc_channel_t channel;
    adc_atten_t atten;
    adc_bitwidth_t bitwidth;

    adc_oneshot_unit_handle_t handle;
    adc_cali_handle_t cali_handle;
    bool cali_enabled;
} adc_config_t;

typedef struct
{
    adc_config_t* io_config;
    adc_channel_t channel;
    gpio_num_t pin;
    int raw;
    int voltage_mv;
} adc_device_t;

typedef struct
{
    gpio_config_t* io_config;
    gpio_num_t pin;
    int level;
} io_device_t;

typedef struct {
    int state;
    adc_device_t* sensor;
    io_device_t* button;
    io_device_t* led;
} alarmator_config_t;

gpio_config_t gpio_config_button = {
    .pin_bit_mask = 1ULL << BUTTON_PIN,
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE};

gpio_config_t gpio_config_led = {
    .pin_bit_mask = 1ULL << LED_PIN,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE};    

adc_config_t adc_config_sensor = {
    .unit = SENSOR_ADC_UNIT,
    .channel = SENSOR_ADC_CHANNEL,
    .atten = SENSOR_ADC_ATTEN,
    .bitwidth = SENSOR_ADC_BITWIDTH,
    .cali_enabled = true};

adc_device_t sensor_config = {
    .io_config = &adc_config_sensor,
    .channel = SENSOR_ADC_CHANNEL,
    .pin = SENSOR_PIN,
    .raw = 0,
    .voltage_mv = 0};    

io_device_t button_config = {
    .io_config = &gpio_config_button,
    .pin = BUTTON_PIN,
    .level = 1};

io_device_t led_config = {
    .io_config = &gpio_config_led,
    .pin = LED_PIN,
    .level = 0};

alarmator_config_t alarmator = {
    .state = 0,
    .sensor = &sensor_config,
    .button = &button_config,
    .led = &led_config
};

bool read_adc(adc_device_t* adc)
{
    adc_oneshot_read(adc->io_config->handle, adc->channel, &adc->raw);

    if (adc->io_config->cali_enabled)
    {
        adc_cali_raw_to_voltage(adc->io_config->cali_handle, adc->raw, &adc->voltage_mv);
    }
    
    return adc->io_config->cali_enabled;
}

int read_gpio(io_device_t* io)
{
    int res = gpio_get_level(io->pin);

    io->level = res;

    return res;
}

void adc_config(adc_config_t* adc)
{
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = adc->unit };
    adc_oneshot_new_unit(&init_config, &adc->handle);

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = adc->bitwidth,
        .atten = adc->atten,
    };
    adc_oneshot_config_channel(adc->handle, adc->channel, &chan_config);

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = adc->unit,
        .chan = adc->channel,
        .atten = adc->atten,
        .bitwidth = adc->bitwidth,
    };
    adc->cali_enabled = adc_cali_create_scheme_curve_fitting(&cali_config, &adc->cali_handle) == ESP_OK;
}

void init_io_configs()
{
    adc_config(sensor_config.io_config);
    gpio_config(button_config.io_config);
    gpio_config(led_config.io_config);
}

void handle_sensor(void* params)
{
    alarmator_config_t* alarmator = (alarmator_config_t*)params;

    while (1)
    {
        read_adc(alarmator->sensor);

        switch (alarmator->state)
        {
            case 1:
                if (alarmator->sensor->voltage_mv > 2800) // TODO: question for a threshold
                {
                    alarmator->led->level = 0;
                } else
                {
                    alarmator->led->level = 1;
                }

                break;
            case 2:
                alarmator->led->level = 1;
                break;    
            default:
                alarmator->led->level = 0;
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
    }
}

void handle_button(void* params)
{
    alarmator_config_t* alarmator = (alarmator_config_t*)params;

    while (1)
    {
        read_gpio(alarmator->button);

        if (!alarmator->button->level)
        {
            switch (alarmator->state)
            {
                case 0:
                    alarmator->state = 1;
                    break;
                case 1:
                    alarmator->state = 2;
                    break;
                case 2:
                    alarmator->state = 0;
                    break;    
                default:
                    break;
            }
        }
        else if (!alarmator->button->level)
        {
            alarmator->button->level = 1;
        }

        vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
    }
}

void handle_led(void* params)
{
    alarmator_config_t* alarmator = (alarmator_config_t*)params;

    while (1)
    {
        if (alarmator->led->level && alarmator->state == 2)
        {
            gpio_set_level(alarmator->led->pin, 1);
            vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
        }
        else if (alarmator->led->level)
        {
            gpio_set_level(alarmator->led->pin, 1);
            vTaskDelay(pdMS_TO_TICKS(20));

            gpio_set_level(alarmator->led->pin, 0);
            vTaskDelay(pdMS_TO_TICKS(20));
        } else 
        {
            gpio_set_level(alarmator->led->pin, 0);
            vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
        }
    }
}

void app_main() {
    init_io_configs();

    xTaskCreate(handle_sensor, "sensor_task", 4096, &alarmator, 5, NULL);
    xTaskCreate(handle_button, "button_task", 4096, &alarmator, 5, NULL);
    xTaskCreate(handle_led, "led_task", 4096, &alarmator, 5, NULL);
}
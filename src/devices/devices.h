#pragma once

#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

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

void adc_config(adc_config_t* adc);
bool read_adc(adc_device_t* adc);
int read_gpio(io_device_t* io);

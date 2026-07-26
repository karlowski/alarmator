#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define LOOP_FREQ_MS        200
#define SENSOR_THRESHOLD    2800

#define LED_PIN             GPIO_NUM_6
#define BUTTON_PIN          GPIO_NUM_5
#define SENSOR_PIN          GPIO_NUM_5
#define SENSOR_ADC_UNIT     ADC_UNIT_1
#define SENSOR_ADC_CHANNEL  ADC_CHANNEL_3
#define SENSOR_ADC_ATTEN    ADC_ATTEN_DB_12
#define SENSOR_ADC_BITWIDTH ADC_BITWIDTH_12

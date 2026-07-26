#include "alarmator.h"
#include "config.h"

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

void init_io_configs(void)
{
    adc_config(sensor_config.io_config);
    gpio_config(button_config.io_config);
    gpio_config(led_config.io_config);
}

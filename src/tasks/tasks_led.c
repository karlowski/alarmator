#include "tasks.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "alarmator.h"
#include "config.h"

void handle_led(void* params)
{
    alarmator_config_t* alarmator = (alarmator_config_t*)params;

    while (1)
    {
        switch (alarmator->state)
        {
            case 2:
                gpio_set_level(alarmator->led->pin, 1);
                vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
                break;
            case 1:
                if (alarmator->led->level)
                {
                    gpio_set_level(alarmator->led->pin, 1);
                    vTaskDelay(pdMS_TO_TICKS(20));
                    gpio_set_level(alarmator->led->pin, 0);
                    vTaskDelay(pdMS_TO_TICKS(20));
                }
                else
                {
                    gpio_set_level(alarmator->led->pin, 0);
                    vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
                }
                break;
            default:
                gpio_set_level(alarmator->led->pin, 0);
                vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
                break;
        }
    }
}

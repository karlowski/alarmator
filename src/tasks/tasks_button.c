#include "tasks.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "alarmator.h"
#include "devices/devices.h"
#include "config.h"

void handle_button(void* params)
{
    alarmator_config_t* alarmator = (alarmator_config_t*)params;
    int level_prev = 1;

    while (1)
    {
        read_gpio(alarmator->button);
        if (alarmator->button->level && !level_prev)
        {
            switch (alarmator->state)
            {
                case 0: alarmator->state = 1; break;
                case 1: alarmator->state = 2; break;
                case 2: alarmator->state = 0; break;
                default: break;
            }
        }

        level_prev = alarmator->button->level;

        vTaskDelay(pdMS_TO_TICKS(LOOP_FREQ_MS));
    }
}

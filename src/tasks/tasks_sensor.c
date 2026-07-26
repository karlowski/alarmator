#include "tasks.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "alarmator.h"
#include "config.h"

void handle_sensor(void* params)
{
    alarmator_config_t* alarmator = (alarmator_config_t*)params;

    while (1)
    {
        read_adc(alarmator->sensor);

        switch (alarmator->state)
        {
            case 1: 
                alarmator->led->level = alarmator->sensor->voltage_mv > SENSOR_THRESHOLD ? 0 : 1; // TODO: question for a threshold
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

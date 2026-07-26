#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "alarmator.h"
#include "tasks/tasks.h"

void app_main() {
    init_io_configs();

    xTaskCreate(handle_sensor, "sensor_task", 4096, &alarmator, 5, NULL);
    xTaskCreate(handle_button, "button_task", 4096, &alarmator, 5, NULL);
    xTaskCreate(handle_led, "led_task", 4096, &alarmator, 5, NULL);
}
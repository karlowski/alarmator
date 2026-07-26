#pragma once

#include "devices/devices.h"

typedef struct
{
    int state;
    adc_device_t* sensor;
    io_device_t* button;
    io_device_t* led;
} alarmator_config_t;

extern alarmator_config_t alarmator;

void init_io_configs(void);

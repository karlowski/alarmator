#include "devices.h"

// Note: to mimic same behavior as adc_oneshot_read
int read_gpio(io_device_t* io)
{
    int level = gpio_get_level(io->pin);

    io->level = level;

    return level;
}

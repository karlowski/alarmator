#include "devices.h"
#include "esp_adc/adc_cali_scheme.h"

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

bool read_adc(adc_device_t* adc)
{
    adc_oneshot_read(adc->io_config->handle, adc->channel, &adc->raw);

    if (adc->io_config->cali_enabled)
    {
        adc_cali_raw_to_voltage(adc->io_config->cali_handle, adc->raw, &adc->voltage_mv);
    }

    return adc->io_config->cali_enabled;
}

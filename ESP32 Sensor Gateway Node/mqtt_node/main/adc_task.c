#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

static const char *TAG = "ADC_READ";

// LM35 → 10 mV/°C
#define LM35_mV_PER_C 10.0

// ADC Calibration
#define DEFAULT_VREF    1100    // mV – used if no eFuse calibration
static esp_adc_cal_characteristics_t adc_chars;

void adc_init(void)
{
    // ----- ADC Config -----
    adc1_config_width(ADC_WIDTH_BIT_12);                    // 0–4095
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);  
    /*
        Channel 6 = GPIO34
        ATTN_11dB → input range approx 0–3.3V
    */

    // ----- ADC Calibration -----
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
        ADC_UNIT_1,
        ADC_ATTEN_DB_12,
        ADC_WIDTH_BIT_12,
        DEFAULT_VREF,
        &adc_chars
    );

    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI(TAG, "Using eFuse Vref");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI(TAG, "Using Two Point calibration");
    } else {
        ESP_LOGW(TAG, "Using Default Vref");
    }
}



void sensor_task()
{
    // <! Implement sensor sampling task here
    int delay = 1000;
    

    while (1)
    {
        // Read raw adc
        int raw = adc1_get_raw(ADC1_CHANNEL_6);

        // Convert to millivolts
        uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(raw, &adc_chars);

        // LM35: 10 mV per °C
        float temperature_c = voltage_mv / LM35_mV_PER_C;

        // ESP_LOGI(TAG, "Raw: %d   Voltage: %u mV   Temp: %.2f °C",
        //          raw, voltage_mv, temperature_c);

        vTaskDelay(pdMS_TO_TICKS(delay));
    }
}
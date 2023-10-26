#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "vl53l0x_user.h"

#define I2C_MASTER_SCL_IO           2                           /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           4                           /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_SLAVE_DEV_ADDRESS       0x8A
#define PROX_SENSOR_XSHUT_GPIO      17

static const char *tag = "Proximity sensor";
static vl53l0x_t prox_cfg;

void prox_task(void *pvParameter) {
    while (1) {
        uint16_t  range = vl53l0x_readRangeSingleMillimeters(&prox_cfg);
        ESP_LOGI(tag, "range = %i", range);
        usleep(1000);
    }
}
#include "vl53l0x.h"
void prox_init(void) {
    const char * err;
    err = vl53l0x_config(&prox_cfg , I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO, I2C_SLAVE_DEV_ADDRESS, -1, 0);
    ESP_LOGI(tag, "%s", err);

    for (int address = 1; address <= 127; address++) { /* Scan TWI for all present devices. */
        err_code = i2c_readReg(twim_idx, address, 0, &sample_data);
        if (err_code == NRF_SUCCESS) {
            detected_device = true;
            LOGI("TWI address 0x%02x ........ OK.", address);
        }
        /*
        else
        {
            LOGE ("TWI address 0x%02x ........ FAIL !!!", address);
        }
        */
        nrf_delay_ms(1);
    }

    
    uint16_t reg = vl53l0x_readReg8Bit(&prox_cfg, 0x0E);
    ESP_LOGI(tag, "reg = %i", reg);
    err = vl53l0x_init(&prox_cfg);
    ESP_LOGI(tag, "%s", err);
    xTaskCreate(&prox_task, "prox_task", 4096, NULL, 3, NULL);
    ESP_LOGI(tag, "Proxipity sensor initialized.");
}

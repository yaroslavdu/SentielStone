#include <stdio.h>
#include "driver/i2c.h"
#include "i2c_driver.h"


#define I2C_MASTER_SCL_IO           2                           /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           4                           /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_SLAVE_DEV_ADDRESS       0x29
#define PROX_SENSOR_XSHUT_GPIO      17

esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void i2c_scan_bus(void) {

    const uint8_t reg_addr = 0x0C;
    uint8_t data[8];
    uint16_t len = 8;
    esp_err_t err_code;
    for (int address = 1; address <= 127; address++) { /* Scan TWI for all present devices. */
        err_code = i2c_master_write_read_device(I2C_MASTER_NUM, address, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
        if (err_code == ESP_OK) {
            ESP_LOGI("scan bus", "TWI address 0x%02x ........ OK.", address);
        }
        /*
        else
        {
            LOGE ("TWI address 0x%02x ........ FAIL !!!", address);
        }
    */
    vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
uint8_t result_reg = 0xC0;
void i2c_read_reg_uint16(void) {
    uint8_t data[2];
    uint16_t len = 2;
    esp_err_t err_code;
    err_code = i2c_master_write_read_device(I2C_MASTER_NUM, I2C_SLAVE_DEV_ADDRESS, &result_reg, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (err_code == ESP_OK) {
        ESP_LOGI("i2c_read_reg_uint16", "data0=%i, data1=%i", data[0], data[1]);
    }
}
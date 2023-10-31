#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_
#include "esp_log.h"
#include <esp_log_internal.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"

#define I2C_MASTER_SCL_IO           2                           /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           4                           /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_SLAVE_DEV_ADDRESS       0x29
#define PROX_SENSOR_XSHUT_GPIO      17

void i2c_scan_bus(void);
esp_err_t i2c_master_init(void);
void i2c_read_reg_uint16(void);
#endif //_I2C_DRIVER_H_
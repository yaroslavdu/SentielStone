#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_
#include "esp_log.h"
#include <esp_log_internal.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"


void i2c_scan_bus(void);
esp_err_t i2c_master_init(void);
void i2c_read_reg_uint16(void);
#endif //_I2C_DRIVER_H_
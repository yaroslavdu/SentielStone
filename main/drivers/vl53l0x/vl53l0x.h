// VL53L0X control
// Copyright © 2019 Adrian Kennard, Andrews & Arnold Ltd. See LICENCE file for details. GPL 3.0

#ifndef VL53L0X_H
#define VL53L0X_H

// https://github.com/revk/ESP32-VL53L0X/tree/master

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

enum
{
   SYSRANGE_START = 0x00,

   SYSTEM_THRESH_HIGH = 0x0C,
   SYSTEM_THRESH_LOW = 0x0E,

   SYSTEM_SEQUENCE_CONFIG = 0x01,
   SYSTEM_RANGE_CONFIG = 0x09,
   SYSTEM_INTERMEASUREMENT_PERIOD = 0x04,

   SYSTEM_INTERRUPT_CONFIG_GPIO = 0x0A,

   GPIO_HV_MUX_ACTIVE_HIGH = 0x84,

   SYSTEM_INTERRUPT_CLEAR = 0x0B,

   RESULT_INTERRUPT_STATUS = 0x13,
   RESULT_RANGE_STATUS = 0x14,

   RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN = 0xBC,
   RESULT_CORE_RANGING_TOTAL_EVENTS_RTN = 0xC0,
   RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF = 0xD0,
   RESULT_CORE_RANGING_TOTAL_EVENTS_REF = 0xD4,
   RESULT_PEAK_SIGNAL_RATE_REF = 0xB6,

   ALGO_PART_TO_PART_RANGE_OFFSET_MM = 0x28,

   MSRC_CONFIG_CONTROL = 0x60,

   PRE_RANGE_CONFIG_MIN_SNR = 0x27,
   PRE_RANGE_CONFIG_VALID_PHASE_LOW = 0x56,
   PRE_RANGE_CONFIG_VALID_PHASE_HIGH = 0x57,
   PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT = 0x64,

   FINAL_RANGE_CONFIG_MIN_SNR = 0x67,
   FINAL_RANGE_CONFIG_VALID_PHASE_LOW = 0x47,
   FINAL_RANGE_CONFIG_VALID_PHASE_HIGH = 0x48,
   FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,

   PRE_RANGE_CONFIG_SIGMA_THRESH_HI = 0x61,
   PRE_RANGE_CONFIG_SIGMA_THRESH_LO = 0x62,

   PRE_RANGE_CONFIG_VCSEL_PERIOD = 0x50,
   PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x51,
   PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x52,

   SYSTEM_HISTOGRAM_BIN = 0x81,
   HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT = 0x33,
   HISTOGRAM_CONFIG_READOUT_CTRL = 0x55,

   FINAL_RANGE_CONFIG_VCSEL_PERIOD = 0x70,
   FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x71,
   FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x72,
   CROSSTALK_COMPENSATION_PEAK_RATE_MCPS = 0x20,

   MSRC_CONFIG_TIMEOUT_MACROP = 0x46,

   I2C_SLAVE_DEVICE_ADDRESS = 0x8A,

   SOFT_RESET_GO2_SOFT_RESET_N = 0xBF,
   IDENTIFICATION_MODEL_ID = 0xC0,
   IDENTIFICATION_REVISION_ID = 0xC2,

   OSC_CALIBRATE_VAL = 0xF8,

   GLOBAL_CONFIG_VCSEL_WIDTH = 0x32,
   GLOBAL_CONFIG_SPAD_ENABLES_REF_0 = 0xB0,
   GLOBAL_CONFIG_SPAD_ENABLES_REF_1 = 0xB1,
   GLOBAL_CONFIG_SPAD_ENABLES_REF_2 = 0xB2,
   GLOBAL_CONFIG_SPAD_ENABLES_REF_3 = 0xB3,
   GLOBAL_CONFIG_SPAD_ENABLES_REF_4 = 0xB4,
   GLOBAL_CONFIG_SPAD_ENABLES_REF_5 = 0xB5,

   GLOBAL_CONFIG_REF_EN_START_SELECT = 0xB6,
   DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD = 0x4E,
   DYNAMIC_SPAD_REF_EN_START_OFFSET = 0x4F,
   POWER_MANAGEMENT_GO1_POWER_FORCE = 0x80,

   VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV = 0x89,

   ALGO_PHASECAL_LIM = 0x30,
   ALGO_PHASECAL_CONFIG_TIMEOUT = 0x30,
};

typedef struct {
    uint8_t port;
    uint8_t address;
    int8_t xshut;
    uint16_t io_timeout;
    uint8_t io_2v8:1;
    uint8_t did_timeout:1;
    uint8_t i2c_fail:1;
    // int8_t port;
    // int8_t scl;
    // int8_t sda;
    // int8_t xshut;
    // uint8_t address;
    // uint8_t io_2v8;
    // uint8_t i2c_fail;
    // uint8_t did_timeout;
    // uint16_t io_timeout;
} vl53l0x_t;

typedef enum
{ VcselPeriodPreRange, VcselPeriodFinalRange } vl53l0x_vcselPeriodType;

// Functions returning const char * are OK for NULL, else error string

// Set up I2C and create the vl53l0x structure, NULL means could not see device on I2C
const char *  vl53l0x_config (vl53l0x_t * cfg, int8_t port, int8_t scl, int8_t sda, int8_t xshut, uint8_t address, uint8_t io_2v8);
// Initialise the VL53L0X
const char *vl53l0x_init (vl53l0x_t *);
// End I2C and free the structure
void vl53l0x_end (vl53l0x_t *);

void vl53l0x_setAddress (vl53l0x_t *, uint8_t new_addr);
uint8_t vl53l0x_getAddress (vl53l0x_t * v);

void vl53l0x_writeReg8Bit (vl53l0x_t *, uint8_t reg, uint8_t value);
void vl53l0x_writeReg16Bit (vl53l0x_t *, uint8_t reg, uint16_t value);
void vl53l0x_writeReg32Bit (vl53l0x_t *, uint8_t reg, uint32_t value);
uint8_t vl53l0x_readReg8Bit (vl53l0x_t *, uint8_t reg);
uint16_t vl53l0x_readReg16Bit (vl53l0x_t *, uint8_t reg);
uint32_t vl53l0x_readReg32Bit (vl53l0x_t *, uint8_t reg);

void vl53l0x_writeMulti (vl53l0x_t *, uint8_t reg, uint8_t const *src, uint8_t count);
void vl53l0x_readMulti (vl53l0x_t *, uint8_t reg, uint8_t * dst, uint8_t count);

const char *vl53l0x_setSignalRateLimit (vl53l0x_t *, float limit_Mcps);
float vl53l0x_getSignalRateLimit (vl53l0x_t *);

const char *vl53l0x_setMeasurementTimingBudget (vl53l0x_t *, uint32_t budget_us);
uint32_t vl53l0x_getMeasurementTimingBudget (vl53l0x_t *);

const char *vl53l0x_setVcselPulsePeriod (vl53l0x_t *, vl53l0x_vcselPeriodType type, uint8_t period_pclks);
uint8_t vl53l0x_getVcselPulsePeriod (vl53l0x_t *, vl53l0x_vcselPeriodType type);

void vl53l0x_startContinuous (vl53l0x_t *, uint32_t period_ms);
void vl53l0x_stopContinuous (vl53l0x_t *);
uint16_t vl53l0x_readRangeContinuousMillimeters (vl53l0x_t *);
uint16_t vl53l0x_readRangeSingleMillimeters (vl53l0x_t *);

void vl53l0x_setTimeout (vl53l0x_t *, uint16_t timeout);
uint16_t vl53l0x_getTimeout (vl53l0x_t *);
int vl53l0x_timeoutOccurred (vl53l0x_t *);
int vl53l0x_i2cFail (vl53l0x_t *);


#endif

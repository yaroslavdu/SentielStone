/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */


#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <esp_log.h>
// #include <esp_log_internal.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sdkconfig.h"

#include "driver/gpio.h"
#include "pn532_user.h"
#include "vl53l0x_user.h"
#include "cap_touch.h"
#include "driver/spi_master.h"
#include "msm.h"
#include "esp_timer.h"
#include "esp_log.h"

static void startup_debug(void) {
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

}

void app_main(void)
{
    startup_debug();

    esp_log_level_set(__FUNCTION__, ESP_LOG_INFO);

    pn532_init();
    //prox_init();
    cap_touch_init();

    msm_init();
    send_msm_event(5);
    fflush(stdout);
    //esp_restart();

    for (;;) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

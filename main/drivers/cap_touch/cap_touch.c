#include "cap_touch.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include "esp_log.h"
#include "msm.h"

#define TOUCH_PAD_GPIO4_CHANNEL TOUCH_PAD_NUM0
static TaskHandle_t         m_cap_touch_task = NULL;

#define CAPT_THRESH_UP      185
#define CAPT_THRESH_DOWN    140

int capt_detect_object(void) {
    uint16_t val;
    uint16_t filtered_value = 0;
    uint16_t raw_value_touch = 0;

    touch_pad_read_raw_data(TOUCH_PAD_GPIO4_CHANNEL, &raw_value_touch);
    touch_pad_read_filtered(TOUCH_PAD_GPIO4_CHANNEL, &filtered_value);
    touch_pad_read(TOUCH_PAD_GPIO4_CHANNEL, &val);
    ESP_LOGI(__FUNCTION__, "val_touch_gpio13 = %d raw_value = %d filtered_value = %d\n", val, raw_value_touch, filtered_value);

    if (filtered_value < CAPT_THRESH_DOWN) {
        return 0;
    }

    return -1;
}

int capt_detect_clear_zone(void) {
    uint16_t val;
    uint16_t filtered_value = 0;
    uint16_t raw_value_touch = 0;

    touch_pad_read_raw_data(TOUCH_PAD_GPIO4_CHANNEL, &raw_value_touch);
    touch_pad_read_filtered(TOUCH_PAD_GPIO4_CHANNEL, &filtered_value);
    touch_pad_read(TOUCH_PAD_GPIO4_CHANNEL, &val);
    ESP_LOGI(__FUNCTION__, "val_touch_gpio13 = %d raw_value = %d filtered_value = %d\n", val, raw_value_touch, filtered_value);

    if (filtered_value > CAPT_THRESH_UP) {
        return 0;
    }

    return -1;
}
void capt_execute(void){
    BaseType_t      xResult;
    static uint8_t        cap_touch_cmd;

    xResult = xTaskNotifyWait(0, 0, &cap_touch_cmd, portMAX_DELAY);

    switch (cap_touch_cmd) {
        case CAP_TOUCH_CMD_IDLE:
            break;
        case CAP_TOUCH_CMD_DETECT_OBJECT:
            while (capt_detect_object()) {
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            send_msm_event(MSM_EVT_OBJECT_DETECTED);
            break;
        case CAP_TOUCH_CMD_DETECT_CLEAR_ZONE:
            while (capt_detect_clear_zone()) {
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            send_msm_event(MSM_EVT_OBJECT_REMOVED);
            break;
    }
}
void cap_touch_task(void) {
    while(1) {
        capt_execute();
    }
}

void cap_touch_init(void) {
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_GPIO4_CHANNEL, -1);
    touch_pad_filter_start(10);

    xTaskCreate(cap_touch_task, "cap_touch_task", 4096, NULL, 4, &m_cap_touch_task);
    ESP_LOGI(__FUNCTION__, "cap_touch_task created");
}

void capt_start_scan_object(void) {
    xTaskNotify(m_cap_touch_task, CAP_TOUCH_CMD_DETECT_OBJECT, eSetValueWithOverwrite);
}

void capt_start_scan_clear_zone(void) {
    xTaskNotify(m_cap_touch_task, CAP_TOUCH_CMD_DETECT_CLEAR_ZONE, eSetValueWithOverwrite);
}

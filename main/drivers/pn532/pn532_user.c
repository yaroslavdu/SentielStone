#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include <esp_log_internal.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sdkconfig.h"

#include "driver/gpio.h"

#include "msm.h"
#include "pn532.h"
#include "pn532_user.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "ws2812_api.h"

#define BLINK_GPIO      2

// #define PN532_SCK       32
// #define PN532_MOSI      26
// #define PN532_SS        25
// #define PN532_MISO      33

#define PN532_SCK       26
#define PN532_MOSI      33
#define PN532_SS        32
#define PN532_MISO      25


static TaskHandle_t         m_nfc_task = NULL;
static pn532_t              nfc;
static esp_timer_handle_t   nfc_read_timer;

// char* nfc_state_to_text(nfc_state_t state) {
//     static char* state_names[] = {
//         "NFC_STATE_IDLE",
//         "NFC_STATE_READ"
//     };

//     if (state < NFC_STATE_COUNT) {
//         return state_names[state];
//     } else {
//         return "State undefined";
//     }
// }

static uint8_t uid[7] = {0}; // Buffer to store the returned UID
static uint8_t nfc_read_timeout_fl = 0;
static uint32_t nfc_buf[100] = {0};

static uint32_t nfc_tag_convert_to_u32(uint8_t * tag_buf) {
    static uint32_t ret = 0;
    ret = uid[0] << 24;
    ret |= uid[1] << 16;
    ret |= uid[2] << 8;
    ret |= uid[3];
    return ret;
}

static void nfc_found_in_buf(uint32_t tag) {
    for (int i=0; i < 100; i++) {
        if (nfc_buf[i] == tag) {
            nfc_buf[i] = 0;
            led_indicate(COLOR_RED, 200, 3);
            return;
        }
    }

    for (int i=0; i < 100; i++) {
        if (nfc_buf[i] == 0) {
            nfc_buf[i] = tag;
            led_indicate(COLOR_GREEN, 200, 3);
            return;
        }
    }
}

static int nfc_check_tag_access(uint32_t tag) {
    for (int i=0; i < 100; i++) {
        if (nfc_buf[i] == tag) {
            return 1;
        }
    }
    return 0;
}

int nfc_read_tag(uint8_t * uid) {
    uint8_t success;
    uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

    if (success && nfc_check_tag_access(nfc_tag_convert_to_u32(uid))) {
        // Display some basic information about the card
        ESP_LOGI(__FUNCTION__, "UID Value:");
        esp_log_buffer_hexdump_internal(__FUNCTION__, uid, uidLength, ESP_LOG_INFO);
        esp_timer_stop(nfc_read_timer);
        return 1;

    }
    return 0;
}


void nfc_process_admin_mode(void) {
    BaseType_t              xResult;
    static uint32_t         nfc_cmd;
        uint8_t success;
    uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    while(1) {
        xResult = xTaskNotifyWait(0, 0, &nfc_cmd, 0);
        if (nfc_cmd == NFC_ADMIN_MODE_STOP) return;

        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

        if (success) {
            ESP_LOGI(__FUNCTION__, "uid[0] = 0x%0X", uid[0]);
            ESP_LOGI(__FUNCTION__, "uid[1] = 0x%0X", uid[1]);
            ESP_LOGI(__FUNCTION__, "uid[2] = 0x%0X", uid[2]);
            ESP_LOGI(__FUNCTION__, "uid[3] = 0x%0X", uid[3]);
            ESP_LOGI(__FUNCTION__, "uid[4] = 0x%0X", uid[4]);
            ESP_LOGI(__FUNCTION__, "uid[5] = 0x%0X", uid[5]);
            ESP_LOGI(__FUNCTION__, "uid[6] = 0x%0X", uid[6]);

            ESP_LOGI(__FUNCTION__, "tag_id_32b = %"PRIu32"", nfc_tag_convert_to_u32(uid));

            nfc_found_in_buf(nfc_tag_convert_to_u32(uid));
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void nfc_execute() {
    BaseType_t              xResult;
    static uint32_t         nfc_cmd;

    xResult = xTaskNotifyWait(0, 0, &nfc_cmd, portMAX_DELAY);

    switch (nfc_cmd) {
        case NFC_START_SCAN:
            esp_timer_start_once(nfc_read_timer, 1100000); //1.1s
            nfc_read_timeout_fl = 1;
            ESP_LOGI(__FUNCTION__, "NFC scan start");

            while (nfc_read_timeout_fl) {
                if(nfc_read_tag(uid)) {
                    led_indicate(COLOR_GREEN, 2000, 1);
                    send_msm_event(MSM_EVT_NFC_READ_DONE);
                    return;
                }
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            led_indicate(COLOR_RED, 2000, 1);
            send_msm_event(MSM_EVT_NFC_READ_DONE);
            break;
        case NFC_ADMIN_MODE:
            ESP_LOGI(__FUNCTION__, "NFC admin mode start");
            nfc_process_admin_mode();
            break;
        default:
            break;
    }


}

void nfc_task(void *pvParameter) {
    while (1) {
        nfc_execute();
    }
}

static void nfc_read_timer_callback(void* arg) {
    ESP_LOGI(__FUNCTION__, "NFC scan expired");
    nfc_read_timeout_fl = 0;
}



void pn532_init(void) {
    pn532_spi_init(&nfc, PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
    pn532_begin(&nfc);

    uint32_t versiondata = pn532_getFirmwareVersion(&nfc);
    if (!versiondata)
    {
        ESP_LOGI(__FUNCTION__, "Didn't find PN53x board");
        return;
    }
    // Got ok data, print it out!
    ESP_LOGI(__FUNCTION__, "Found chip PN5 %lu", (versiondata >> 24) & 0xFF);
    ESP_LOGI(__FUNCTION__, "Firmware ver. %lu.%lu", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);

    // configure board to read RFID tags
    pn532_SAMConfig(&nfc);

    const esp_timer_create_args_t nfc_read_timer_args = {
        .callback = &nfc_read_timer_callback,
        /* name is optional, but may help identify the timer when debugging */
        .name = "nfc read timeout"
    };


    ESP_ERROR_CHECK(esp_timer_create(&nfc_read_timer_args, &nfc_read_timer));
    
    xTaskCreate(nfc_task, "nfc_task", 4096, NULL, 3, &m_nfc_task);
}

void nfc_read_start(void) {
    xTaskNotify(m_nfc_task, NFC_START_SCAN, eSetValueWithOverwrite);
}

void nfc_logging_start(void) {
    xTaskNotify(m_nfc_task, NFC_ADMIN_MODE, eSetValueWithOverwrite);
}

void nfc_logging_stop(void) {
    xTaskNotify(m_nfc_task, NFC_ADMIN_MODE_STOP, eSetValueWithOverwrite);
}
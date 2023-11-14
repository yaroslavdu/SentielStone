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



#define BLINK_GPIO      2

#define PN532_SCK       32
#define PN532_MOSI      26
#define PN532_SS        25
#define PN532_MISO      33

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



int nfc_read_tag(void) {
    uint8_t success;
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
    uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 1);
    if (success) {
        // Display some basic information about the card
        ESP_LOGI(__FUNCTION__, "UID Value:");
        esp_log_buffer_hexdump_internal(__FUNCTION__, uid, uidLength, ESP_LOG_INFO);
        //xTaskNotify(m_nfc_task, NFC_IDLE, eSetValueWithOverwrite);
        esp_timer_stop(nfc_read_timer);

        return 1;
    }
    return 0;
}

uint8_t nfc_read_timeout_fl = 0;
void nfc_execute() {
    BaseType_t      xResult;
    static uint32_t        nfc_cmd;

    xResult = xTaskNotifyWait(0, 0, &nfc_cmd, portMAX_DELAY);

    esp_timer_start_once(nfc_read_timer, 5000000); //200ms
    nfc_read_timeout_fl = 1;
    ESP_LOGI(__FUNCTION__, "NFC scan start");

    while (nfc_read_timeout_fl) {
        if(nfc_read_tag()) {
            send_msm_event(MSM_EVT_NFC_FOUND);
            return;
        }
        ESP_LOGI(__FUNCTION__, "NFC read ...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    send_msm_event(MSM_EVT_NFC_NOT_FOUND);
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

void nfc_read_start(void) {
    xTaskNotify(m_nfc_task, NFC_START_SCAN, eSetValueWithOverwrite);
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
    

    xTaskCreate(nfc_task, "nfc_task", 4096, NULL, 4, &m_nfc_task);
}
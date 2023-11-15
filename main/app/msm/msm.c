#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "msm.h"
#include "vl53l0x_user.h"
#include "pn532_user.h"

static QueueHandle_t m_msm_queue_handle = NULL;  /* Queue used by MSM to receive messages from all other tasks/ISRs */
static TaskHandle_t  m_msm_task = NULL;

static msm_state_e m_msm_curr_state; /* Main State Machine current state */

static char* msm_state_to_text(msm_state_e state) {
    static char* state_names[] = {
        "MSM_STATE_IDLE",
        "MSM_STATE_WAIT_FOR_OBJECT",
        "MSM_STATE_WAIT_FOR_NFC",
        "MSM_STATE_WAIT_FOR_OBJECT_REMOVING",
    };

    if (state < MSM_STATE_COUNT) {
        return state_names[state];
    } else {
        return "State undefined";
    }
}

static char* msm_event_to_text(msm_event_e event) {
    static char* event_names[] = {
        "MSM_EVT_OBJECT_DETECTED",
        "MSM_EVT_OBJECT_REMOVED",
        "MSM_EVT_NFC_FOUND",
        "MSM_EVT_NFC_NOT_FOUND",
    };
    
    if (event < MSM_EVT_COUNT) {
        return event_names[event];
    } else {
        return "Event undefined";
    }
}
/** @brief Processing an enter to new MSM state. */
static void msm_on_enter_state(msm_state_e state) {


    ESP_LOGI(__FUNCTION__, "New MSM state: %s(%d)", msm_state_to_text(state), state);
    switch (state) {

        case MSM_STATE_IDLE:
            break;
        case MSM_STATE_WAIT_FOR_OBJECT:
            prox_start_scan();
            break;
        case MSM_STATE_WAIT_FOR_NFC:
            nfc_read_start();
            break;
        case MSM_STATE_WAIT_FOR_OBJECT_REMOVING:
            prox_start_scan_object_removed();

        default:
            /* Do nothing. */
            break;
        }
}

/** @brief Processing the exit from MSM state. */
static void msm_on_exit_state(msm_state_e state) {
    switch (state) {
    case MSM_STATE_IDLE:
        //app_timer_stop(m_N_seconds_of_inactivity);
        break;

    default:
        /* Do nothing. */
        break;
    }
};

static msm_state_e msm_process_event(msm_event_e event) {

    ESP_LOGI(__FUNCTION__, "MSM process event: %s(%d) in state: %s(%d)", \
                        msm_event_to_text(event), event, msm_state_to_text(m_msm_curr_state),  m_msm_curr_state);
    msm_state_e new_state = m_msm_curr_state; /* assume not change state if event isn't processed */

    switch (m_msm_curr_state) {
        /* ------------- MSM_STATE_IDLE --------------------- */
        case MSM_STATE_IDLE:
            new_state = MSM_STATE_WAIT_FOR_OBJECT;
        case MSM_STATE_COUNT:
            break;
        case MSM_STATE_WAIT_FOR_OBJECT:
            switch (event) {
                // case MSM_EVT_USR_BTN_PRESS:
                //     new_state = MSM_STATE_WAIT_FOR_NFC_IN_ADMIN_MODE;
                //     break;
                case MSM_EVT_OBJECT_DETECTED:
                    new_state = MSM_STATE_WAIT_FOR_NFC;
                    break;
                case MSM_EVT_OBJECT_REMOVED:
                case MSM_EVT_NFC_FOUND:
                case MSM_EVT_NFC_NOT_FOUND:
                case MSM_EVT_COUNT:
                    break;
            }
            break;
        /* ------------- MSM_STATE_WAIT_FOR_NFC ------------- */
        case MSM_STATE_WAIT_FOR_NFC:
            switch (event) {
                case MSM_EVT_NFC_FOUND:
                    new_state = MSM_STATE_WAIT_FOR_OBJECT_REMOVING;
                    //led_indicate(PASS_INDK);
                    break;
                case MSM_EVT_NFC_NOT_FOUND:
                    //led_indicate(ALARM_INDK);
                    new_state = MSM_STATE_WAIT_FOR_OBJECT_REMOVING;
                    break;
                case MSM_EVT_OBJECT_REMOVED:
                case MSM_EVT_COUNT:
                case MSM_EVT_OBJECT_DETECTED:
                    break;
            }
            break;
        case MSM_STATE_WAIT_FOR_OBJECT_REMOVING:
            switch (event) {
                case MSM_EVT_OBJECT_REMOVED:
                    new_state = MSM_STATE_WAIT_FOR_OBJECT;
                    break;
                case MSM_EVT_NFC_FOUND:
                case MSM_EVT_NFC_NOT_FOUND:
                case MSM_EVT_COUNT:
                case MSM_EVT_OBJECT_DETECTED:
                    break;
            }
            break;
        // case MSM_STATE_WAIT_FOR_NFC_IN_ADMIN_MODE:
        //     switch (event) {
        //         case MSM_EVT_USR_BTN_PRESS:
        //             new_state = MSM_STATE_IDLE;
        //             break;
        //         case MSM_EVT_NFC_FOUND:
        //             //storage_tag_add();
        //             led_indicate(TAG_ADD_INDK);
        //             break;
        //         case MSM_EVT_NFC_NOT_FOUND:
        //             //storage_tag_delete();
        //             //led_indicate(TAG_DEL_INDK);
        //             break;
        //     }
        // case MSM_STATE_LED_INDICATE:
        //     switch (event) {
        //         case MSM_EVT_LED_INDICATE_END:
        //             new_state = MSM_STATE_IDLE;
        //         break;
        //     break;
        //     }
        // break;
    }
    return new_state;
}



/** @brief Main State Machine executing */
bool msm_execute(void) {

    msm_event_e ReceivedEvent;
    BaseType_t  xStatus;
    /* Block on the RX queue, wait forever for a new message */
    xStatus = xQueueReceive(m_msm_queue_handle, &ReceivedEvent, portMAX_DELAY);
    configASSERT(xStatus == pdPASS);


    /* A new message was received.  Process it now. */
    msm_state_e new_state = msm_process_event(ReceivedEvent);

    /* Transition to new state */
    if (new_state != m_msm_curr_state) {
        msm_on_exit_state(m_msm_curr_state);
        m_msm_curr_state = new_state;
        msm_on_enter_state(m_msm_curr_state);
    }
    return true;
}

static void msm_thread(void* pvParameters) {

    m_msm_curr_state = MSM_STATE_IDLE; /* dummy initial state */

    while (msm_execute()) {
    };
}

/** @brief Create Main State Machine task and queue. */
void msm_init(void) {
    /* The queue is created to hold a maximum of 20 values,
       each of which is large enough to hold a variable of
       type msm_event_e (uint32_t). */
    m_msm_queue_handle= xQueueCreate(5, sizeof(msm_event_e));

    if (m_msm_queue_handle == NULL) {
        
    }
    vQueueAddToRegistry(m_msm_queue_handle, "MsmQ");

#define MSM_PRIORITY 1
    xTaskCreate(&msm_thread, "msm_thread", 4096, NULL, MSM_PRIORITY, m_msm_task);

    ESP_LOGI(__FUNCTION__, "Sentiel Stone MSM task started.");
}

/** @brief Send MSM event message to Main State Machine. */
void send_msm_event(msm_event_e event) {
    BaseType_t xStatus;
    xStatus = xQueueSendToBack(m_msm_queue_handle, &event, portMAX_DELAY);
    configASSERT(xStatus == pdPASS);
}
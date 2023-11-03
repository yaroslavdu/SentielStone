#include <stdbool.h>
#include "msm.h"

static msm_state_e m_msm_curr_state; /* Main State Machine current state */

/** @brief Processing an enter to new MSM state. */
static void msm_on_enter_state(msm_state_e state) {

    set_msm_state_led_indication(state);
    LOGI("New MSM state: %s (%d)", msm_state_to_text(state), state);
    switch (state) {

    case MSM_STATE_IDLE:
        break;

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

    LOGI("MSM process event: %s(%d) in state: %s(%d)", msm_event_to_text(event), event, msm_state_to_text(m_msm_curr_state), m_msm_curr_state);
    msm_state_e new_state = m_msm_curr_state; /* assume not change state if event isn't processed */
    change_usb_state(event);
    switch (m_msm_curr_state) {
        /* ------------- MSM_STATE_IDLE --------------------- */
        case MSM_STATE_IDLE:
            switch (event) {
                case MSM_EVT_USR_BTN_PRESS:
                    new_state = MSM_STATE_WAIT_FOR_NFC_IN_ADMIN_MODE;
                    break;
                case MSM_EVT_OBJECT_DETECTED:
                    new_state = MSM_STATE_WAIT_FOR_NFC;
                    break;
            }
            break;
        /* ------------- MSM_STATE_WAIT_FOR_NFC ------------- */
        case MSM_STATE_WAIT_FOR_NFC:
            switch (event) {
                case MSM_EVT_NFC_FOUND:
                    new_state = MSM_STATE_LED_INDICATE;
                    led_indicate(PASS_INDK);
                break;
                case MSM_EVT_NFC_NOT_FOUND:
                    led_indicate(ALARM_INDK);
                break;
            }
            break;
        case MSM_STATE_WAIT_FOR_NFC_IN_ADMIN_MODE:
        switch (event) {
            case MSM_EVT_USR_BTN_PRESS:
                new_state = MSM_STATE_IDLE;
                break;
            case MSM_EVT_NFC_FOUND:
                storage_tag_add();
                led_indicate(TAG_ADD_INDK);
                break;
            case MSM_EVT_NFC_NOT_FOUND:
                storage_tag_delete();
                led_indicate(TAG_DEL_INDK);
                break;
        }
        case MSM_STATE_LED_INDICATE:
            switch (event) {
                case MSM_EVT_LED_INDICATE_END:
                    new_state = MSM_STATE_IDLE;
                break;
            break;
            }
        break;
    }
}

char* msm_state_to_text(msm_state_e state) {
    static char* state_names[] = {
        "MSM_STATE_IDLE",
        "MSM_STATE_LOW_BATTERY",
        "MSM_STATE_WAIT_FOR_OBJECT",
        "MSM_STATE_WAIT_FOR_NFC",
        "MSM_STATE_WAIT_FOR_NFC_IN_ADMIN_MODE",
        "MSM_STATE_LED_INDICATE",
    };
    //STATIC_ASSERT(sizeof(state_names) / sizeof(state_names[0]) == MSM_STATE_COUNT, "Size of state_names not equal MSM_STATE_COUNT");

    if (state < MSM_STATE_COUNT) {
        return state_names[state];
    } else {
        return "State undefined";
    }
}

char* msm_event_to_text(msm_event_e event) {
    static char* event_names[] = {
        "MSM_EVT_BOOT",
        "MSM_EVT_ERROR",
        "MSM_EVT_NFC_FOUND",
        "MSM_EVT_NFC_NOT_FOUND",
        "MSM_EVT_USR_BTN_PRESS",
        "MSM_EVT_OBJECT_DETECTED",
        "MSM_EVT_LED_INDICATE_END",
};
    //STATIC_ASSERT(sizeof(event_names) / sizeof(event_names[0]) == MSM_EVT_COUNT, "Size of event_names not equal MSM_EVT_COUNT");
    if (event < MSM_EVT_COUNT) {
        return event_names[event];
    } else {
        return "Event undefined";
    }
}

/** @brief Main State Machine executing */
bool msm_execute(void) {
    msm_event_e ReceivedEvent;
    BaseType_t  xStatus;
    /* Block on the RX queue, wait forever for a new message */
    xStatus = xQueueReceive(m_msm_queue_handle, &ReceivedEvent, portMAX_DELAY);
    configASSERT(xStatus == pdPASS);
    UNUSED_VARIABLE(xStatus);

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
    send_msm_event(MSM_EVT_BOOT);

    while (msm_execute()) {
    };
}
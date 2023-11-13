
typedef enum {
    // MSM_EVT_BOOT = 0,                       //  0 - device turned ON
    // MSM_EVT_ERROR,                          //  1 - error event
    MSM_EVT_NFC_FOUND,                      //  2 - battery discharged
    MSM_EVT_NFC_NOT_FOUND,                  //  3 - battery charged
    // MSM_EVT_USR_BTN_PRESS,                  //  4 - idle state time reached N minutes
    MSM_EVT_OBJECT_DETECTED,                //  5 - APP button released at short time (less 1 seconds)
    MSM_EVT_OBJECT_REMOVED,
    MSM_EVT_COUNT
} msm_event_e;

typedef enum {
    MSM_STATE_IDLE = 0,
    // MSM_STATE_LOW_BATTERY,
    MSM_STATE_WAIT_FOR_OBJECT,
    MSM_STATE_WAIT_FOR_NFC,
    MSM_STATE_WAIT_FOR_OBJECT_REMOVING,
    // MSM_STATE_WAIT_FOR_NFC_IN_ADMIN_MODE,
    // MSM_STATE_LED_INDICATE,
    MSM_STATE_COUNT
} msm_state_e;

void msm_init(void);
void send_msm_event(msm_event_e event);
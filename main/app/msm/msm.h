
typedef enum {
    MSM_EVT_OBJECT_DETECTED,                // Smth detected in front of target area
    MSM_EVT_OBJECT_REMOVED,                 // Range clear
    MSM_EVT_NFC_FOUND,                      //  NFC tag found
    MSM_EVT_NFC_NOT_FOUND,                  //  NFC tag not found during timeout
    MSM_EVT_INDICATION_END,
    MSM_EVT_COUNT,
} msm_event_e;

typedef enum {
    MSM_STATE_IDLE = 0,
    MSM_STATE_WAIT_FOR_OBJECT,
    MSM_STATE_WAIT_FOR_NFC,
    MSM_STATE_INDICATE,
    MSM_STATE_WAIT_FOR_OBJECT_REMOVING,
    MSM_STATE_COUNT,
} msm_state_e;

void msm_init(void);
void send_msm_event(msm_event_e event);
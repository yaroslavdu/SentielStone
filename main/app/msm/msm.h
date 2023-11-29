#ifndef _MSM_H_
#define _MSM_H_
typedef enum {
    MSM_EVT_OBJECT_DETECTED,                // Smth detected in front of target area
    MSM_EVT_OBJECT_REMOVED,                 // Range clear
    MSM_EVT_NFC_READ_DONE,                  //  NFC tag found
    MSM_EVT_BTN_SINGLE_PRESS,
    MSM_EVT_COUNT,
} msm_event_e;

typedef enum {
    MSM_STATE_IDLE = 0,
    MSM_STATE_WAIT_FOR_OBJECT,
    MSM_STATE_WAIT_FOR_NFC,
    MSM_STATE_WAIT_FOR_OBJECT_REMOVING,
    MSM_STATE_WAIT_FOR_NFC_IN_ADMIN_MODE,
    MSM_STATE_COUNT,
} msm_state_e;

void msm_init(void);
void send_msm_event(msm_event_e event);

#endif //_MSM_H_
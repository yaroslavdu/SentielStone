#ifndef _CAP_TOUCH_H_
#define _CAP_TOUCH_H_

typedef enum {
    CAP_TOUCH_CMD_IDLE = 0,
    CAP_TOUCH_CMD_DETECT_OBJECT,
    CAP_TOUCH_CMD_DETECT_CLEAR_ZONE,
    CAP_TOUCH_CMD_STOP,
} cap_touch_cmd_e;

void cap_touch_init(void);
void capt_start_scan_object(void);
void capt_start_scan_clear_zone(void);
void capt_stop_scan(void);

#endif //_CAP_TOUCH_H_
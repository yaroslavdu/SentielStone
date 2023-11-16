typedef enum {
    CAP_TOUCH_CMD_IDLE = 0,
    CAP_TOUCH_CMD_DETECT_OBJECT,
    CAP_TOUCH_CMD_DETECT_CLEAR_ZONE,
} cap_touch_cmd_e;

void cap_touch_init(void);
void capt_start_scan_object(void);
void capt_start_scan_clear_zone(void);
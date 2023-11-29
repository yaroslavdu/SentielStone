#ifndef _PN532_USER_H_
#define _PN532_USER_H_

typedef enum {
    NFC_IDLE = 0,
    NFC_START_SCAN,
    NFC_ADMIN_MODE,
    NFC_ADMIN_MODE_STOP,
} nfc_cmd_e;

void nfc_read_start(void);
void nfc_logging_start(void);
void nfc_logging_stop(void);
void pn532_init(void);
#endif //_PN532_USER_H_
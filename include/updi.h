#ifndef UPDI_H_
#define UPDI_H_

#include <stdint.h>
#include "user_periph_setup.h"

#pragma pack(1)
typedef struct {
    char family_id[7];
    char space;
    char nvm_version[3];
    char debug_version[3];
    char dash;
    char osc_info[3];
    char space2;
    char extra[12];
    char terminator;
} __attribute__((packed)) updi_sib_t;


typedef enum {
    UPDI_OK,
    UPDIERR_MODE_CHANGE_FAILED,
    UPDIERR_WRITE_FAILED,
    UPDIERR_INVALID_SIZE,
    UPDIERR_TIMEOUT,
    UPDIERR_NACK
} updi_err_t;



updi_err_t updi_send_break();
updi_err_t updi_get_sib(updi_sib_t *sib);
updi_err_t updi_read_data(uint16_t address, uint8_t *out, uint8_t size);
updi_err_t updi_read_user_row(uint8_t data[32]);
updi_err_t updi_write_user_row(const uint8_t data[32]);
updi_err_t updi_erase_chip();
updi_err_t updi_enter_programming_mode();
void updi_leave_programming_mode();
void updi_reset_device();

#endif // UPDI_H_

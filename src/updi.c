
#include "updi.h"
#include <uart.h>
#include <app_easy_timer.h>
#include <debug.h>
#include "user_app.h"
#include <systick.h>
 

#define UPDI_BREAK 0x00

#define UPDI_LDS 0x00
#define UPDI_STS 0x40
#define UPDI_LD 0x20
#define UPDI_ST 0x60
#define UPDI_LDCS 0x80
#define UPDI_STCS 0xC0
#define UPDI_REPEAT 0xA0
#define UPDI_KEY 0xE0

#define UPDI_PTR 0x00
#define UPDI_PTR_INC 0x04
#define UPDI_PTR_ADDRESS 0x08

#define UPDI_ADDRESS_8 0x00
#define UPDI_ADDRESS_16 0x04
#define UPDI_ADDRESS_24 0x08

#define UPDI_DATA_8 0x00
#define UPDI_DATA_16 0x01
#define UPDI_DATA_24 0x02

#define UPDI_KEY_SIB 0x04
#define UPDI_KEY_KEY 0x00

#define UPDI_KEY_64 0x00
#define UPDI_KEY_128 0x01
#define UPDI_KEY_256 0x02

#define UPDI_SIB_8BYTES UPDI_KEY_64
#define UPDI_SIB_16BYTES UPDI_KEY_128
#define UPDI_SIB_32BYTES UPDI_KEY_256

#define UPDI_REPEAT_BYTE 0x00
#define UPDI_REPEAT_WORD 0x01

#define UPDI_PHY_SYNC 0x55
#define UPDI_PHY_ACK 0x40

// Repeat counter of 1-byte, with off-by-one counting
#define UPDI_MAX_REPEAT_SIZE (0xFF+1) 


//CS and ASI Register Address map
#define UPDI_CS_STATUSA 0x00
#define UPDI_CS_STATUSB 0x01
#define UPDI_CS_CTRLA 0x02
#define UPDI_CS_CTRLB 0x03
#define UPDI_ASI_KEY_STATUS 0x07
#define UPDI_ASI_RESET_REQ 0x08
#define UPDI_ASI_CTRLA 0x09
#define UPDI_ASI_SYS_CTRLA 0x0A
#define UPDI_ASI_SYS_STATUS 0x0B
#define UPDI_ASI_CRC_STATUS 0x0C

#define UPDI_CTRLA_IBDLY_BIT 7
#define UPDI_CTRLB_CCDETDIS_BIT 3
#define UPDI_CTRLB_UPDIDIS_BIT 2

#define UPDI_ASI_STATUSA_REVID 4
#define UPDI_ASI_STATUSB_PESIG 0

#define UPDI_ASI_KEY_STATUS_CHIPERASE 3
#define UPDI_ASI_KEY_STATUS_NVMPROG 4
#define UPDI_ASI_KEY_STATUS_UROWWRITE 5

#define UPDI_ASI_SYS_STATUS_RSTSYS 5
#define UPDI_ASI_SYS_STATUS_INSLEEP 4
#define UPDI_ASI_SYS_STATUS_NVMPROG 3
#define UPDI_ASI_SYS_STATUS_UROWPROG 2
#define UPDI_ASI_SYS_STATUS_LOCKSTATUS 0

#define UPDI_ASI_SYS_CTRLA_UROW_FINAL 1

#define UPDI_RESET_REQ_VALUE 0x59

//FLASH CONTROLLER
#define UPDI_NVMCTRL_CTRLA 0x00
#define UPDI_NVMCTRL_CTRLB 0x01
#define UPDI_NVMCTRL_STATUS 0x02
#define UPDI_NVMCTRL_INTCTRL 0x03
#define UPDI_NVMCTRL_INTFLAGS 0x04
#define UPDI_NVMCTRL_DATAL 0x06
#define UPDI_NVMCTRL_DATAH 0x07
#define UPDI_NVMCTRL_ADDRL 0x08
#define UPDI_NVMCTRL_ADDRH 0x09

//NVMCTRL v0 CTRLA
#define UPDI_V0_NVMCTRL_CTRLA_NOP 0x00
#define UPDI_V0_NVMCTRL_CTRLA_WRITE_PAGE 0x01
#define define UPDI_V0_NVMCTRL_CTRLA_ERASE_PAGE 0x02
#define UPDI_V0_NVMCTRL_CTRLA_ERASE_WRITE_PAGE 0x03
#define UPDI_V0_NVMCTRL_CTRLA_PAGE_BUFFER_CLR 0x04
#define UPDI_V0_NVMCTRL_CTRLA_CHIP_ERASE 0x05
#define UPDI_V0_NVMCTRL_CTRLA_ERASE_EEPROM 0x06
#define UPDI_V0_NVMCTRL_CTRLA_WRITE_FUSE 0x07

//NVMCTRL v2 CTRLA
#define UPDI_V2_NVMCTRL_CTRLA_NOCMD 0x00
#define UPDI_V2_NVMCTRL_CTRLA_FLASH_WRITE 0x02
#define UPDI_V2_NVMCTRL_CTRLA_FLASH_PAGE_ERASE 0x08
#define UPDI_V2_NVMCTRL_CTRLA_EEPROM_ERASE_WRITE 0x13
#define UPDI_V2_NVMCTRL_CTRLA_CHIP_ERASE 0x20
#define UPDI_V2_NVMCTRL_CTRLA_EEPROM_ERASE 0x30

//NVMCTRL v3 CTRLA
#define UPDI_V3_NVMCTRL_CTRLA_NOCMD 0x00
#define UPDI_V3_NVMCTRL_CTRLA_NOP 0x01
#define UPDI_V3_NVMCTRL_CTRLA_FLASH_PAGE_WRITE 0x04
#define UPDI_V3_NVMCTRL_CTRLA_FLASH_PAGE_ERASE_WRITE 0x05
#define UPDI_V3_NVMCTRL_CTRLA_FLASH_PAGE_ERASE 0x08
#define UPDI_V3_NVMCTRL_CTRLA_FLASH_PAGE_BUFFER_CLEAR 0x0F
#define UPDI_V3_NVMCTRL_CTRLA_EEPROM_PAGE_WRITE 0x14
#define UPDI_V3_NVMCTRL_CTRLA_EEPROM_PAGE_ERASE_WRITE 0x15
#define UPDI_V3_NVMCTRL_CTRLA_EEPROM_PAGE_ERASE 0x17
#define UPDI_V3_NVMCTRL_CTRLA_EEPROM_PAGE_BUFFER_CLEAR 0x1F
#define UPDI_V3_NVMCTRL_CTRLA_CHIP_ERASE 0x20
#define UPDI_V3_NVMCTRL_CTRLA_EEPROM_ERASE 0x30




#define UPDI_NVM_STATUS_WRITE_ERROR 2
#define UPDI_NVM_STATUS_EEPROM_BUSY 1
#define UPDI_NVM_STATUS_FLASH_BUSY 0

#define USERDATA_SZ 32
#define USERDATA_ADDR 0x1300
#define KEY_SZ 8


#define updi_send(x) uart_write_byte(UART2, x)



static timer_hnd timer_handle = EASY_TIMER_INVALID_TIMER;

/**
 * Key for unlocking NVM - Its NVMProg' ' backwards (lsb is sent first)
 */
static const uint8_t UPDI_KEY_NVM[KEY_SZ] = {' ', 'g', 'o', 'r', 'P', 'M', 'V', 'N'};
/**
 * Key for unlocking Chip Erase - NVMErase backwards
 */
static const uint8_t UPDI_KEY_CHIPERASE[KEY_SZ] = {'e', 's', 'a', 'r', 'E', 'M', 'V', 'N'};
/**
 * Key for unlocking User Row for write - NVMUs&te backwards
 */
static const uint8_t UPDI_KEY_UROW[KEY_SZ] = {'e', 't', '&', 's', 'U', 'M', 'V', 'N'};




/**
 * data (Read/Writeable) - one for BLE - max 64 bytes long (one Attiny page)
 * operation (Writeable)- write to it to set off an operation - Has two bytes - an id and a command
 * address (Read/Writeable) - The Address/block to use.
 * result (Read/Notifiable) - The outcome of the operation - id and outcome. ID is synced to command.
 * 
 * Operations:
 *   - Read Device Info (possibly a different characteristic)
 *   - Read from Address into data - Can this be instant?  Also requires a length parameter.
 *   - Write from data to Address - Does different things depending on address. Will Erase first if address is flash
 *   - Normal Reset device 
 *   - Reset UPDI
 * 
 * Responses
 *   - In Progress
 *   - Success
 *   - Device not responsive
 *   - Invalid Address
 *   - Invalid Size
 *   - Write failed. 
 * 
 * All operations should copy data from buffer into internal buffer when operating to allow the client to
 * queue the next command. 
 * 
 * All writes to command will fail until previous command have completed.
 */


// static void reset_complete() {
//     // Turn it back into a UART afterwards. 
// }

static uint8_t updi_rev;


static void updi_send_sync() {
    uart_one_wire_tx_en(UART2);
    updi_send(UPDI_PHY_SYNC);
}

static void updi_write_cs_reg(uint8_t reg, uint8_t val) {
    updi_send_sync();
    updi_send(UPDI_STCS | reg);
    uart_write_byte(UART2, val);
}



static updi_err_t updi_read_byte(uint8_t *data, uint32_t timeout) {
    // Wait until received data are available
    updi_err_t err = UPDI_OK;
    systick_start(timeout, false);
    while (!GetBits32(&SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk)) {
        if (uart_data_ready_getf(UART2)) {
            // Read element from the receive FIFO
            *data = uart_read_rbr(UART2);
            goto cleanup;
        }
    }
    err = UPDIERR_TIMEOUT;
cleanup:
    systick_stop();
    return err;
}

static updi_err_t updi_read_cs_reg(uint8_t reg, uint8_t *out) {
    updi_send_sync();
    updi_send(UPDI_LDCS | reg);
    uart_wait_tx_finish(UART2);

    uart_one_wire_rx_en(UART2);
    return updi_read_byte(out, 2000);
}

updi_err_t updi_send_break() {
    DEBUG_PRINT_STRING("Sending Break\r\n");

    // Drive the output low for at least 24.6 millis (recommended by datasheet.).  do it twice
    // We do this by setting the baud to 300 then sending 0x00 twice.
    uart_baudrate_setf(UART2, 0x0D0505);
    uart_one_wire_tx_en(UART2);
    uart_write_byte(UART2, 0x00);
    uart_write_byte(UART2, 0x00);

    // Reset the bad back to normal.
    uart_baudrate_setf(UART2, UART_BAUDRATE_115200);

   
    // Disable Collision Detection
    updi_write_cs_reg(UPDI_CS_CTRLB, (1<<UPDI_CTRLB_CCDETDIS_BIT));

    // Turn on inter-byte-delay
    updi_write_cs_reg(UPDI_CS_CTRLA, (1<<UPDI_CTRLA_IBDLY_BIT));


    // Get status
    return updi_read_cs_reg(UPDI_CS_STATUSA, &updi_rev);
}


bool updi_check_link() {
    uint8_t val;
    updi_err_t err = updi_read_cs_reg(UPDI_CS_STATUSA, &val);
    if (err) {
        return false;
    }
    return val != 0x00;
}


/**
 * @briefChecks whether the NVM PROG flag is up
 * 
 * @return true if the flag is set
 * @return false otherwise
 */
static bool updi_in_programming_mode() {
    uint8_t val;
    updi_err_t err = updi_read_cs_reg(UPDI_ASI_SYS_STATUS, &val);
    if (err) {
        return false;
    }
    return val & (1 << UPDI_ASI_SYS_STATUS_NVMPROG);
}

static bool updi_is_locked() {
    uint8_t val;
    updi_err_t err = updi_read_cs_reg(UPDI_ASI_SYS_STATUS, &val);
    if (err) {
        return true; // I suppose?
    }
    return val & (1 << UPDI_ASI_SYS_STATUS_LOCKSTATUS);
}

static updi_err_t updi_wait_for_unlocked(uint32_t timeout) {
    updi_err_t err = UPDI_OK;
    systick_start(timeout, false);
    while (!GetBits32(&SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk)) {
        if (!updi_is_locked()) {
            goto cleanup;
        }
    }
    err = UPDIERR_TIMEOUT;
cleanup:
    systick_stop();
    return err;
}

/**
 * @brief Resets the device by toggling the reset register.
 * 
 */
void updi_reset_device() {
    updi_write_cs_reg(UPDI_ASI_RESET_REQ, UPDI_RESET_REQ_VALUE);
    updi_write_cs_reg(UPDI_ASI_RESET_REQ, 0x00);
}

static void updi_send_key(const uint8_t key[KEY_SZ]) {
    updi_send_sync();
    updi_send(UPDI_KEY | UPDI_SIB_8BYTES);
    for (int i = 0; i < KEY_SZ; i++) {
        updi_send(key[i]);
    }
    uart_one_wire_rx_en(UART2);
}


/**
 * @brief Erases chip, and unlocks if it was previously locked.
 * 
 */
updi_err_t updi_erase_chip() {
    updi_send_key(UPDI_KEY_CHIPERASE);
    uint8_t key_status;
    updi_err_t err = updi_read_cs_reg(UPDI_ASI_KEY_STATUS, &key_status);

    if (err) {
        return err;
    }

    if ((key_status & (1 << UPDI_ASI_KEY_STATUS_CHIPERASE)) == 0) {
        return UPDIERR_MODE_CHANGE_FAILED;
    }

    updi_reset_device();
    return updi_wait_for_unlocked(50);   
}



updi_err_t updi_enter_programming_mode() {
    if (updi_in_programming_mode()) {
        return UPDI_OK; // Already in programming mode.
    }

    updi_send_key(UPDI_KEY_NVM);
    uint8_t key_status;
    updi_err_t err = updi_read_cs_reg(UPDI_ASI_KEY_STATUS, &key_status);

    if (err) {
        return err;
    }
    if ((key_status & (1 << UPDI_ASI_KEY_STATUS_NVMPROG)) == 0) {
        return UPDIERR_MODE_CHANGE_FAILED;
    }

    updi_reset_device();
    err = updi_wait_for_unlocked(10);
    if (err) {
        return err;
    }

    if (!updi_in_programming_mode()) {
        // Didn't work.
    }
    uart_one_wire_rx_en(UART2);
    return UPDI_OK;
}

void updi_leave_programming_mode() {
    updi_reset_device();
    updi_write_cs_reg(UPDI_CS_CTRLB, (1 << UPDI_CTRLB_UPDIDIS_BIT) | (1 << UPDI_CTRLB_CCDETDIS_BIT));
}

static updi_err_t updi_wait_for_user_row_writeable(uint32_t timeout, bool wait_for_high) {
    bool timer_expired = false;
    uint8_t status;
    updi_err_t err = UPDI_OK;
    systick_start(timeout, false);
    while (!GetBits32(&SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk)) {
        err = updi_read_cs_reg(UPDI_ASI_SYS_STATUS, &status);
        if (err || status & (1 << UPDI_ASI_SYS_STATUS_UROWPROG)) {
            goto cleanup; // Worked!
        }
    }
    err = UPDIERR_TIMEOUT;
cleanup:
    systick_stop();
    return err;
}



static updi_err_t updi_read_buffer(uint8_t *data, uint8_t sz, uint32_t timeout) {
    // Wait until received data are available
    updi_err_t err = UPDI_OK;
    systick_start(timeout, false);

    while (sz--) {     
        while (!uart_data_ready_getf(UART2)) {
            if (GetBits32(&SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk)) {
                err = UPDIERR_TIMEOUT;
                goto cleanup;
            }
        }
        // Read element from the receive FIFO
        *data++ = uart_read_rbr(UART2);
    }
cleanup:
    systick_stop();
    return err;
}


static updi_err_t updi_wait_for_ack() {
    uint8_t response;

    uart_one_wire_rx_en(UART2);
    // Response should come after about 1 ms, so we give it 2.
    updi_err_t err = updi_read_byte(&response, 2000);
    if (!err) {
        if (response != UPDI_PHY_ACK) {
            return UPDIERR_NACK;
        }
    }
    return err;
}

/**
 * @brief Performs data phase of transaction:
 *  1. receive ACK
 *  2. send data
 *
 * @param values bytearray of value(s) to send
 * @return updi_err_t 
 */
updi_err_t updi_st_data_phase(uint8_t *values, size_t sz) {
    updi_err_t err;
    err = updi_wait_for_ack();
    if (err) {
        return err;
    }
    uart_one_wire_tx_en(UART2);
    uart_write_buffer(UART2, values, sz);
    err = updi_wait_for_ack();
    return err;
}


updi_err_t updi_st(uint16_t address, uint8_t data) {
    updi_send_sync();
    updi_send(UPDI_STS | UPDI_ADDRESS_16 | UPDI_DATA_8);
    updi_send(address & 0xFF);
    updi_send(address >> 8);
    return updi_st_data_phase(&data, 1);
}

/**
 * @brief  Set the pointer location
 * 
 * @param address address to write
 * @return updi_err_t 
 */
updi_err_t updi_st_ptr(uint16_t address) {
    updi_send_sync();
    updi_send(UPDI_ST | UPDI_PTR_ADDRESS | UPDI_DATA_16);
    updi_send(address & 0xFF);
    updi_send(address >> 8);
    return updi_wait_for_ack();
}

/**
 * @brief Store a value to the repeat counter
 * 
 * @param num number of repeats requested
 * @return updi_err_t 
 */
void updi_repeat(uint8_t num) {
    num -= 1;
    updi_send_sync();
    updi_send(UPDI_REPEAT | UPDI_REPEAT_BYTE);
    updi_send(num);
}

/**
 * @brief Writes out bytes after the address has been set.  Expects Ack after each byte.
 * 
 * @param data data to store
 * @return updi_err_t 
 */
static updi_err_t updi_st_ptr_inc(uint8_t *data, uint8_t sz) {
    updi_err_t err;
    updi_send_sync();
    updi_send(UPDI_ST | UPDI_PTR_INC |  UPDI_DATA_8);


    while (sz--) {
        uart_one_wire_tx_en(UART2);
        updi_send(*data++);
        err = updi_wait_for_ack(); 
        if (err) {
            return err;
        }
    }
    return UPDI_OK;
}
/**
 * @brief Loads a number of bytes from the pointer location with pointer post-increment
 * 
 * @param data the buffer to read into
 * @param size number of bytes to load
 * @return updi_err_t 
 */
static updi_err_t updi_ld_ptr_inc(uint8_t *data, uint8_t size) {
    updi_send_sync();
    updi_send(UPDI_LD | UPDI_PTR_INC |  UPDI_DATA_8);
    uart_one_wire_rx_en(UART2);

    return updi_read_buffer(data, size, size * 1500);
}

/**
 * @brief Writes a number of bytes to memory on the target device.
 * @param address address to write to
 * @param data data to write
 * @param sz size of data buffer
 */
static updi_err_t updi_write_data(uint16_t address, const uint8_t *data, const uint8_t sz) {
    // Special case of 1 byte
    updi_err_t err;

    if (sz == 1) {
        return updi_st(address, data[0]);
    }
    // Special case of 2 byte
    if (sz == 2) {
        err = updi_st(address, data[0]);
        if (err) {
            return err;
        }
        return updi_st(address + 1, data[1]);
    }

    // Store the address
    err = updi_st_ptr(address);
    if (err) {
        return err;
    }

    // Fire up the repeat
    updi_repeat(sz);
    return updi_st_ptr_inc((uint8_t *) data, (uint8_t) sz);
}


/**
 * @brief Reads a number of bytes of data from UPDI
 * 
 * @param address address to write to
 * @param out 
 * @param size number of bytes to read
 * @return updi_err_t 
 */
updi_err_t updi_read_data(uint16_t address, uint8_t *out, uint8_t size) {
    if (size > UPDI_MAX_REPEAT_SIZE) {
        return UPDIERR_INVALID_SIZE;
    }

    //Store the address
    updi_err_t err = updi_st_ptr(address);
    if (err) {
        return err;
    }

    // Fire up the repeat
    if (size > 1) {
        updi_repeat(size);
    }

    

    // Do the read(s)
    return updi_ld_ptr_inc(out, size);
}


updi_err_t updi_read_user_row(uint8_t data[USERDATA_SZ]) {
    updi_err_t err;

    if (!updi_in_programming_mode()) {
        updi_enter_programming_mode();
    }
    usDelay(1000); // give it a moment...
    return updi_read_data(USERDATA_ADDR, data, USERDATA_SZ);
}

updi_err_t updi_write_user_row(const uint8_t data[USERDATA_SZ]) {
    updi_send_key(UPDI_KEY_UROW);
    uint8_t key_status;
    updi_err_t err = updi_read_cs_reg(UPDI_ASI_KEY_STATUS, &key_status);
    if (err) {
        return err;
    }

    if ((key_status & (1 << UPDI_ASI_KEY_STATUS_UROWWRITE)) == 0) {
        // Didn't take it.
        return UPDIERR_MODE_CHANGE_FAILED;
    }

    updi_reset_device();
    err = updi_wait_for_user_row_writeable(50, true);
    if (err) {
        return err;
    }

    // Write to buffer in ram (32 bytes)
    updi_write_data(USERDATA_ADDR, data, USERDATA_SZ);

    // Finalize the write
    updi_write_cs_reg(UPDI_ASI_SYS_CTRLA, (1 << UPDI_ASI_SYS_CTRLA_UROW_FINAL) | (1 << UPDI_CTRLB_CCDETDIS_BIT));

    err = updi_wait_for_user_row_writeable(50, false);
    if (err) {
        return err;
    }

    // Clear status
    updi_write_cs_reg(UPDI_ASI_KEY_STATUS, (1 << UPDI_ASI_KEY_STATUS_UROWWRITE) | (1 << UPDI_CTRLB_CCDETDIS_BIT));
    updi_reset_device();

    return UPDI_OK;
}


updi_err_t updi_get_sib(updi_sib_t *sib) {
    // Doco says you read 16 bytes, not 32, but it appears as if you need to ask for 32
    updi_send_sync();
    updi_send(UPDI_KEY | UPDI_KEY_SIB | UPDI_SIB_32BYTES);

    // Guard cycles defaults to 128
    uart_one_wire_rx_en(UART2);
    // Read in 16 bytes
    return updi_read_buffer((uint8_t *) sib, 32, 32 * 1500);
}



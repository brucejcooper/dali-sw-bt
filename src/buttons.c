

#include "buttons.h"
#include <i2c.h>
#include "debug.h"
#include "user_periph_setup.h"
#include <app_easy_timer.h>
#include "user_app.h"

/* I/O Direction*/
#define IOEXP_REG_DIRECTION         0x00
#define IOEXP_DIRECTION_INPUT(pin)  IOEXP_BIT(pin)
#define IOEXP_DIRECTION_OUTPUT(pin) 0x00


/* Input Polarity */
#define IOEXP_REG_POLARITY              0x01
#define IOEXP_POLARITY_REVERSED(pin)    IOEXP_BIT(pin)
#define IOEXP_POLARITY_NORMAL(pin)      0x00


/* Whether interrupts are enabled or not for the interrupt.*/
#define IOEXP_REG_INT_ON_CHANGE     0x02
#define IOEXP_INT_ON_CHANGE(pin)    IOEXP_BIT(pin)
#define IOEXP_INT_NONE(pin)         0x00


/* Used by interrupt comparison when REG_COMPARISON is set to DEFAULT_VALUE */
#define IOEXP_REG_DEFAULT_VALUE     0x03


/* Defines how comparisons are done for interrupts */
#define IOEXP_REG_COMPARISON                0x04
#define IOEXP_COMPARISON_DEFAULT_VALUE(pin) IOEXP_BIT(pin)
#define IOEXP_COMPARISON_PREV(pin)          0x00

#define IOEXP_REG_IOCON                         0x05
/* IF active driver and this is 1, interupt is active high, otherwise active low*/
#define IOEXP_IOCON_INT_POLARITY_HIGH           0x02
#define IOEXP_IOCON_INT_POLARITY_LOW            0x00
/* If set, IF set, Interrupt pin is Open Drain */
#define IOEXP_IOCON_INT_OPEN_DRAIN              0x04
#define IOEXP_IOCON_INT_ACTIVE_DRIVE            0x00
/* If set, Slew rate is disabled*/
#define IOEXP_IOCON_DISABLE_SLEW_RATE           0x10
/* If set, sequential read is disabled */
#define IOEXP_IOCON_DISABLE_SEQUENTIAL_READ    0x20



#define IOEXP_REG_PULL_UP           0x06
#define IOEXP_PULL_UP_ENABLE(pin)   IOEXP_BIT(pin)
#define IOEXP_PULL_UP_DISABLE(pin)  0x00


#define IOEXP_REG_INTERRUPT     0x07
#define IOEXP_INTERRUPT_PENDING(pin) IOEXP_BIT(pin)

/* Captures the value of the GPIO port at interrupt time */
#define IOEXP_REG_VAL_AT_INT    0x08

/* Current GPIO Value */
#define IOEXP_REG_GPIO          0x09

/* Output Latch */
#define IOEXP_REG_OUTPUT_LATCH  0x0A


#define SWITCHES_bm 0b00011111
static uint8_t current_buttons = 0xFF;

typedef enum {
    STATE_RELEASED,
    STATE_PRESSED_DEBOUNCING,
    STATE_PRESSED,
    STATE_RELEASED_DEBOUNCING,
    STATE_REPRESSED_DEBOUNCING,
    STATE_WAITING_FOR_REPRESS
} button_state_t;

typedef enum {
    FADE_NOT_ACTIVE,
    FADE_UP,
    FADE_DOWN
} button_fade_direction_t;

typedef struct {
    uint8_t index;
    bool released; // 0 for pressed, 1 for released.
    button_state_t state;
    timer_hnd timer;
    button_fade_direction_t fade;
} button_t;

typedef enum {
    EVT_PRESSED,
    EVT_RELEASED,
    EVT_TIMER_EXPIRED
} button_evt_t;






/**
 * @brief Timings, in 10ms ticks of when things happen with buttons. These should be exposed as characteristics.
 * 
 */
#define DEBOUNCE_TICKS 1
#define KEY_DIM_START_TICKS 50
#define KEY_DIM_REPEAT_TICKS 25
#define REPRESS_TICKS 25




static button_t buttons[5] = {
    { .index = 0, .state = STATE_RELEASED, .timer = EASY_TIMER_INVALID_TIMER, .fade = FADE_NOT_ACTIVE },
    { .index = 1, .state = STATE_RELEASED, .timer = EASY_TIMER_INVALID_TIMER, .fade = FADE_NOT_ACTIVE },
    { .index = 2, .state = STATE_RELEASED, .timer = EASY_TIMER_INVALID_TIMER, .fade = FADE_NOT_ACTIVE },
    { .index = 3, .state = STATE_RELEASED, .timer = EASY_TIMER_INVALID_TIMER, .fade = FADE_NOT_ACTIVE },
    { .index = 4, .state = STATE_RELEASED, .timer = EASY_TIMER_INVALID_TIMER, .fade = FADE_NOT_ACTIVE },
};

static void change_state(button_t *b, button_state_t new_state, uint32_t delay);


static void write_reg(uint8_t reg, uint8_t val) {
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
    i2c_write_byte(reg);                            // Write the Address
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
    i2c_write_byte(val | I2C_STOP);                 // write value, issuing a STOP afterwards. 
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
    while(i2c_is_master_busy());                    // Wait until no master activity

}

static uint8_t read_reg(uint8_t reg) {
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
    i2c_write_byte(reg);                            // Write Address Byte
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
    i2c_write_byte(I2C_CMD | I2C_STOP);             // Trigger a Read, executing a STOP afterwards.  It would seem it is smart enough to recognise the need for a RESTART on its own.

    while(!i2c_get_rx_fifo_level());                // Wait for the received byte.
    return i2c_read_byte();                         // Get the value that was read.
}

static void process_button(button_t *button, button_evt_t evt) {
    switch (button->state) {
        case STATE_RELEASED:
            if (evt == EVT_PRESSED) {
                // Now we're pressed. Debounce for 10 milliseconds.
                DEBUG_PRINT_STRING("FETCH\r\n");
                change_state(button, STATE_PRESSED_DEBOUNCING, DEBOUNCE_TICKS);
            } else {
                WARNING(0);
            }
        break;

        case STATE_PRESSED_DEBOUNCING:
            if (evt==EVT_TIMER_EXPIRED) {              
                if (button->released) {
                    // it was just a <10ms pulse on the line.  Ignore it, and go back to the previous state.
                    change_state(button, STATE_RELEASED, 0);
                } else {
                    // Still pressed.  This means the button is now officially pressed. 
                    // DEBUG_PRINT_STRING("PRESSED\r\n");
                    change_state(button, STATE_PRESSED, KEY_DIM_START_TICKS);
                }
            }
            // Ignore other events (press/release) as that is just bounce noise. 
        break;

        case STATE_RELEASED_DEBOUNCING:
            if (evt==EVT_TIMER_EXPIRED) {              
                if (button->released) {
                    // Still released.
                    if (button->fade == FADE_NOT_ACTIVE) {
                        // We haven't gone into fade mode yet, so treat this as a toggle.
                        DEBUG_PRINT_STRING("TOGGLE LIGHT\r\n"); // TODO could happen as soon as release is detected, instead of here.
                        change_state(button, STATE_RELEASED, 0);
                    } else {
                        // We are fading, so we want to give the user the option to re-press to switch direction.
                        change_state(button, STATE_WAITING_FOR_REPRESS, REPRESS_TICKS);
                    }
                } else {
                    // Button was re-pressed or it was just a <10ms pulse on the line.  Ignore it, and go back to the previous state.
                    button->state = STATE_PRESSED;
                }
            }
            // Ignore other events (press/release) as that is just bounce noise. 
        break;

        case STATE_REPRESSED_DEBOUNCING:
            if (evt==EVT_TIMER_EXPIRED) {              
                if (button->released) {
                    // Button was released or it was just a <10ms pulse on the line.  Bail out to the idle state
                    change_state(button, STATE_RELEASED, 0);
                } else {
                    // Button has been pressed again after being released recently. This reverses the fade direction. 
                    button->fade = (button->fade == FADE_DOWN) ? FADE_UP : FADE_DOWN;
                    // DEBUG_PRINT_STRING("RE-PRESSED\r\n");
                    change_state(button, STATE_PRESSED, KEY_DIM_REPEAT_TICKS);
                }
            }
            // Ignore other events (press/release) as that is just bounce noise. 
        break;

        case STATE_WAITING_FOR_REPRESS:
            if (evt==EVT_TIMER_EXPIRED) {              
                // No repress occurred during timeout.  We're proper released now
                // DEBUG_PRINT_STRING("FINISHED FADE\r\n");
                button->state = STATE_RELEASED;
                button->fade = FADE_NOT_ACTIVE; // Reset Fade Direction.
            } else if (evt==EVT_PRESSED) { 
                change_state(button, STATE_REPRESSED_DEBOUNCING, DEBOUNCE_TICKS);
            }
        break;

        case STATE_PRESSED:
            if (evt == EVT_RELEASED) {
                change_state(button, STATE_RELEASED_DEBOUNCING, DEBOUNCE_TICKS);
            } else if (evt == EVT_TIMER_EXPIRED) {
                if (button->fade == FADE_NOT_ACTIVE) {
                    // Its the first fade, so we need to decide which direction we're going in. This shuold be based on current level.
                    button->fade = FADE_DOWN;
                }
                DEBUG_PRINT_STRING(button->fade == FADE_DOWN ? "FADE_DOWN\r\n" : "FADE_UP\r\n");
                change_state(button, STATE_PRESSED, KEY_DIM_REPEAT_TICKS);
            }
        break;
    }
}


// Poor mans way of tying timers to objects.
void button_1_timer_expired() {
    buttons[0].timer = EASY_TIMER_INVALID_TIMER;
    process_button(&buttons[0], EVT_TIMER_EXPIRED);
}
void button_2_timer_expired() {
    buttons[1].timer = EASY_TIMER_INVALID_TIMER;
    process_button(&buttons[1], EVT_TIMER_EXPIRED);    
}
void button_3_timer_expired() {
    buttons[2].timer = EASY_TIMER_INVALID_TIMER;
    process_button(&buttons[2], EVT_TIMER_EXPIRED);
}
void button_4_timer_expired() {
    buttons[3].timer = EASY_TIMER_INVALID_TIMER;
    process_button(&buttons[3], EVT_TIMER_EXPIRED);
    
}
void button_5_timer_expired() {
    buttons[4].timer = EASY_TIMER_INVALID_TIMER;
    process_button(&buttons[4], EVT_TIMER_EXPIRED);   
}



static void change_state(button_t *b, button_state_t new_state, uint32_t delay) {
    b->state = new_state;
    if (b->timer != EASY_TIMER_INVALID_TIMER) {
        app_easy_timer_cancel(b->timer);
        b->timer = EASY_TIMER_INVALID_TIMER;
    }
    if (delay) {
        timer_callback cb = NULL;
        switch (b->index) {
            case 0:
            cb = button_1_timer_expired;
            break;
            case 1:
            cb = button_2_timer_expired;
            break;
            case 2:
            cb = button_3_timer_expired;
            break;
            case 3:
            cb = button_4_timer_expired;
            break;
            case 4:
            cb = button_5_timer_expired;
            break;
        }
        if (cb) {
            b->timer = app_easy_timer(delay, cb);
            if (b->timer == EASY_TIMER_INVALID_TIMER) {
                DEBUG_PRINT_STRING("COULDN'T assign timer\r\n");
            }
            // TODO check to see that it returned a real timer. 
        }
    }
}


void read_buttons() {
    uint8_t new_buttons = ioexp_reread();
    uint8_t changed = new_buttons ^ current_buttons;
    if (changed) {
        for (int i = 0; i < 5; i++, changed <<=1) {
            if (changed & 1) {
                uint8_t released = (new_buttons >> i)  & 0x01;
                buttons[i].released = released;
                process_button(&buttons[i], released ? EVT_RELEASED : EVT_PRESSED);
            }
        }
        current_buttons = new_buttons;
    }
}


void ioexp_int_callback(void)
{
    read_buttons();
    GPIO_ResetIRQ(GPIO0_IRQn);
}


bool buttons_idle() {
    // DEBUG_PRINT_STRING("Button states =\r\n");
    for (int i = 0; i < 5; i++) {
        // DEBUG_PRINT_STRING(" ");
        // DEBUG_PRINT_INT(buttons[i].state);
        if (buttons[i].state != STATE_RELEASED) {
            return false;
        }
    }
    return true;
}


void ioexp_configure() {
    // Reset the device.
    GPIO_SetInactive(I2C_RESET_PORT, I2C_RESET_PIN);
    usDelay(1);
    GPIO_SetActive(I2C_RESET_PORT, I2C_RESET_PIN);
    usDelay(5);


    // Turn switches into inputs with pullups - last three bits are left for LEDs. 
    write_reg(IOEXP_REG_PULL_UP, SWITCHES_bm);
    write_reg(IOEXP_REG_DIRECTION, SWITCHES_bm);
    write_reg(IOEXP_REG_POLARITY, 0x00);

    // Interrupt on falling edge (when its not 1)
    write_reg(IOEXP_REG_DEFAULT_VALUE, SWITCHES_bm);
    write_reg(IOEXP_REG_COMPARISON, 0); //SWITCHES_bm);
    write_reg(IOEXP_REG_INT_ON_CHANGE, SWITCHES_bm);

    // Disable Auto-increment so we can just read the GPIO over and over.
    write_reg(IOEXP_REG_IOCON, 
                IOEXP_IOCON_INT_ACTIVE_DRIVE | 
                IOEXP_IOCON_INT_POLARITY_LOW | 
                IOEXP_IOCON_DISABLE_SEQUENTIAL_READ
    );


    // Read the value right now, before we set interrupts.  We do this to clear out any existing interrupt.
    // If we didn't do this, it'd interrupt immediately, then wait for the line to drop, which it never would and we'll watchdog out.
    current_buttons = ioexp_get();
    for (int i = 0; i < 5; i++) {
        if ((current_buttons >> i)  & 0x01) {
            change_state(&buttons[i], STATE_RELEASED, 0);
        } else {
            // Already pressed.
            change_state(&buttons[i], STATE_PRESSED_DEBOUNCING, 10);
        }
        
    }

    // GPIO_EnableIRQ(I2C_INT_PORT, I2C_INT_PIN, GPIO0_IRQn, true, true, 0);
}

void buttons_sleep() {}


/**
 * @brief Issues an I2C Read without writing an address first.
 * 
 * @return uint8_t 
 */
uint8_t ioexp_reread() {
    i2c_write_byte(I2C_CMD | I2C_STOP);             // Trigger a Read (Start is implicit), executing a STOP afterwards.
    while(!i2c_get_rx_fifo_level());                // Wait for the fifo to fill
    return i2c_read_byte();                         // Read the value from the register, which will trigger fifo drain.
}


uint8_t ioexp_get() {
    return read_reg(IOEXP_REG_GPIO);
}

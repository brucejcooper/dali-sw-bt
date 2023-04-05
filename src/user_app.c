#include <da1458x_config_basic.h>
#include <da1458x_config_advanced.h>
#include <user_config.h>
#include <rwip_config.h> // SW configuration

#include <wkupct_quadec.h>
#include <user_app.h>
#include <custs1.h>
#include <custs1_task.h>
#include <gpio.h>
#include <user_custs1_def.h>
#include <user_periph_setup.h>
#include <app_task.h>
#include <ble_handlers.h>
#include <debug.h>
#include <spi_flash.h>

#include "updi.h"
#include <uart.h>


void usDelay(uint32_t nof_us)
{
    while ( nof_us-- )
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}



// static void app_wakeup_cb(void) {
//     //
//     // DEBUG_PRINT_STRING("Wakeup!!\r\n");
// }

// void user_app_wakeup_press_cb(void) {
//     // Button Was Pressed
//     // DEBUG_PRINT_STRING("BTN Press\r\n");
// }


/*
This function is called as a callback by system arch, after peripheral init, and app init, but before giving away to the kernel and main loop
*/
void app_on_init(void)
{
  	spi_flash_power_down();

    
    // To keep compatibility call default handler
    default_app_on_init();

    extern uint32_t __StackTop;
    extern uint32_t __HeapBase;
    extern uint32_t __HeapLimit;

    uint32_t initial_sp = (uint32_t)&__StackTop;
    uint32_t heap_base = (uint32_t)&__HeapBase;
    uint32_t heap_limit = (uint32_t)&__HeapLimit;

    DEBUG_PRINT_STRING("app_on_init()\r\n");
    // printf("Compiled: %s %s\r\n", __DATE__, __TIME__);
    // printf("stack: 0x%08lX\r\n", initial_sp);
    // printf("heap: 0x%08lX (0x%04X)\r\n", heap_base, (uint16_t)heap_limit);
}

void app_going_to_sleep(sleep_mode_t sleep_mode) {

    // DEBUG_PRINT_STRING("sleep\r\n");

    // app_easy_wakeup_set(app_wakeup_cb);
    // wkupct_enable_irq(WKUPCT_PIN_SELECT(nWAKE_PORT, nWAKE_PIN), 
    //                   WKUPCT_PIN_POLARITY(nWAKE_PORT, nWAKE_PIN, WKUPCT_PIN_POLARITY_LOW),	// WKUPCT_PIN_POLARITY will make sure the appropriate bit in the register is set.
    //                   1, // how many events must occur before interrupt is generated
    //                   0);																																																												

    // wkupct_register_callback(user_app_wakeup_press_cb);	// sets this function as wake-up interrupt callback
}


void app_resume_from_sleep() {
    // read_buttons();

    // if (!buttons_idle()) {
        // if (ke_state_get(TASK_APP) == APP_CONNECTABLE) {
        //     DEBUG_PRINT_STRING("Starting Advertising\r\n");
        //     default_advertise_operation();
        // }
    // }

}


void app_advertise_complete(const uint8_t status)
{
    // Called when a user connects, or if the advertising stops due to timeout.
    if (status == GAP_ERR_CANCELED)
    {
        DEBUG_PRINT_STRING("Advertising Timeout\r\n");

        // app_easy_wakeup_set(app_wakeup_cb);

    }
}


arch_main_loop_callback_ret_t app_on_system_powered(void)
{
    wdg_reload(1);
   
    // if (buttons_idle()) {
        // return GOTO_SLEEP;
    // } else {
    //     read_buttons();
        return KEEP_POWERED;
    // }
    // return buttons_idle() ? GOTO_SLEEP : KEEP_POWERED;
}

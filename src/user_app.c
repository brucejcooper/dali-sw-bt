#include <da1458x_config_basic.h>
#include <da1458x_config_advanced.h>
#include <user_config.h>
#include <rwip_config.h> // SW configuration

#include <user_app.h>
#include <custs1.h>
#include <custs1_task.h>
#include <gpio.h>
#include <user_custs1_def.h>
#include <user_periph_setup.h>
#include <ble_handlers.h>
#include <debug.h>
#include <spi_flash.h>
#include "buttons.h"

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
    ioexp_configure();
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
    return KEEP_POWERED;

    // return GOTO_SLEEP;
}

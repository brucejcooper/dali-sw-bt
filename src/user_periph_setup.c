/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */
#include <user_periph_setup.h>
#include <datasheet.h>
#include <system_library.h>
#include <rwip_config.h>
#include <syscntl.h>
#include <gpio.h>
#include <spi_flash.h>
#include "i2c.h"
#include <buttons.h>

#include <debug.h>

// Dev Kit flash is 1 Mbit, or 256K.
#define SPI_FLASH_DEV_SIZE          (256 * 1024)





static const spi_flash_cfg_t spi_flash_cfg = {
    .chip_size = SPI_FLASH_DEV_SIZE,
};

static const spi_cfg_t spi_cfg = {
    .spi_ms = SPI_MS_MODE_MASTER,
    .spi_cp = SPI_CP_MODE_0,
    .spi_speed = SPI_SPEED_MODE_4MHz,
    .spi_wsz = SPI_MODE_8BIT,
    .spi_cs = SPI_CS_0,
    .cs_pad.port = GPIO_PORT_0,
    .cs_pad.pin = GPIO_PIN_1,
    .spi_capture = SPI_MASTER_EDGE_CAPTURE,
#if defined (CFG_SPI_DMA_SUPPORT)
    .spi_dma_channel = SPI_DMA_CHANNEL_01,
    .spi_dma_priority = DMA_PRIO_0,
#endif
};


static const i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_FAST,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESSING_7B,
    .address = 0x20, // Address of MCP230008. assuming all address pins are grounded
    .tx_fifo_level = 1,
    .rx_fifo_level = 1
};



#if DEVELOPMENT_DEBUG
void GPIO_reservations(void)
{
    // I2C
    RESERVE_GPIO( I2C_SCL, I2C_SCL_PORT, I2C_SCL_PIN, PID_I2C_SCL);
    RESERVE_GPIO( I2C_SDA, I2C_SDA_PORT, I2C_SDA_PIN, PID_I2C_SDA);
    RESERVE_GPIO( I2C_INT, I2C_INT_PORT, I2C_INT_PIN, FUNC_GPIO);
    RESERVE_GPIO( I2C_RESET, I2C_RESET_PORT, I2C_RESET_PIN, FUNC_GPIO);


#if defined (CFG_SPI_FLASH_ENABLE)
    // SPI FLASH
    RESERVE_GPIO(SPI_EN,  SPI_EN_PORT,  SPI_EN_PIN,  PID_SPI_EN);
    RESERVE_GPIO(SPI_CLK, SPI_CLK_PORT, SPI_CLK_PIN, PID_SPI_CLK);
    RESERVE_GPIO(SPI_DO,  SPI_DO_PORT,  SPI_DO_PIN,  PID_SPI_DO);
    RESERVE_GPIO(SPI_DI,  SPI_DI_PORT,  SPI_DI_PIN,  PID_SPI_DI);
#endif

}
#endif

static void set_pad_functions(void)
{
#if defined (CFG_SPI_FLASH_ENABLE)
    // SPI Flash
    GPIO_ConfigurePin(SPI_EN_PORT,  SPI_EN_PIN,  OUTPUT, PID_SPI_EN,  true);
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(SPI_DO_PORT,  SPI_DO_PIN,  OUTPUT, PID_SPI_DO,  false);
    GPIO_ConfigurePin(SPI_DI_PORT,  SPI_DI_PIN,  INPUT,  PID_SPI_DI,  false);
#endif

    // I2C
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, INPUT_PULLUP, PID_I2C_SCL, false);
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT_PULLUP, PID_I2C_SDA, false);
    GPIO_ConfigurePin(I2C_INT_PORT, I2C_INT_PIN, INPUT, FUNC_GPIO, false);
    GPIO_ConfigurePin(I2C_RESET_PORT, I2C_RESET_PIN, OUTPUT, FUNC_GPIO, true); // chip is reset on low


}


void periph_init(void)
{
    // Disable HW RST on P0_0
    GPIO_Disable_HW_Reset();
    // Enable DC/DC buck mode
    if (syscntl_dcdc_turn_on_in_buck(SYSCNTL_DCDC_LEVEL_1V1) != 0)
    {
        __BKPT(0);
    }

    // ROM patch
    patch_func();

    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);

#ifdef DEBUG_SEGGER
    static bool isSeggerInitialized = false;
    if (!isSeggerInitialized)
    {
        // Set up JLink RTT
        SEGGER_RTT_Init();
        DEBUG_PRINT_STRING("periph_init()\r\n");

        isSeggerInitialized = true;
    }
#endif

    // We use i2c for GPIO on a MCP230008
    i2c_init(&i2c_cfg);
    GPIO_RegisterCallback(GPIO0_IRQn, ioexp_int_callback);


#if defined (CFG_SPI_FLASH_ENABLE)
    spi_flash_configure_env(&spi_flash_cfg);

    // Initialize SPI
    spi_initialize(&spi_cfg);
#endif

}

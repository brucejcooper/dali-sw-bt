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
#include <uart.h>

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


// Configuration struct for UART
static const uart_cfg_t uart_cfg = {
    // Set Baud Rate
    .baud_rate = UART_BAUDRATE_115200,
    // Set data bits
    .data_bits = UART_DATABITS_8,
    // Set parity
    .parity = UART_PARITY_EVEN,
    // Set stop bits
    .stop_bits = UART_STOPBITS_2,
    // Set flow control
    .auto_flow_control = UART_AFCE_DIS,
    // Set FIFO enable
    .use_fifo = UART_FIFO_EN,
    // Set Tx FIFO trigger level
    .tx_fifo_tr_lvl = UART_TX_FIFO_LEVEL_0,
    // Set Rx FIFO trigger level
    .rx_fifo_tr_lvl = UART_RX_FIFO_LEVEL_0,
    // Set interrupt priority
    .intr_priority = 2,
#if defined (CFG_UART_DMA_SUPPORT)
    // Set UART DMA Channel Pair Configuration
    .uart_dma_channel = UART_DMA_CHANNEL_01,
    // Set UART DMA Priority
    .uart_dma_priority = DMA_PRIO_0,
#endif
};



#if DEVELOPMENT_DEBUG
void GPIO_reservations(void)
{
    // I2C
    RESERVE_GPIO( UPDI, UPDI_PORT, UPDI_PIN, PID_UART2_TX);
    RESERVE_GPIO( nWAKE, nWAKE_PORT, nWAKE_PIN, PID_GPIO);

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

    GPIO_ConfigurePin(UPDI_PORT,  UPDI_PIN, OUTPUT, PID_UART2_TX, false);
    
    GPIO_ConfigurePin(nWAKE_PORT,  nWAKE_PIN, INPUT, PID_GPIO,  false);
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

    uart_initialize(UART2, &uart_cfg);


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

#if defined (CFG_SPI_FLASH_ENABLE)
    spi_flash_configure_env(&spi_flash_cfg);

    // Initialize SPI
    spi_initialize(&spi_cfg);
#endif

    // UPDI baud is autodetected (Max 225 Kbit when at default 4Mhz clock), with 2 stop bits and even parity
    uart_one_wire_enable(UART2, UPDI_PORT, UPDI_PIN);

    // Break and idle are special
    // Break (held low) minimum duration = 24.6Ms
    // Idle 
}

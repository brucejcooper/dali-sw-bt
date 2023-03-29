/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
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

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "da1458x_periph_setup.h"
#include "uart.h"
#include "gpio.h"




#define SPI_EN_PORT GPIO_PORT_0
#define SPI_EN_PIN GPIO_PIN_1
#define SPI_CLK_PORT GPIO_PORT_0
#define SPI_CLK_PIN GPIO_PIN_4
#define SPI_DO_PORT GPIO_PORT_0
#define SPI_DO_PIN GPIO_PIN_0
#define SPI_DI_PORT GPIO_PORT_0
#define SPI_DI_PIN GPIO_PIN_3

#define I2C_SCL_PORT                GPIO_PORT_0
#define I2C_SCL_PIN                 GPIO_PIN_11

#define I2C_SDA_PORT                GPIO_PORT_0
#define I2C_SDA_PIN                 GPIO_PIN_8

// Use P05 as the Interrupt line - it leaves SWD and SPI free, and is the only Type-B port left.  For production we could turn of JTAG and us P02 insetad
#define I2C_INT_PORT                GPIO_PORT_0
#define I2C_INT_PIN                 GPIO_PIN_5

// Use P09 as the chip reset line.
#define I2C_RESET_PORT                GPIO_PORT_0
#define I2C_RESET_PIN                 GPIO_PIN_7


void periph_init(void);
void GPIO_reservations(void);



#endif // _USER_PERIPH_SETUP_H_

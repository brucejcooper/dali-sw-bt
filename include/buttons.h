#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <stdint.h>

#define IOEXP_BIT(pin) (1 << pin)


void ioexp_configure();
uint8_t ioexp_get();
uint8_t ioexp_reread();

#endif
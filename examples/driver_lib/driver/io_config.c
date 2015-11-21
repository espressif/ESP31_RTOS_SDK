/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "espressif/esp_common.h"
#include "freertos/portmacro.h"
#include "gpio.h"

/*
 * set gpio input to a signal
 * one gpio can input to several signals
 * If gpio == 0x30, cancel input to the signal, input 0 to signal
 * If gpio == 0x38, cancel input to the signal, input 1 to signal, for I2C pad
 */
void gpio_matrix_in(uint32 gpio, uint32 signal_idx)
{
    uint32 addr = GPIO_FUNC_IN_SEL0 + ((signal_idx / 5) * 4);
    uint32 shift = (signal_idx % 5) * 6;

    portENTER_CRITICAL();

    SET_PERI_REG_BITS(addr, 0x3f, gpio, shift);//0x30 0 0x38 1

    if ((signal_idx < 6) || ((signal_idx < 16) && (signal_idx > 7)) || ((signal_idx < 19) && (signal_idx > 16)) || ((signal_idx < 69) && (signal_idx > 62))) {
        SET_PERI_REG_MASK(SIG_FUNC_IN_SEL, 1 << signal_idx);
    }

    portEXIT_CRITICAL();
}

/*
 * set signal output to gpio
 * one signal can output to several gpios
 * If signal_idx == 0x80, cancel output put to the gpio
 */
#define INVALID_SIGNAL 0x80
void gpio_matrix_out(uint32 gpio, uint32 signal_idx)
{
    uint32 addr = GPIO_FUNC_OUT_SEL0 + ((gpio / 4) * 4);
    uint32 shift = (gpio % 4) * 8;
    uint32 pin_reg;

    if (gpio >= GPIO_PIN_COUNT) {
        return;
    }

    portENTER_CRITICAL();

    if (gpio < 32) {
        pin_reg = GPIO_REG_READ(GPIO_ENABLE);

        if (INVALID_SIGNAL == signal_idx) {
            pin_reg &= ~(1 << gpio);
        } else {
            pin_reg |= (1 << gpio);
        }

        GPIO_REG_WRITE(GPIO_ENABLE, pin_reg);
    } else {
        pin_reg = GPIO_REG_READ(GPIO_ENABLE1);

        if (INVALID_SIGNAL == signal_idx) {
            pin_reg &= ~(1 << (gpio - 32));
        } else {
            pin_reg |= (1 << (gpio - 32));
        }

        GPIO_REG_WRITE(GPIO_ENABLE1, pin_reg);
    }

    SET_PERI_REG_BITS(addr, 0xff, signal_idx, shift);//0x80

    portEXIT_CRITICAL();
}

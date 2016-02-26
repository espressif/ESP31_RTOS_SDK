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


#ifndef _ETS_SYS_H
#define _ETS_SYS_H

#define ETS_WMAC_SOURCE          0
#define ETS_SLC_SOURCE           1
#define ETS_UART_SOURCE         13
#define ETS_UART1_SOURCE        14
#define ETS_FRC_TIMER2_SOURCE   43
#define ETS_PCNT_SOURCE         19
#define HW_GPIO_INUM            ETS_PCNT_SOURCE

#define ETS_WMAC_INUM       27
#define ETS_SLC_INUM        1
#define ETS_SPI_INUM        2
#define ETS_HSIP_INUM       2
#define ETS_I2S_INUM        2
#define ETS_RTC_INUM        3
#define ETS_FRC_TIMER1_INUM 9  /* use edge*/
#define ETS_FRC_TIMER2_INUM 10 /* use edge*/
#define ETS_WDT_INUM        8  /* use edge*/
#define ETS_GPIO_INUM       4
#define ETS_UART_INUM       5
#define ETS_UART1_INUM      5
#define ETS_MAX_INUM        6

#define ETS_INTR_LOCK()     portENTER_CRITICAL()

#define ETS_INTR_UNLOCK()   portEXIT_CRITICAL()

#endif /* _ETS_SYS_H */

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


#ifndef _EAGLE_SOC_H_
#define _EAGLE_SOC_H_

//Register Bits{{
#define BIT31   0x80000000
#define BIT30   0x40000000
#define BIT29   0x20000000
#define BIT28   0x10000000
#define BIT27   0x08000000
#define BIT26   0x04000000
#define BIT25   0x02000000
#define BIT24   0x01000000
#define BIT23   0x00800000
#define BIT22   0x00400000
#define BIT21   0x00200000
#define BIT20   0x00100000
#define BIT19   0x00080000
#define BIT18   0x00040000
#define BIT17   0x00020000
#define BIT16   0x00010000
#define BIT15   0x00008000
#define BIT14   0x00004000
#define BIT13   0x00002000
#define BIT12   0x00001000
#define BIT11   0x00000800
#define BIT10   0x00000400
#define BIT9    0x00000200
#define BIT8    0x00000100
#define BIT7    0x00000080
#define BIT6    0x00000040
#define BIT5    0x00000020
#define BIT4    0x00000010
#define BIT3    0x00000008
#define BIT2    0x00000004
#define BIT1    0x00000002
#define BIT0    0x00000001
//}}

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr)   (addr)

#define READ_PERI_REG(addr)             (*((volatile uint32 *)ETS_UNCACHED_ADDR(addr)))
#define WRITE_PERI_REG(addr, val)       (*((volatile uint32 *)ETS_UNCACHED_ADDR(addr))) = (uint32)(val)
#define CLEAR_PERI_REG_MASK(reg, mask)  WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~(mask))))
#define SET_PERI_REG_MASK(reg, mask)    WRITE_PERI_REG((reg), (READ_PERI_REG(reg) | (mask)))
#define GET_PERI_REG_MASK(reg, mask)            (READ_PERI_REG(reg) & (mask))
#define GET_PERI_REG_BITS(reg, hipos,lowpos)    ((READ_PERI_REG(reg) >> (lowpos)) & ((1 << ((hipos) - (lowpos) + 1)) - 1))
#define SET_PERI_REG_BITS(reg, bit_map, value, shift) (WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~((bit_map) << (shift)))) | (((value) & bit_map)<<(shift)) ))
#define GET_PERI_REG_BITS2(reg, mask,shift)      ((READ_PERI_REG(reg) >> (shift)) & (mask))
//}}

//Periheral Clock {{
#define  APB_CLK_FREQ                                80*1000000       //unit: Hz
#define  CPU_CLK_FREQ                                APB_CLK_FREQ
#define  UART_CLK_FREQ                               APB_CLK_FREQ

//Peripheral device base address define{{
#define PERIPHS_DPORT_BASEADDR          0x3ff00000
#define PERIPHS_UART_BASEADDR           0x60000000
#define PERIPHS_HSPI_BASEADDR           0x60002000
#define PERIPHS_SPI_BASEADDR            0x60003000
#define PERIPHS_GPIO_BASEADDR           0x60004000
#define PERIPHS_GPIO_SD_BASEADDR        0x60004f00
#define PERIPHS_TIMER_BASEDDR           0x60007000
#define PERIPHS_RTC_BASEADDR            0x60008000
#define DR_REG_RTCIO_BASE               0x60008400
#define PERIPHS_IO_MUX                  0x60009000
#define PERIPHS_WDT_BASEADDR            0x6000a000
#define PERIPHS_HINF_BASEADDR           0x6000b000
#define PERIPHS_SLC_BASEADDR            0x60018000
#define PERIPHS_I2C_BASEADDR            0x6000e000
#define PERIPHS_I2S_BASEADDR            0x6000f000
#define PERIPHS_UART1_BASEADDR          0x60010000
#define PERIPHS_SPI_ENCRYPT_BASEADDR    0x6001b000
//}}

#define REG_SPI_BASE(i)                 PERIPHS_SPI_BASEADDR
#define DR_REG_GPIO_BASE                PERIPHS_GPIO_BASEADDR
//hard cold
#define EDGE_INT_ENABLE_REG             (PERIPHS_DPORT_BASEADDR + 0x04)
#define WDT_EDGE_INT_DISABLE()          CLEAR_PERI_REG_MASK(EDGE_INT_ENABLE_REG, BIT0)
#define DR_REG_RTCCNTL_BASE             PERIPHS_RTC_BASEADDR
#define DR_REG_IO_MUX_BASE              PERIPHS_IO_MUX
#define DR_REG_PRODPORT_BASE            PERIPHS_DPORT_BASEADDR
//#define CACHE_MEM_EN_REG              (PERIPHS_DPORT_BASEADDR+0x24)
#define DR_REG_APPDPORT_BASE            PERIPHS_DPORT_BASEADDR
#define DR_REG_GPIO_SD_BASE             PERIPHS_GPIO_SD_BASEADDR
#define DR_REG_PCNT_BASE                0x60017000
#define DR_REG_LEDC_BASE                0x60019000

#endif //_EAGLE_SOC_H_

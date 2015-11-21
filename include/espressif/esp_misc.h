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

#ifndef __ESP_MISC_H__
#define __ESP_MISC_H__

#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Misc_APIs Misc APIs
  * @brief misc APIs
  */

/** @addtogroup Misc_APIs
  * @{
  */

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"

void ets_delay_us(uint16 us);

void ets_install_putc1(void (*p)(char c));

void ets_putc(char c);

void ets_printf(const char *fmt, ...);

void *ets_memset(void *dest, int val, unsigned int nbyte);

void *ets_memcpy(void *dest, const void *src, unsigned int nbyte);

void *ets_memmove(void *dest, const void *src, unsigned int nbyte);

int ets_memcmp(const void *string1, const void *string2, unsigned int nbyte);

char *ets_strcpy(char *s1, const char *s2);

char *ets_strncpy(char *s1, const char *s2, unsigned int n);

int ets_strcmp(const char *s1, const char *s2);

int ets_strncmp(const char *s1, const char *s2, unsigned int n);

int ets_strlen(const char *s);

char *ets_strstr(const char *, const char *);

void ets_bzero(void *, size_t);

enum dhcp_status {
    DHCP_STOPPED,   /**< disable DHCP */
    DHCP_STARTED    /**< enable DHCP */
};

struct dhcps_lease {
    bool enable;                /**< enable DHCP lease or not */
    struct ip_addr start_ip;    /**< start IP of IP range */
    struct ip_addr end_ip;      /**< end IP of IP range */
};

enum dhcps_offer_option {
    OFFER_START = 0x00,         /**< DHCP offer option start */
    OFFER_ROUTER = 0x01,        /**< DHCP offer router, only support this option now */
    OFFER_END                   /**< DHCP offer option start */
};

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

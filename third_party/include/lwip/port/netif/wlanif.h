/*
 *  Copyright (c) 2010-2011 	Espressif System
 *   
*/

#ifndef _WLAN_LWIP_IF_H_
#define _WLAN_LWIP_IF_H_

#include "lwip/err.h"

#include "esp_wifi.h"

err_t wlanif_init(struct netif *netif);

void wlanif_input(struct netif *netif, void *buffer, uint16 len, void* eb);

bool ieee80211_output(WIFI_INTERFACE wifi_if, void *buffer, uint16 len);

WIFI_INTERFACE wifi_get_interface(void *dev);

#endif /*  _WLAN_LWIP_IF_H_ */

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

#ifndef __ESP_WIFI_H__
#define __ESP_WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WiFi_Common_APIs Common APIs
  * @brief WiFi common APIs
  *
  *        The Flash system parameter area is the last 16KB of the Flash.
  *
  */

/** @addtogroup WiFi_Common_APIs
  * @{
  */

typedef enum {
    NULL_MODE = 0,      /**< null mode */
    STATION_MODE,       /**< WiFi station mode */
    SOFTAP_MODE,        /**< WiFi soft-AP mode */
    STATIONAP_MODE,     /**< WiFi station + soft-AP mode */
    MAX_MODE
} WIFI_MODE;

typedef enum {
    AUTH_OPEN = 0,      /**< authenticate mode : open */
    AUTH_WEP,           /**< authenticate mode : WEP */
    AUTH_WPA_PSK,       /**< authenticate mode : WPA_PSK */
    AUTH_WPA2_PSK,      /**< authenticate mode : WPA2_PSK */
    AUTH_WPA_WPA2_PSK,  /**< authenticate mode : WPA_WPA2_PSK */
    AUTH_MAX
} AUTH_MODE;

/**
  * @brief  Get the current operating mode of the WiFi.
  *
  * @param  null
  *
  * @return WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  */
WIFI_MODE wifi_get_opmode(void);

/**
  * @brief  Get the operating mode of the WiFi saved in the Flash.
  *
  * @param  null
  *
  * @return WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  */
WIFI_MODE wifi_get_opmode_default(void);

/**
  * @brief     Set the WiFi operating mode, and save it to Flash.
  *
  *            Set the WiFi operating mode as station, soft-AP or station+soft-AP,
  *            and save it to Flash. The default mode is soft-AP mode.
  *
  * @attention This configuration will be saved in the Flash system parameter area if changed.
  *
  * @param     uint8 opmode : WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_set_opmode(WIFI_MODE opmode);

/**
  * @brief  Set the WiFi operating mode, and will not save it to Flash.
  *
  *         Set the WiFi operating mode as station, soft-AP or station+soft-AP, and
  *         the mode won't be saved to the Flash.
  *
  * @param  uint8 opmode : WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_opmode_current(WIFI_MODE opmode);

typedef enum {
    STATION_IF = 0, /**< ESP32 station interface */
    SOFTAP_IF,      /**< ESP32 soft-AP interface */
    MAX_IF
} WIFI_INTERFACE;

struct ip_info {
    struct ip_addr ip;      /**< IP address */
    struct ip_addr netmask; /**< netmask */
    struct ip_addr gw;      /**< gateway */
};

/**
  * @brief     Get the IP address of the ESP32 WiFi station or the soft-AP interface.
  *
  * @attention Users need to enable the target interface (station or soft-AP) by wifi_set_opmode first.
  *
  * @param     WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                      0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param     struct ip_info *info : the IP information obtained.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_get_ip_info(WIFI_INTERFACE if_index, struct ip_info *info);

/**
  * @brief     Set the IP address of the ESP32 WiFi station or the soft-AP interface.
  *
  * @attention 1. Users need to enable the target interface (station or soft-AP) by
  *               wifi_set_opmode first.
  * @attention 2. To set static IP, users need to disable DHCP first (wifi_station_dhcpc_stop
  *               or wifi_softap_dhcps_stop):
  *    - If the DHCP is enabled, the static IP will be disabled; if the static IP is enabled,
  *      the DHCP will be disabled. It depends on the latest configuration.
  *
  * @param  WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                   0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param  struct ip_info *info : the IP information obtained.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_ip_info(WIFI_INTERFACE if_index, struct ip_info *info);

/**
  * @brief  Get MAC address of the ESP32 WiFi station or the soft-AP interface.
  *
  * @param  WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                   0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param  uint8 *macaddr : the MAC address.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_get_macaddr(WIFI_INTERFACE if_index, uint8 *macaddr);

/**
  * @brief     Set MAC address of the ESP32 WiFi station or the soft-AP interface.
  *
  * @attention 1. This API can only be called in user_init.
  * @attention 2. Users need to enable the target interface (station or soft-AP) by wifi_set_opmode first.
  * @attention 3. ESP32 soft-AP and station have different MAC addresses, do not set them to be the same.
  *    - The bit0 of the first byte of ESP32 MAC address can not be 1. For example, the MAC address
  *      can set to be "1a:XX:XX:XX:XX:XX", but can not be "15:XX:XX:XX:XX:XX".
  *
  * @param     WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                   0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param     uint8 *macaddr : the MAC address.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_set_macaddr(WIFI_INTERFACE if_index, uint8 *macaddr);

typedef enum {
    EVENT_STAMODE_SCAN_DONE = 0,        /**< ESP32 station finish scanning AP */
    EVENT_STAMODE_CONNECTED,            /**< ESP32 station connected to AP */
    EVENT_STAMODE_DISCONNECTED,         /**< ESP32 station disconnected to AP */
    EVENT_STAMODE_AUTHMODE_CHANGE,      /**< the auth mode of AP connected by ESP32 station changed */
    EVENT_STAMODE_GOT_IP,               /**< ESP32 station got IP from connected AP */
    EVENT_STAMODE_DHCP_TIMEOUT,         /**< ESP32 station dhcp client got IP timeout */
    EVENT_SOFTAPMODE_STACONNECTED,      /**< a station connected to ESP32 soft-AP */
    EVENT_SOFTAPMODE_STADISCONNECTED,   /**< a station disconnected to ESP32 soft-AP */
    EVENT_SOFTAPMODE_PROBEREQRECVED,    /**< Receive probe request packet in soft-AP interface */
    EVENT_MAX
} SYSTEM_EVENT;

enum {
    REASON_UNSPECIFIED              = 1,
    REASON_AUTH_EXPIRE              = 2,
    REASON_AUTH_LEAVE               = 3,
    REASON_ASSOC_EXPIRE             = 4,
    REASON_ASSOC_TOOMANY            = 5,
    REASON_NOT_AUTHED               = 6,
    REASON_NOT_ASSOCED              = 7,
    REASON_ASSOC_LEAVE              = 8,
    REASON_ASSOC_NOT_AUTHED         = 9,
    REASON_DISASSOC_PWRCAP_BAD      = 10,
    REASON_DISASSOC_SUPCHAN_BAD     = 11,
    REASON_IE_INVALID               = 13,
    REASON_MIC_FAILURE              = 14,
    REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
    REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
    REASON_IE_IN_4WAY_DIFFERS       = 17,
    REASON_GROUP_CIPHER_INVALID     = 18,
    REASON_PAIRWISE_CIPHER_INVALID  = 19,
    REASON_AKMP_INVALID             = 20,
    REASON_UNSUPP_RSN_IE_VERSION    = 21,
    REASON_INVALID_RSN_IE_CAP       = 22,
    REASON_802_1X_AUTH_FAILED       = 23,
    REASON_CIPHER_SUITE_REJECTED    = 24,

    REASON_BEACON_TIMEOUT           = 200,
    REASON_NO_AP_FOUND              = 201,
    REASON_AUTH_FAIL                = 202,
    REASON_ASSOC_FAIL               = 203,
    REASON_HANDSHAKE_TIMEOUT        = 204,
};

typedef struct {
    uint32 status;          /**< status of scanning APs*/
    struct bss_info *bss;   /**< list of APs found*/
} Event_StaMode_ScanDone_t;

typedef struct {
    uint8 ssid[32];         /**< SSID of connected AP */
    uint8 ssid_len;         /**< SSID length of connected AP */
    uint8 bssid[6];         /**< BSSID of connected AP*/
    uint8 channel;          /**< channel of connected AP*/
} Event_StaMode_Connected_t;

typedef struct {
    uint8 ssid[32];         /**< SSID of disconnected AP */
    uint8 ssid_len;         /**< SSID length of disconnected AP */
    uint8 bssid[6];         /**< BSSID of disconnected AP */
    uint8 reason;           /**< reason of disconnection */
} Event_StaMode_Disconnected_t;

typedef struct {
    uint8 old_mode;         /**< the old auth mode of AP */
    uint8 new_mode;         /**< the new auth mode of AP */
} Event_StaMode_AuthMode_Change_t;

typedef struct {
    struct ip_addr ip;      /**< IP address that ESP32 station got from connected AP */
    struct ip_addr mask;    /**< netmask that ESP32 station got from connected AP */
    struct ip_addr gw;      /**< gateway that ESP32 station got from connected AP */
} Event_StaMode_Got_IP_t;

typedef struct {
    uint8 mac[6];           /**< MAC address of the station connected to ESP32 soft-AP */
    uint8 aid;              /**< the aid that ESP32 soft-AP gives to the station connected to  */
} Event_SoftAPMode_StaConnected_t;

typedef struct {
    uint8 mac[6];           /**< MAC address of the station disconnects to ESP32 soft-AP */
    uint8 aid;              /**< the aid that ESP32 soft-AP gave to the station disconnects to  */
} Event_SoftAPMode_StaDisconnected_t;

typedef struct {
    int rssi;               /**< Received probe request signal strength */
    uint8 mac[6];           /**< MAC address of the station which send probe request */
} Event_SoftAPMode_ProbeReqRecved_t;

typedef union {
    Event_StaMode_ScanDone_t            scan_done;          /**< ESP32 station scan (APs) done */
    Event_StaMode_Connected_t           connected;          /**< ESP32 station connected to AP */
    Event_StaMode_Disconnected_t        disconnected;       /**< ESP32 station disconnected to AP */
    Event_StaMode_AuthMode_Change_t     auth_change;        /**< the auth mode of AP ESP32 station connected to changed */
    Event_StaMode_Got_IP_t              got_ip;             /**< ESP32 station got IP */
    Event_SoftAPMode_StaConnected_t     sta_connected;      /**< a station connected to ESP32 soft-AP */
    Event_SoftAPMode_StaDisconnected_t  sta_disconnected;   /**< a station disconnected to ESP32 soft-AP */
    Event_SoftAPMode_ProbeReqRecved_t   ap_probereqrecved;  /**< ESP32 softAP receive probe request packet */
} Event_Info_u;

typedef struct _esp_event {
    SYSTEM_EVENT event_id;      /**< even ID */
    Event_Info_u event_info;    /**< event information */
} System_Event_t;

/**
  * @brief      The Wi-Fi event handler.
  *
  * @attention No complex operations are allowed in callback. 
  *                 If users want to execute any complex operations, please post message to another task instead.  
  *
  * @param      System_Event_t *event : WiFi event
  *
  * @return     null
  */
typedef void (* wifi_event_handler_cb_t)(System_Event_t *event);

/**
  * @brief  Register the Wi-Fi event handler.
  *
  * @param  wifi_event_handler_cb_t cb : callback function
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_event_handler_cb(wifi_event_handler_cb_t cb);

/**
  * @}
  */


/** \defgroup WiFi_Sniffer_APIs Sniffer APIs
  * @brief WiFi sniffer APIs
  */

/** @addtogroup WiFi_Sniffer_APIs
  * @{
  */

/**
  * @brief The RX callback function in the promiscuous mode.
  *
  *        Each time a packet is received, the callback function will be called.
  *
  * @param uint8 *buf : the data received
  * @param uint16 len : data length
  *
  * @return null
  */
typedef void (* wifi_promiscuous_cb_t)(uint8 *buf, uint16 len);

/**
  * @brief Register the RX callback function in the promiscuous mode.
  *
  *        Each time a packet is received, the registered callback function will be called.
  *
  * @param wifi_promiscuous_cb_t cb : callback
  *
  * @return null
  */
void wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

/**
  * @brief  Get the channel number for sniffer functions.
  *
  * @param  null
  *
  * @return channel number
  */
uint8 wifi_get_channel(void);

/**
  * @brief  Set the channel number for sniffer functions.
  *
  * @param  uint8 channel :  channel number
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_channel(uint8 channel);

/**
  * @brief     Enable the promiscuous mode.
  *
  * @attention 1. The promiscuous mode can only be enabled in the ESP32 station mode.
  * @attention 2. When in the promiscuous mode, the ESP32 station and soft-AP are disabled.
  * @attention 3. Call wifi_station_disconnect to disconnect before enabling the promiscuous mode.
  * @attention 4. Don't call any other APIs when in the promiscuous mode. Call
  *               wifi_promiscuous_enable(0) to quit sniffer before calling other APIs.
  *
  * @param     uint8 promiscuous :
  *    - 0: to disable the promiscuous mode
  *    - 1: to enable the promiscuous mode
  *
  * @return    null
  */
void wifi_promiscuous_enable(uint8 promiscuous);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

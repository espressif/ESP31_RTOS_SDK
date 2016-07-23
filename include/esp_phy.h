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

#ifndef __ESP_PHY__
#define __ESP_PHY__

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup Phy_APIs  Phy APIs
  * @brief ESP32 phy layer related APIs
  *
  */

/** @addtogroup Phy_APIs
  * @{
  */

enum phy_mode {
    PHY_MODE_11B = 1, /**< 802.11b mode */
    PHY_MODE_11G = 2, /**< 802.11g mode */
    PHY_MODE_11N = 3, /**< 802.11n mode */
};

enum phy_bw{
    PHY_BW_HT20   = 1, /**< phy bandwidth 20M only */
    PHY_BW_HT40   = 2, /**< phy bandwidth 40M only */
    PHY_BW_HT2040 = 3, /**< phy bandwidth 2040M coexist, currently not supported */
};

enum phy_2nd_chan {
    PHY_HT40U = 1, /**< the second channel is above the primary channel */
    PHY_HT40D = 2, /**< the second channel is below the primary channel */
};

#define CFG_MASK_PHY_MODE (1<<1)
#define CFG_MASK_PHY_BW   (1<<2)
#define CFG_MASK_PHY_SND_CHAN (1<<3)

struct phy_config {
    enum phy_mode phy_mode;  /**< phy mode */
    enum phy_bw phy_bw; /**< phy bandwidth */
    enum phy_2nd_chan phy_2nd_chan; /**< HT40 second channel, this parameter is for AP only, for sta the second channel is determined by AP */
    uint32 cfg_mask; /**< configuration mask */
};

/**
  * @brief     Set the phy configuration of ESP32 station/softap and save it to the Flash.
  *
  * @attention 1. This API can be called only when the opmode is correctly configured, e.g. if you try
  *               to configure softap phy while the opmode is station, it returns false.
  * @attention 2. If call this API when station/softap is started(connected), it will firstly stop/disconnect
  *               the station/softap, configure phy, then start/connect station/softap.
  * @attention 3. If you change the phy mode from 11N HT40 to 11b/g, the bandwith will be switched to 
  *               HT20 since the HT40 is for 11N only.
  * @attention 4. You can configure phy mode/bw/2nd_chan by mask the cfg_mask correctly, e.g. if (cfg_mask & 
  *               CFG_MASK_PHY_MODE) is true, the phy mode will be configued, otherwise, it has no impact on the 
  *               phy mode.
  * @attention 5. Since wifi_set_phy_mode/wifi_set_phy_bw/wifi_set_2nd_chan will stop station/softap and then
  *               start station/softap. So if you don't want station/softap stop/start several times, please
  *               use wifi_set_phy since it only stop/start once. Of course, if you only want to configure 
  *               phy mode, you can use the one you prefer.
  * @attention 6. The default sdk phy configuration: 11N/HT40
  *
  * @param     WIFI_INTERFACE : interface to be configured
  * @param     struct phy_config *config : phy configuration
  *
  * @return    true  : success
  * @return    false : failure
  */
bool wifi_set_phy(WIFI_INTERFACE ifx, struct phy_config *config);

/**
  * @brief     Get the phy configuration of ESP32 station/softap and save it to the Flash.
  *
  * @param     WIFI_INTERFACE : station/softap interface 
  * @param     struct phy_config *config : phy configuration
  *
  * @return    true  : success
  * @return    false : failure
  */
bool wifi_get_phy(WIFI_INTERFACE ifx, struct phy_config *config);

/**
  * @brief     Set the phy mode of ESP32 station/softap and save it to the flash.
  *
  * @attention Refer to wifi_set_phy()
  * 
  * @param     WIFI_INTERFACE : interface to be configured
  * @param     enum phy_mode phy_mode : phy mode
  * 
  * @return    true  : success
  * @return    false : failure
  */
bool wifi_set_phy_mode(WIFI_INTERFACE ifx, enum phy_mode phy_mode);

/**
  * @brief     Set the phy bandwidth of ESP32 station/softap and save it to the flash.
  *
  * @attention See wifi_set_phy()
  * 
  * @param     WIFI_INTERFACE : interface to be configured
  * @param     enum phy_bw phy_bw : phy bandwidth
  * 
  * @return    true  : success
  * @return    false : failure
  */
bool wifi_set_phy_bw(WIFI_INTERFACE ifx, enum phy_bw phy_bw);

/**
  * @brief     Set the phy second channel of ESP32 softap and save it to the flash.
  *
  * @attention Refer to wifi_set_phy()
  * 
  * @param     enum phy_2nd_chan : phy configuration
  * 
  * @return    true  : success
  * @return    false : failure
  */
bool wifi_set_phy_2nd_chan(enum phy_2nd_chan chan);

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

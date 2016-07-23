
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

#ifndef __ESP_REGDOMAIN_H__
#define __ESP_REGDOMAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup Regdomain_APIs  Regdomain APIs
  * @brief ESP32 Regdomain APIs
  *
  */

/** @addtogroup Regdomain_APIs
  * @{
  */
  
#define REGDOMAIN_CHANGROUP_MAX 1

struct regdomain_info{
    uint8_t    rd_sta_enable; /**< station regdomain enable/disable flag, 1 means enable, 0 means disable, default 0 */
    uint8_t    rd_ap_enable; /**< softap regdomain enable/disable flag, 1 means enable, 0 means disable, default 0 */

    uint16_t   regdomain; /**< regdomain */
    uint16_t   country; /**< ISO country code */
    uint8_t    location; /**< I (indoor), O(outdoor), other */
    char       isocc[2]; /**< country code string */
    uint8_t    ngroup; /**< For ESP32, ngroup should always be 1 */
    struct regdomain_chan { /**< regdomain_chan indicates on operation class */
        uint8_t schan; /**< Start channel number, the range is from 1 to 14 */
        uint8_t nchan; /**< The the count of channels in operation class */
        uint8_t maxtxpwr; /**< The max tranmit power, this field is ignored in ESP32 */
    }chan[REGDOMAIN_CHANGROUP_MAX];
};

/**
  * @brief  Get the regdomain info of station/softap
  *
  * @param  WIFI_INTERFACE ifx : station/softap interface
  * @param  struct regdomain_info *rd : For station, only field rd_sta_enable is valid,
  *         please ignore all other fields. For softap, if rd_ap_enable is 1, then fields from 
  *         regdomain to chan are valid, otherwise, all other fields are invalid.
  *
  * @return    true  : success
  * @return    false : failure
  *
  */
bool wifi_get_regdomain(WIFI_INTERFACE ifx, struct regdomain_info *rd);

/**
  * @brief  Set the regdomain info of station/softap
  *
  * @attention 1. If the regodmain of station is enable, the station will use passive scan
  *               to scan channel 1~14, NO PROBE REQUEST will send out, this is also the
  *               behavior specified in 80211-2012 standard. However, this may impact the 
  *               scan performance,  e.g. it can't find the hidden AP etc. If you think this
  *               not what you expected, pls contact espressif.
  * @attention 2. For softap, if regdomain is enable, the regodmain info will be added into
  *               beacon.
  * @attention 3. If current channel of softap is not in [schan, schan+nchan-1], then softap
  *               will switch to the channel schan automatically
  * @attention 4. In stationap mode, if the regdomain information of softap is conflicted with 
  *               the regodmain information of the external AP that sta is conneted, then the 
  *               external AP's regodmain information is prefered.
  *
  * @param  WIFI_INTERFACE ifx : station/softap interface
  * @param  struct regdomain_info *rd : For station, only field rd_sta_enable is valid, 
  *         please ignore all other field. For softap, if rd_ap_enable is 1, then fields from 
  *         regdomain to chan are valid, otherwise, all other fields are invalid.
  *
  * @return    true  : success
  * @return    false : failure
  *
  */
bool wifi_set_regdomain(WIFI_INTERFACE ifx, struct regdomain_info *rd);

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

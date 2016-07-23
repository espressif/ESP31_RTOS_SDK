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

#ifndef __ESP_SYSTEM_H__
#define __ESP_SYSTEM_H__

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup System_APIs System APIs
  * @brief System APIs
  */

/** @addtogroup System_APIs
  * @{
  */

/**
  * @brief  Get information of the SDK version.
  *
  * @param  null
  *
  * @return Information of the SDK version.
  */
const char *system_get_sdk_version(void);

/**
  * @brief  Reset to default settings.
  *
  *         Reset to default settings of the following APIs : wifi_station_set_auto_connect,
  *         wifi_set_phy_mode, wifi_softap_set_config related, wifi_station_set_config
  *         related, and wifi_set_opmode.
  *
  * @param  null
  *
  * @return null
  */
void system_restore(void);

/**
  * @brief  Restart system.
  *
  * @param  null
  *
  * @return null
  */
void system_restart(void);

/**
  * @brief     Set the chip to deep-sleep mode.
  *
  *            The device will automatically wake up after the deep-sleep time set
  *            by the users. Upon waking up, the device boots up from user_init.
  *
  * @attention  The parameter time_in_us to be "uint64" is for further development. 
  *                  Only the low 32 bits of parameter time_in_us are avalable now.
  *
  * @param     uint64 time_in_us : deep-sleep time, only the low 32bits are avalable now. unit: microsecond
  *
  * @return    null
  */
void system_deep_sleep(uint64 time_in_us);

/**
  * @brief  Get system time, unit: microsecond.
  *
  * @param  null
  *
  * @return System time, unit: microsecond.
  */
uint32 system_get_time(void);

/**
  * @brief  Print the system memory distribution, including data/rodata/bss/heap.
  *
  * @param  null
  *
  * @return null
  */
void system_print_meminfo(void);

/**
  * @brief  Get the size of available heap.
  *
  * @param  null
  *
  * @return Available heap size.
  */
uint32 system_get_free_heap_size(void);

/**
  * @brief  Get the chip ID.
  *
  * Example:          
  * <pre> 
  *         uint8 chip_id[6];
  *         system_get_chip_id(chip_id);
  * </pre>
  *
  * @param  uint8 *chip_id : the chip ID
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_get_chip_id(uint8 *chip_id);

/**
  * @brief     Get RTC time, unit: RTC clock cycle.
  *
  * @param     null
  *
  * @return    RTC time.
  */
uint64 system_get_rtc_time(void);

/**
  * @brief     Read user data from the RTC memory.
  *
  *            The user data segment (1024 bytes, as shown below) is used to store user data.
  *
  *             |<---- system data(512 bytes) ---->|<----------- user data(1024 bytes) --------->|
  *
  * @attention Read and write unit for data stored in the RTC memory is 4 bytes.
  * @attention src_addr is the block number (4 bytes per block). So when reading data
  *            at the beginning of the user data segment, src_addr will be 512/4 = 128,
  *            n will be data length.
  *
  * @param     uint16 src : source address of rtc memory, src_addr >= 128
  * @param     void *dst : data pointer
  * @param     uint16 n : data length, unit: byte
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_rtc_mem_read(uint16 src, void *dst, uint16 n);

/**
  * @brief     Write user data to  the RTC memory.
  *
  *            During deep-sleep, only RTC is working. So users can store their data
  *            in RTC memory if it is needed. The user data segment below (1024 bytes)
  *            is used to store the user data.
  *
  *            |<---- system data(512 bytes) ---->|<----------- user data(1024 bytes) --------->|
  *
  * @attention Read and write unit for data stored in the RTC memory is 4 bytes.
  * @attention src_addr is the block number (4 bytes per block). So when storing data
  *            at the beginning of the user data segment, src_addr will be 512/4 = 128,
  *            n will be data length.
  *
  * @param     uint16 src : source address of rtc memory, src_addr >= 128
  * @param     void *dst : data pointer
  * @param     uint16 n : data length, unit: byte
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_rtc_mem_write(uint16 dst, const void *src, uint16 n);

typedef enum {
    ADC1_PAD_GPIO36 = 0,
    ADC1_PAD_GPIO37,
    ADC1_PAD_GPIO38,
    ADC1_PAD_GPIO39,
    ADC1_PAD_GPIO32,
    ADC1_PAD_GPIO33,
    ADC1_PAD_GPIO34,
    ADC1_PAD_GPIO35
} adc1_read_pad;

typedef enum {
    ADC1_ATTEN_0DB = 0,
    ADC1_ATTEN_3DB,
    ADC1_ATTEN_6DB,
    ADC1_ATTEN_12DB
} adc1_read_atten;

/**
  * @brief    Read ADC1.
  *
  * @param     adc1_read_pad pad : the corresponding GPIO
  * @param     adc1_read_atten atten :  value of attenuation 
  *
  * @return    range of the return value is [0, 4096].
  *    - If atten == 0, the range of voltage can be measured is  [0, 1] V.
  *    - If atten == 1, the range of voltage can be measured is  [0, 1.4] V.
  *    - If atten == 2, the range of voltage can be measured is  [0, 2] V.
  *    - If atten == 3, the range of voltage can be measured is  [0, 4] V.
  */
uint16 system_adc1_read(adc1_read_pad pad, adc1_read_atten atten);

/**
  * @brief     Measure the power voltage of VDD3P3 pin 3 and 4, unit : 1/1024 V.
  *
  * @attention system_get_vdd33 depends on RF, please do not use it if RF is disabled.
  *
  * @param     null
  *
  * @return    Power voltage of VDD33, unit : 1/1024 V
  */
uint16 system_get_vdd33(void);

/**
  * @brief  Write data into flash with protection.
  *
  *         Flash read/write has to be 4-bytes aligned.
  *
  *         Protection of flash read/write :
  *             use 3 sectors (4KBytes per sector) to save  4KB data with protect,
  *             sector 0 and sector 1 are data sectors, back up each other,
  *             save data alternately, sector 2 is flag sector, point out which sector
  *             is keeping the latest data, sector 0 or sector 1.
  *
  * @param  uint16 start_sec : start sector (sector 0) of the 3 sectors which are
  *                            used for flash read/write protection.
  *    - For example, in IOT_Demo we can use the 3 sectors (3 * 4KB) starting from flash
  *      0x3D000 for flash read/write protection, so the parameter start_sec should be 0x3D
  * @param  void *param : pointer of the data to be written
  * @param  uint16 len : data length, should be less than a sector, which is 4 * 1024
  *
  * @return true  : succeed
  * @return false : fail
  */
bool system_param_save_with_protect(uint16 start_sec, void *param, uint16 len);

/**
  * @brief  Read the data saved into flash with the read/write protection.
  *
  *         Flash read/write has to be 4-bytes aligned.
  *
  *         Read/write protection of flash:
  *             use 3 sectors (4KB per sector) to save  4KB data with protect, sector
  *             0 and sector 1 are data sectors, back up each other, save data alternately,
  *             sector 2 is flag sector, point out which sector is keeping the latest data,
  *             sector 0 or sector 1.
  *
  * @param  uint16 start_sec : start sector (sector 0) of the 3 sectors used for
  *                            flash read/write protection. It cannot be sector 1 or sector 2.
  *    - For example, in IOT_Demo, the 3 sectors (3 * 4KB) starting from flash 0x3D000
  *      can be used for flash read/write protection.
  *      The parameter start_sec is 0x3D, and it cannot be 0x3E or 0x3F.
  * @param  uint16 offset : offset of data saved in sector
  * @param  void *param : data pointer
  * @param  uint16 len : data length, offset + len =< 4 * 1024
  *
  * @return true  : succeed
  * @return false : fail
  */
bool system_param_load(uint16 start_sec, uint16 offset, void *param, uint16 len);


/** \defgroup System_boot_APIs Boot APIs
  * @brief boot APIs
  */

/** @addtogroup System_boot_APIs
  * @{
  */

enum flash_size{
    FLASH_SIZE_1MB = 0,     /**<  Flash size :  1M Bytes */
    FLASH_SIZE_2MB,         /**<  Flash size :  2M Bytes */
    FLASH_SIZE_4MB,         /**<  Flash size :  4M Bytes */
    FLASH_SIZE_8MB,         /**<  Flash size :  8M Bytes */
    FLASH_SIZE_16MB,        /**<  Flash size : 16M Bytes */
    FLASH_SIZE_MAX
} ;

struct b_info{
	char bin_start_flash_id;  /**< start flash id of bin file*/
	char bin_end_flash_id;    /**< end flash id of bin file*/
	char bin_status;		  /**< bin's run status */
	char pad;				  /**< padding*/
	unsigned int jump_addr;   /**< jump_addr is irom0_flash.bin's start addr in flash*/
};

/**
  * @brief  Get the current Flash size.
  *
  * @param  null
  *
  * @return enum flash_size
  */
enum flash_size system_get_flash_size(void);

/**
  * @brief  Get CPU frequency.
  *
  * @param  null
  *
  * @return CPU frequency, unit : MHz.
  */
uint8 system_get_cpu_freq(void);

/**
  * @brief   Get bin info named by b_id
  *
  * @param   uint8 bin_id : b_id number, must < 5
  * @param   struct b_info *b_if : bin info of bin named by b_id 
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool system_get_bin_info(uint8 bin_id, struct b_info *b_if);

/**
  * @brief  Set bin info named by b_id
  *
  * @param   uint8 bin_id : b_id number, must < 5
  * @param   struct b_info *b_if : bin info of bin named by b_id 
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool system_set_bin_info(uint8 bin_id, struct b_info *b_if);

/**
  * @brief   Get current bin's bin_id
  *
  * @param   uint8 bin_id : b_id number, must < 5
  *
  * @return  uint8 type b_id
  */
uint8 system_get_current_bin_id(void);

/**
  * @brief   reboot and jump to bin named by bin_id
  *
  * @param   uint8 bin_id : b_id number, must < 5
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool system_reboot_to_userbin(uint8 bin_id);

/**
  * @}
  */
  
/** \defgroup Hardware_MAC_APIs Hardware MAC APIs
  * @brief Hardware MAC address APIs
  *
  * In WiFi MAC, only ESP32 station MAC is the hardware MAC, ESP32 softAP MAC is a software MAC 
  * calculated from ESP32 station MAC. 
  * So users need to call wifi_get_macaddr to query the ESP32 softAP MAC if ESP32 station MAC changed.
  *
  */

/** @addtogroup Hardware_MAC_APIs
  * @{
  */

typedef enum {
    DEFAULT_MAC = 0,    /**< Default hardware MAC provided by Espressif Systems */
    USER_MAC,           /**< User-define hardware MAC  */
} mac_group;

typedef enum {
    WIFI_MAC = 0,       /**< Hardware MAC address of ESP32 WiFi  */
    BT_MAC,             /**< Hardware MAC address of ESP32 bluetooth  */
} mac_type;

/**
  * @brief  Set user-define hardware MAC address.
  *
  * @attention  Hardware MAC address can only be set ONCE for each ESP32 chip.
  *
  * @param  mac_type type : type of hardware MAC address.
  * @param  uint8 *mac : user-define hardware MAC address, length: 6 bytes.
  *
  * @return  0 : succeed to set.
  * @return  1 : the hardware MAC has been set once, users can not set it any more.
  * @return  2 : fail to set.
  * @return  3 : invalid parameter.
  */
int system_efuse_program_user_mac(mac_type type, uint8 *mac);

/**
  * @brief  Read hardware MAC address.
  *
  * @param  mac_group group : default MAC or user-defined MAC.
  * @param  mac_type type : type of hardware MAC address.
  * @param  uint8 *mac : the hardware MAC address, length: 6 bytes.
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool system_efuse_read_mac(mac_group group, mac_type type, uint8 *mac);

/**
  * @brief  Set hardware MAC group, default MAC or user-defined MAC.
  *
  * @attention  This API needs system_restart to take effect.
  *
  * @param  mac_group group : default MAC or user-defined MAC.
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool system_efuse_set_mac_group(mac_group group);

/**
  * @brief  Get hardware MAC group, default MAC or user-defined MAC.
  *
  * @param  null
  *
  * @return  mac_group, the hardware MAC group.
  */
mac_group system_efuse_get_mac_group(void);

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

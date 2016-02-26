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

#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include "lwip/sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup system_upgrade_APIs OTA APIs
  * @brief OTA APIs
  */

/** @addtogroup system_upgrade_APIs
  * @{
  */

#define SPI_FLASH_SEC_SIZE      4096

/**
  * @brief    upgrade check call function type 
  *
  * @param    void * arg : call back parameter
  *
  * @return   void
  */
typedef void (*upgrade_states_check_callback)(void * arg);

enum ota_error_id {
	UPGRADE_OK,                       /**< OTA succeed */
	UPGRADE_FLAG_ERROR      = 10000,  /**< OTA is in progress, can not start it again */
	NO_STATION_IP_ERROR     = 10001,  /**< ESP32 station does not get IP address */
	UPGRADE_MEM_ERROR       = 10002,  /**< fail to alloc memory, maybe NULL pointer, or out of memory*/
	CREATE_SOCKET_ERROR     = 10003,  /**< fail to create socket*/
	SEND_QUEUE_ERROR        = 10004,  /**< fail to send message into queue */
	SERVER_CONNECT_ERROR    = 10005,  /**< fail to connect to the OTA server */
	SEND_URL_ERROR          = 10006,  /**< fail to send HTTP request */
	HTTP_HEAD_ERROR         = 10007,  /**< can not parse the HTTP response from OTA server */
	DOWNLOAD_TIMEOUT_ERROR  = 10008,  /**< OTA time out */
	GET_BIN_LENGTH_ERROR    = 10009,  /**< fail to get the length of the OTA bin file */
	ERASE_FLASH_ERROR       = 10010,  /**< fail to erase flash */
	RECV_DATA_ERROR         = 10011,  /**< fail to receive the OTA bin file*/
	BIN_MAGIC_ERROR         = 10012,  /**< the OTA bin file's magic check fail, invalid bin file */
	USER_ID_CONFLICT_ERROR  = 10013,  /**< the bin ID is using, can not set the same bin ID when calling system_upgrade_init */
	FLASH_ID_CONFLICT_ERROR = 10014,  /**< the new OTA bin will overlap with the current bin, so the downloading is rejected */
	CRC_CHECK_FAILED_ERROR  = 10015,  /**< the new OTA bin's CRC check fail  */
};

enum {
	NO_READY = 1,               /**< bin file is broken or not a correct bin file */
	TENTATIVE,                  /**< bin file has been downloaded in flash but never run */
	READY                       /**< bin is runing or has run before */ 
}; 

struct remote_bin_info {
	uint32 b_sumlen;           /**< bin's sum length */
	uint32 b_irom1len;		   /**< bin's irom1 part length */
	uint8 flash_id_num;        /**< the number of flash id occupied by bin file ,can be calculated by b_sumlen & b_irom1len */
};

struct server_info {
	struct sockaddr_in sockaddrin;   /**< remote server info,ip and port */
	char *http_req;					 /**< http request url */
};

struct upgrade_info {
	struct server_info s_if;        /**< remote server info */
    
    bool upgrade_flag;				/**< upgrade flag need be checked in upgrade check callback func ,false OTA failed ,true OTA success */
	
    uint32 check_times;             /**< OTA time(ms) set by user */
	upgrade_states_check_callback check_cb;  /**< check back function ,if user defined it ,it will be called whether OTA success or failed immediate*/
};

/**
  * @brief  start upgrade progress
  *
  * @attention  call this API should init server param firstly
  *
  * @param  struct upgrade_info *server : upgrade info contains remote server and callback func 
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool system_upgrade_start(struct upgrade_info *server);

/**
  * @brief  init upgrade progress
  *
  * @param  uint8 b_id : b_id number, must < 5
  * @param  uint8 start_flash_id : define a block in flash is 256KB, a block correspond a flash id 
  *                                start flash id means bin file's start flash id	
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool system_upgrade_init(uint8 b_id, uint8 start_flash_id);

/**
  * @brief  deinit upgrade progress
  *
  * @attention  this API should be called in upgrade check call back function or called after it 
  *
  * @param  void 
  *
  * @return  void
  */
void system_upgrade_deinit(void);

/**
  * @brief  get error id of upgrade progress 
  * @param  void 
  *
  * @return  enum ota_error_id : explanation in enum ota_error_id
  *
  */
enum ota_error_id system_upgrade_get_error_id(void);

/**
  * @brief  get download rate process
  *
  * @param  void 
  *
  * @return uint8 : return x means x%
  * 
  */
uint8 upgrade_get_process_rate(void);

/**
  * @brief  get bin's sum length and lenth of irom1 part in remote server
  *
  * @param  struct server_info *s_if :remote server info 
  * @param  struct remote_bin_info *rb_if : point to remote bin's info
  *       
  * @return  true  : succeed
  * @return  false : fail
  * 
  */
bool upgrade_get_remote_bin_info(struct server_info *s_if,struct remote_bin_info *rb_if);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

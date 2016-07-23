/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
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

#include "esp_common.h"
#include "upgrade.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

/*just for example, you can config them*/
#define SYMMETRY_STRUCTURE_MODE 1
#define SSID "netgear"
#define PASSWORD "espressif"
#define SERVER_IP "192.168.1.11"

#define pheadbuffer "Connection: keep-alive\r\n\
Cache-Control: no-cache\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36 \r\n\
Accept: */*\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"

/******************************************************************************
 * FunctionName : user_esp_upgrade_rsp
 * Description  : upgrade check call back function
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_esp_upgrade_rsp(void *arg)
{
    struct upgrade_info *server = arg;
    uint8 current_id = system_get_current_bin_id();
    os_printf("current id %d\n", current_id);

    /*check call back flag is true ,OTA success ,you can jump to run it*/
    if (server->upgrade_flag == true) {
        os_printf("OTA successed\n");

#if SYMMETRY_STRUCTURE_MODE
        if (current_id == 0) {
            system_reboot_to_userbin(1);
        } else {
            system_reboot_to_userbin(0);
        }
#else
        if (current_id == 0) {
            system_reboot_to_userbin(1);
        } else if (current_id == 1) {
            system_reboot_to_userbin(2);
        } else if (current_id == 2) {
            system_reboot_to_userbin(0);
        }

#endif
    }
    /*check call back flag is failed ,OTA failed ,os_printf error code*/
    else {
        os_printf("OTA failed\n");
        os_printf("error id %d\n", system_upgrade_get_error_id());
    }
}

/******************************************************************************
 * FunctionName : update_firmware
 * Description  : config remote server param and init bin info
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void update_firmware(void)
{
    os_printf("update  firmware\r\n");

    ip4_addr_t *server_ip;
    bool ret;
    char *bin_url = "ray/user.ota";
    struct upgrade_info *server = NULL;
    uint8 current_id;
    server = (struct upgrade_info *)malloc(sizeof(struct upgrade_info));
    memset(server, 0, sizeof(struct upgrade_info));
    server->check_cb = user_esp_upgrade_rsp;   //set upgrade check call back function

    server->check_times = 120000;

    server_ip->addr=ipaddr_addr("192, 168, 1, 11");
    //memcpy(server_ip, esp_server_ip, 4);

    if (server->s_if.http_req == NULL) {
        server->s_if.http_req = (uint8 *)malloc(512);
    }

#if SYMMETRY_STRUCTURE_MODE
    if (system_get_current_bin_id() == 0) {
        enum flash_size f_size = system_get_flash_size();

        if (f_size >= FLASH_SIZE_MAX) {
            os_printf("flash size don't support\n");
            return;
        }

        //more information about this api in document about OTA
        if (false == system_upgrade_init(1, pow(2, (f_size + 1)))) { //user id 1's start flash id is in half of all flash size
            os_printf("init user bin 1 falied ,error code %d\n", system_upgrade_get_error_id());
            return;
        }
    } else {
        if (false == system_upgrade_init(0, 0)) { // user id 0's start flash id is 0
            os_printf("init user bin 0 falied ,error code %d\n", system_upgrade_get_error_id());
            return;
        }
    }
#else

    /* if need ,you can call this api(upgrade_get_remote_bin_info) to get bin's info in remote server,
       contains bin's sumlength,bin's irom1 part length, how many flash id it will used */
    /****************************************************************
       	char *http_req;
    	struct remote_bin_info r_bin_info;
       	static struct server_info s_info;
    	memset(&s_info,0,sizeof(struct server_info));

    	s_info.sockaddrin.sin_family = AF_INET;
    	s_info.sockaddrin.sin_port = htons(server_port);
    	s_info.sockaddrin.sin_addr.s_addr = server_ip;
    	http_req = malloc(512);
    	sos_printf(http_req, "GET /%s HTTP/1.0\r\nHost: "IPSTR":%d\r\n"pheadbuffer"",bin_url, IP2STR(server_ip),80);
    	s_info.http_req = http_req;
    	ret = upgrade_get_remote_bin_info(&s_info,&r_bin_info);
    	if (ret != true) {
    		return ;
    	} else {
    		os_printf("flash id num %u ,bin sumlenth %u,irom1 length %u\n",r_bin_info.flash_id_num,r_bin_info.b_sumlen,r_bin_info.b_irom1len);
    	}
    	free(http_req);
    ****************************************************************/

    uint8 BIN_ID;
    uint8 START_FLASH_ID;
    current_id = system_get_current_bin_id();

    if (current_id == 0) {
        BIN_ID = 1;
        START_FLASH_ID = 2;
    } else if (current_id == 1) {
        BIN_ID = 2;
        START_FLASH_ID = 4;
    } else if(current_id == 2) {
        BIN_ID = 0;
        START_FLASH_ID = 0;
    }

    if (false == system_upgrade_init(BIN_ID, START_FLASH_ID)) {
        os_printf("init user bin %d falied ,error code %d\n", BIN_ID, system_upgrade_get_error_id());
        return;
    }
#endif

    sprintf(server->s_if.http_req, "GET /%s HTTP/1.0\r\nHost: "IPSTR":%d\r\n"pheadbuffer"", bin_url, IP2STR(server_ip), 80);
    os_printf("\nhttp_req : %s\n", server->s_if.http_req);

    server->s_if.sockaddrin.sin_family = AF_INET;
    server->s_if.sockaddrin.sin_port   = htons(80);
    server->s_if.sockaddrin.sin_addr.s_addr = inet_addr(SERVER_IP); // remote server ip

    if (system_upgrade_start(server) == false) {
        os_printf("upgrade error code %d\n", system_upgrade_get_error_id());
    }

    free(server->s_if.http_req);
    server->s_if.http_req = NULL;
    free(server);
    server = NULL;
}

/******************************************************************************
 * FunctionName : upgrade_begin
 * Description  : waiting station until get ip
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void upgrade_task(void *pvParameters)
{
    update_firmware();
    vTaskDelete(NULL);
}

void wifi_event_handler_cb(System_Event_t *event)
{
    if (event == NULL) {
        return;
    }

    switch (event->event_id) {
        case EVENT_STAMODE_GOT_IP:
            os_printf("sta got ip , creat upgrade task\n");
            xTaskCreate(upgrade_task, "upgrade_task", 1024, NULL, 1, NULL);
            break;

        default:
            break;
    }
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    os_printf("SDK version:%s\n", system_get_sdk_version());
    wifi_set_opmode(STATION_MODE);

    // set AP parameter
    struct station_config config;
    bzero(&config, sizeof(struct station_config));
    sprintf(config.ssid, SSID);
    sprintf(config.password, PASSWORD);
    wifi_station_set_config(&config);

    wifi_set_event_handler_cb(wifi_event_handler_cb);
}

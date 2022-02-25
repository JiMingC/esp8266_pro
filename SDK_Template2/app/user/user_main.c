/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#include "ets_sys.h"
#include "osapi.h"
#include "ip_addr.h"
#include "espconn.h"
#include "mem.h"
#include "driver/uart.h"

#include "user_interface.h"
#include "smartconfig.h"
#include "airkiss.h"

#include "user_display.h"
#include "user_sntp.h"
#include "user_TcpServer.h"
#include "user_lcd.h"
#include "spi_test.h"


#define DEVICE_TYPE 		"gh_9e2cff3dfa51" //wechat public number
#define DEVICE_ID 			"122475" //model ID

#define DEFAULT_LAN_PORT 	12476


#define U_SC_TYPE_AIRKISS				1
#define	U_SC_TYPE_ESPTOUCH				2
#define U_SC_TYPE_ESPTOUCH_AIRKISS		4
#define U_SC_TYPE						U_SC_TYPE_ESPTOUCH


#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0xfb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0xfc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0xfd000
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#else
#error "The flash map is not supported"
#endif

static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_BOOTLOADER, 						0x0, 												0x1000},
    { SYSTEM_PARTITION_OTA_1,   						0x1000, 											SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_OTA_2,   						SYSTEM_PARTITION_OTA_2_ADDR, 						SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_RF_CAL,  						SYSTEM_PARTITION_RF_CAL_ADDR, 						0x1000},
    { SYSTEM_PARTITION_PHY_DATA, 						SYSTEM_PARTITION_PHY_DATA_ADDR, 					0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER, 				SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR, 			0x3000},
};


LOCAL struct station_config s_staconf;
LOCAL os_timer_t user_timer;			//for wifi connect and sntp
LOCAL os_timer_t user_loop_timer;		//for main loop
LOCAL os_timer_t send_data_timer;     	//for tcp/udp data send


LOCAL struct ip_info user_ip;

LOCAL SntpData_t sntpdata;
LOCAL u8 sntpTupdate = 0xFF;

LOCAL u8 mcu_status = 0;

#if   TCP_CLIENT
struct espconn tcp_client;
#elif TCP_SERVER
struct espconn *tcp_server;
#elif UDP_TEST
struct espconn udp_test;
#endif
#if U_SC_TYPE == U_SC_TYPE_AIRKISS
LOCAL esp_udp ssdp_udp;
LOCAL struct espconn pssdpudpconn;
LOCAL os_timer_t ssdp_time_serv;

uint8_t  lan_buf[200];
uint16_t lan_buf_len;
uint8 	 udp_sent_cnt = 0;
const airkiss_config_t akconf =
{
	(airkiss_memset_fn)&memset,
	(airkiss_memcpy_fn)&memcpy,
	(airkiss_memcmp_fn)&memcmp,
	0,
};

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_time_callback(void)
{
	uint16 i;
	airkiss_lan_ret_t ret;
	
	if ((udp_sent_cnt++) >30) {
		udp_sent_cnt = 0;
		os_timer_disarm(&ssdp_time_serv);//s
		//return;
	}

	ssdp_udp.remote_port = DEFAULT_LAN_PORT;
	ssdp_udp.remote_ip[0] = 255;
	ssdp_udp.remote_ip[1] = 255;
	ssdp_udp.remote_ip[2] = 255;
	ssdp_udp.remote_ip[3] = 255;
	lan_buf_len = sizeof(lan_buf);
	ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
		DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
	if (ret != AIRKISS_LAN_PAKE_READY) {
		os_printf("Pack lan packet error!");
		return;
	}
	
	ret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
	if (ret != 0) {
		os_printf("UDP send error!");
	}
	os_printf("Finish send notify!\n");
}

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_recv_callbk(void *arg, char *pdata, unsigned short len)
{
	uint16 i;
	remot_info* pcon_info = NULL;
		
	airkiss_lan_ret_t ret = airkiss_lan_recv(pdata, len, &akconf);
	airkiss_lan_ret_t packret;
	
	switch (ret){
	case AIRKISS_LAN_SSDP_REQ:
		espconn_get_connection_info(&pssdpudpconn, &pcon_info, 0);
		os_printf("remote ip: %d.%d.%d.%d \r\n",pcon_info->remote_ip[0],pcon_info->remote_ip[1],
			                                    pcon_info->remote_ip[2],pcon_info->remote_ip[3]);
		os_printf("remote port: %d \r\n",pcon_info->remote_port);
      
        pssdpudpconn.proto.udp->remote_port = pcon_info->remote_port;
		os_memcpy(pssdpudpconn.proto.udp->remote_ip,pcon_info->remote_ip,4);
		ssdp_udp.remote_port = DEFAULT_LAN_PORT;
		
		lan_buf_len = sizeof(lan_buf);
		packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
			DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
		
		if (packret != AIRKISS_LAN_PAKE_READY) {
			os_printf("Pack lan packet error!");
			return;
		}

		os_printf("\r\n\r\n");
		for (i=0; i<lan_buf_len; i++)
			os_printf("%c",lan_buf[i]);
		os_printf("\r\n\r\n");
		
		packret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
		if (packret != 0) {
			os_printf("LAN UDP Send err!");
		}
		
		break;
	default:
		os_printf("Pack is not ssdq req!%d\r\n",ret);
		break;
	}
}

void ICACHE_FLASH_ATTR
airkiss_start_discover(void)
{
	ssdp_udp.local_port = DEFAULT_LAN_PORT;
	pssdpudpconn.type = ESPCONN_UDP;
	pssdpudpconn.proto.udp = &(ssdp_udp);
	espconn_regist_recvcb(&pssdpudpconn, airkiss_wifilan_recv_callbk);
	espconn_create(&pssdpudpconn);

	os_timer_disarm(&ssdp_time_serv);
	os_timer_setfn(&ssdp_time_serv, (os_timer_func_t *)airkiss_wifilan_time_callback, NULL);
	os_timer_arm(&ssdp_time_serv, 1000, 1);//1s
}
#endif

LOCAL uint8  messages_send_buffer[50];
LOCAL uint16 messages_send_count = 0;
void ICACHE_FLASH_ATTR
TCP_Send_data() {
#if   TCP_CLIENT
	os_sprintf(messages_send_buffer,"hi this is ESP8266 TCP client![%d]\r\n",messages_send_count);
	tcp_client_send_data(&tcp_client,messages_send_buffer,strlen(messages_send_buffer));
#elif TCP_SERVER
	os_sprintf(messages_send_buffer,"hi this is ESP8266 TCP server![%d]\r\n",messages_send_count);
	tcp_server_send_data(tcp_server,messages_send_buffer,strlen(messages_send_buffer));
#endif
	messages_send_count++;
}


void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
    switch(status) {
        case SC_STATUS_WAIT:					//
        	mcu_status = mcu_SmartConfiging;
            os_printf("SC_STATUS_WAIT\n");
            break;
        case SC_STATUS_FIND_CHANNEL:			//
        	mcu_status = mcu_SmartConfig_founded;
            os_printf("SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
        	mcu_status = mcu_Get_SSID_PSWD;
            os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
			sc_type *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH) {
                os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
            } else {
                os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;
        case SC_STATUS_LINK:
        	mcu_status = mcu_WIFI_Connecting;
            os_printf("SC_STATUS_LINK\n"); //
            struct station_config *sta_conf = pdata;

	        wifi_station_set_config(sta_conf);
	        wifi_station_disconnect();
	        wifi_station_connect();
            break;
        case SC_STATUS_LINK_OVER:				//
        	mcu_status = mcu_SmartConfig_over;
            os_printf("SC_STATUS_LINK_OVER\n");
            if (pdata != NULL) {
				//SC_TYPE_ESPTOUCH
                uint8 phone_ip[4] = {0};
                os_memcpy(phone_ip, (uint8*)pdata, 4);
                os_printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
                system_restart();
            } else {
            	//SC_TYPE_AIRKISS - support airkiss v2.0
#if U_SC_TYPE == U_SC_TYPE_AIRKISS
				airkiss_start_discover();
#endif
            }
            smartconfig_stop();
            break;
    }

}

void ICACHE_FLASH_ATTR
sntp_read_timer_callback2(void *arg)
{
	uint32_t time = sntp_get_current_timestamp();
	uint8 time1[25];
	os_sprintf(time1, "%s", sntp_get_real_time(time));

	SntpData_t T_sntpdata;
	os_printf("date:%s\r\n", sntpTimeChangeToSimpleDateFormat(time1, &T_sntpdata, time));
	sntpTupdate = SntpdataUpadte(&sntpdata, T_sntpdata);
//	sntpdata.hour = T_sntpdata.hour;
//	sntpdata.minute = T_sntpdata.minute;
//	sntpdata.second = T_sntpdata.second;
//	DisplayTime(sntpTupdate, sntpdata);
//
//	os_printf("time:%d\r\n",time);
//	DisplayTime(sntpTupdate, &sntpdata);
//	os_printf("date:%s\r\n",sntp_get_real_time(time));
//	os_printf("date:%s\r\n", sntpTimeChangeToSimpleDateFormat(time1), &sntpdata));
}

void ICACHE_FLASH_ATTR wifi_check(void *arg){
	static uint8 count=0;
	uint8 status;
	os_timer_disarm(&user_timer); //
	count++; //
	mcu_status = mcu_WIFI_Connecting;
	status = wifi_station_get_connect_status(); //
	if(status == STATION_GOT_IP){ //
		mcu_status = mcu_WIFI_Success;
		os_printf("wi_fi connect.\r\n");
		os_timer_disarm(&user_timer);
		os_timer_setfn(&user_timer, sntp_read_timer_callback2, NULL);
		os_timer_arm(&user_timer, 1000, 1);
		wifi_get_ip_info(STATION_IF,&user_ip);		//while connect success, get ip info
#if   TCP_CLIENT
		tcp_client_init(&tcp_client,TCP_SERVER_IP, &user_ip.ip,TCP_SERVER_PORT);
		os_timer_disarm(&send_data_timer);
		os_timer_setfn(&send_data_timer, (os_timer_func_t *) TCP_Send_data,NULL);
		os_timer_arm(&send_data_timer, 2000, true);
#elif TCP_SERVER
		tcp_server_init(tcp_server,TCP_LOCAL_PORT);
		os_timer_disarm(&send_data_timer);
		os_timer_setfn(&send_data_timer, (os_timer_func_t *) TCP_Send_data,NULL);
		os_timer_arm(&send_data_timer, 2000, true);
#elif UDP_TEST
		udp_init(&udp_test,UDP_REMOTE_IP,&local_ip.ip,UDP_REMOTE_PORT);
		os_timer_disarm(&send_data_timer);
		os_timer_setfn(&send_data_timer, (os_timer_func_t *)UDP_Send_Messages,NULL);
		os_timer_arm(&send_data_timer, 2000, true);
#endif
		return;
	}else{
		mcu_status = mcu_WIFI_Reconnect;
		os_printf("wi_fi connect[%d].\r\n", count);
		if(count > 7){ //
			mcu_status = mcu_WIFI_Fail;
			os_printf("wi_fi connect fail.\r\n");
			smartconfig_start(smartconfig_done);
			return;
		}
	}
	os_timer_arm(&user_timer,2000,0);
}

LOCAL void ICACHE_FLASH_ATTR
user_scan_done(void *arg, STATUS status) {
	if (status == OK) {
		mcu_status = mcu_WIFI_Scan;
        os_printf("SCAN_STATUS_LINK\n"); //

        wifi_station_set_config(&s_staconf);
        wifi_station_disconnect();
        if (wifi_station_connect()) {
			os_printf("SCAN_STATUS_connect SUCCESS\n");
			os_timer_disarm(&user_timer);
			os_timer_setfn(&user_timer, wifi_check, NULL);
			os_timer_arm(&user_timer, 2000, 0);
        } else {
			os_printf("SCAN_STATUS_CONNECT FAIL\n");
			smartconfig_start(smartconfig_done);
        }
	} else {
		os_printf("connect time out, start smart_cfg\n");
		smartconfig_start(smartconfig_done);
	}
}

void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
		os_printf("system_partition_table_regist fail\r\n");
		while(1);
	}
}
LOCAL u16 color_rand;
void ICACHE_FLASH_ATTR
Main_loop() {
	//LCD_Clear(color_rand);
	//color_rand += 0xF;
	//LCD_ShowString(LCD_W/2 - 10, LCD_H/2 - 10 ,"hello kogwejun");
	//LCD_DrawRectangle(0, 0, 60, 120);
	//LCD_ShowChar(10, 10, ' ',0);
	//LCD_ShowString(10,30,"2.2 inch TFT 240*320");
	DisplayMcuMessage(mcu_status);
	DisplayTime(sntpTupdate, sntpdata);
	os_printf("test %x\n", color_rand);
}

//wifi Para data exist, sacn the ap
void ICACHE_FLASH_ATTR
user_scan(void)
{
   struct scan_config config;
   os_memset(&config, 0, sizeof(config));
   config.ssid = s_staconf.ssid;
   if (wifi_station_scan(&config, user_scan_done) == 0)
	   smartconfig_start(smartconfig_done);
}

void ICACHE_FLASH_ATTR
user_init(void)
{

	uart_init(115200, 115200);
	char buf[128] = {0};
	os_sprintf(buf,"compile time:"__DATE__" "__TIME__);
	os_printf("SDK version:%s\n", system_get_sdk_version());
	//
	smartconfig_set_type(U_SC_TYPE); //SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS

    wifi_set_opmode(STATION_MODE);

    //Get wi_fi last config data from memory
    wifi_station_get_config_default(&s_staconf);
    s_staconf.bssid_set = 0;
    if (os_strlen(s_staconf.ssid) != 0) {
    	mcu_status = mcu_WIFI_Scan;
        os_printf("user_scan\n");
        system_init_done_cb(user_scan);
    } else {
        os_printf("smart_cfg\n");
        mcu_status = mcu_SmartConfiging;
        smartconfig_start(smartconfig_done);
    }

    SNTP_Init();
    Lcd_Init();
    LCD_Clear(BLACK);
    //LCD_ShowString(10,30,"2.2 inch TFT 240*320");
    //showimage();
    //spi_interface_test();
    //LCD_fillScreen(BLUE);
    //LCD_Clear(WHITE);
    //spi_master_init(1);

	os_timer_disarm(&user_loop_timer);
	os_timer_setfn(&user_loop_timer, (os_timer_func_t *) Main_loop,NULL);
	os_timer_arm(&user_loop_timer, 1000, true);
}

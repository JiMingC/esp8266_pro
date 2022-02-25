/*
 * user_display.h
 *
 *  Created on: 2022Äê2ÔÂ23ÈÕ
 *      Author: jiming
 */

#ifndef APP_INCLUDE_USER_DISPLAY_H_
#define APP_INCLUDE_USER_DISPLAY_H_

#include "user_sntp.h"
#include "ets_sys.h"
#include "osapi.h"

enum mcu_status{
	mcu_Normal = 0,
	mcu_WIFI_Scan,
	mcu_WIFI_Connecting,
	mcu_WIFI_Reconnect,
	mcu_WIFI_Success,
	mcu_WIFI_Fail,
	mcu_SmartConfiging,
	mcu_SmartConfig_founded,
	mcu_SmartConfig_over,
	mcu_Get_SSID_PSWD,
}mcu_status_e;
void DisplayTime(u8 sntpTupdate,SntpData_t sntpdata);
void DisplayMcuMessage(u8 mcu_status);
#endif /* APP_INCLUDE_USER_DISPLAY_H_ */

/*
 * user_display.h
 *
 *  Created on: 2022Äê2ÔÂ23ÈÕ
 *      Author: jiming
 */

#ifndef APP_INCLUDE_USER_DISPLAY_H_
#define APP_INCLUDE_USER_DISPLAY_H_

#include "user_sntp.h"
#include "user_Weatherserver.h"
#include "ets_sys.h"
#include "osapi.h"

#define LCD_H_HALF	LCD_H/2
#define LCD_W_HALF	LCD_W/2

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
	mcu_BootanimationEnd,
}mcu_status_e;
void DisplayTime(u8 sntpTupdate,SntpData_t sntpdata);
void DisplayMcuMessage(u8 mcu_status);
void DisplayWeatherInfo(WeatherData_t * wdata);
void DisplayNetMsg(char *NetMsgBuff);
u8 DisplayNetTFT(u8 *NetMsgBuff);
u8 DisplayNetBuf(u8 *NetMsgBuff);
u8 bootanimation(u8 x, u8 y, u8 len, u16 color);
#endif /* APP_INCLUDE_USER_DISPLAY_H_ */

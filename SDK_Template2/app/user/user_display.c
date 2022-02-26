/*
 * user_display.c
 *
 *  Created on: 2022Äê2ÔÂ23ÈÕ
 *      Author: jiming
 */
#include "user_display.h"
#include "user_lcd.h"

#define LCD_H_HALF	LCD_H/2
#define LCD_W_HALF	LCD_W/2

#define TimeDisplayT	2

void ICACHE_FLASH_ATTR
DisplayTime(u8 sntpTupdate,SntpData_t sntpdata) {
#if TimeDisplayT == 0
	if (sntpTupdate > 1) {
		MLCD_ShowChar(LCD_W_HALF-32+16, LCD_H_HALF, ':', 2);
		MLCD_ShowChar(LCD_W_HALF-32+40, LCD_H_HALF, ':', 2);
	}
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(LCD_W_HALF-32+48, LCD_H_HALF, sntpdata.second, 2);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(LCD_W_HALF-32+24, LCD_H_HALF, sntpdata.minute, 2);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(LCD_W_HALF-32, LCD_H_HALF, sntpdata.hour, 2);
	os_printf("%d, %d, %d\n",sntpdata.hour, sntpdata.minute, sntpdata.second );
#elif TimeDisplayT == 1
	if (sntpTupdate > 1) {
		MLCD_ShowChar(0+34, LCD_H_HALF, ':', 4);
		MLCD_ShowChar(0+76, LCD_H_HALF, ':', 4);
	}
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(0+90, LCD_H_HALF, sntpdata.second, 4);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(0+48, LCD_H_HALF, sntpdata.minute, 4);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(0+4, LCD_H_HALF, sntpdata.hour, 4);
	os_printf("%d, %d, %d\n",sntpdata.hour, sntpdata.minute, sntpdata.second );
#elif TimeDisplayT == 2
	// (1:0804 2:1206 3:1608 4:2412 5:3216)
	if (sntpTupdate > 1) {
		MLCD_ShowChar(40, LCD_H_HALF, ':', 5);
		MLCD_ShowChar(86, LCD_H_HALF + 16, ':', 3);
		LCD_ShowChar2(13, 0, '-', 0, 1);
	}
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(94, LCD_H_HALF + 16, sntpdata.second, 3);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(54, LCD_H_HALF, sntpdata.minute, 5);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(10, LCD_H_HALF, sntpdata.hour, 5);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		LCD_Show2Num(5, 0, sntpdata.month, 2);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		LCD_Show2Num(17, 0, sntpdata.day, 2);
#ifdef DEBUG
	os_printf("%d, %d, %d\n",sntpdata.hour, sntpdata.minute, sntpdata.second );
#endif
#endif
}

void ICACHE_FLASH_ATTR
DisplayMcuMessage(u8 mcu_status) {
	char str_buf[20] = "\0";
	static u8 mcu_curstatus;
	if ((mcu_curstatus == mcu_status)) {
		return;
	}
	u8 fonts_base = 1;
	mcu_curstatus = mcu_status;
	u8 x = GetXfromFontssize(fonts_base);
	u8 y = GetYfromFontssize(fonts_base);
	switch(mcu_status) {
	case mcu_Normal:
		break;
	case mcu_WIFI_Scan:
		os_sprintf(str_buf,"Find SSid...");
		break;
	case mcu_WIFI_Connecting:
		os_sprintf(str_buf,"Connecting...");
		break;
	case mcu_WIFI_Reconnect:
		os_sprintf(str_buf,"RECONNECT...");
		break;
	case mcu_WIFI_Success:
		os_sprintf(str_buf,"CONNECTED");
		break;
	case mcu_WIFI_Fail:
		os_sprintf(str_buf,"CONNECT NG...");
		break;
	case mcu_SmartConfiging:
		os_sprintf(str_buf,"SMARTCONFIG...");
		break;
	case mcu_SmartConfig_founded:
		os_sprintf(str_buf,"Wait found...");
		break;
	case mcu_SmartConfig_over:
		os_sprintf(str_buf,"Smartconfig OK");
		break;
	case mcu_Get_SSID_PSWD:
		os_sprintf(str_buf,"Get_SSID_PSWD");
		break;
	default:
		break;
	}
	if (str_buf[0] != '\0') {
		LCD_Fill(0, 140, LCD_W, 158, BLACK);
		LCD_ShowString(LCD_W/2-os_strlen(str_buf)*x/2,LCD_H - y - 1,str_buf,fonts_base);
	}

}

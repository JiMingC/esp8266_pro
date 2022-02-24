/*
 * user_display.c
 *
 *  Created on: 2022Äê2ÔÂ23ÈÕ
 *      Author: jiming
 */
#include "user_display.h"
#include "user_lcd.h"

void ICACHE_FLASH_ATTR
DisplayTime(u8 sntpTupdate,SntpData_t sntpdata) {

	MLCD_Show2Num(LCD_W/3, LCD_H/2, sntpdata.hour, 2);
	MLCD_Show2Num(LCD_W/3+16, LCD_H/2, sntpdata.minute, 2);
	MLCD_Show2Num(LCD_W/3+32, LCD_H/2, sntpdata.second, 2);
	os_printf("%d, %d, %d\n",sntpdata.hour, sntpdata.minute, sntpdata.second );
}

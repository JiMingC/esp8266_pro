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
	if (sntpTupdate > 1) {
		MLCD_ShowChar(40, LCD_H_HALF, ':', 4);
		MLCD_ShowChar(86, LCD_H_HALF + 16, ':', 2);
	}
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(94, LCD_H_HALF + 16, sntpdata.second, 2);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(54, LCD_H_HALF, sntpdata.minute, 4);
	sntpTupdate >>= 1;
	if (sntpTupdate & 0x1)
		MLCD_Show2Num(10, LCD_H_HALF, sntpdata.hour, 4);
	os_printf("%d, %d, %d\n",sntpdata.hour, sntpdata.minute, sntpdata.second );
#endif
}

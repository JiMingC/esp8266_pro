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

void DisplayTime(u8 sntpTupdate,SntpData_t sntpdata);
#endif /* APP_INCLUDE_USER_DISPLAY_H_ */

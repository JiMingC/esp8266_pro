/*
 * user_sntp.h
 *
 *  Created on: 2022Äê2ÔÂ18ÈÕ
 *      Author: jiming
 */

#ifndef APP_INCLUDE_USER_SNTPH_
#define APP_INCLUDE_USER_SNTPH_

#include "ets_sys.h"
#include "sntp.h"
#include "osapi.h"

typedef struct sntpData_t
{
	uint8 week;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 minute;
	uint8 second;
	u16 year;
} SntpData_t;

void SNTP_Init(void);
void GetSntpTime(char *pRealTime);
void sntp_read_timer_callback(void *arg);
char* sntpTimeChangeToSimpleDateFormat(char *pSntpRealTime, SntpData_t *sntpData,  u32 timeS);
u8 SntpdataUpadte(SntpData_t *O_sntpData, SntpData_t N_sntpData) ;
#endif /* APP_INCLUDE_USER_SNTPH_ */

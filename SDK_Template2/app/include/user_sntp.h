/*
 * user_sntp.h
 *
 *  Created on: 2022��2��18��
 *      Author: jiming
 */

#ifndef APP_INCLUDE_USER_SNTPH_
#define APP_INCLUDE_USER_SNTPH_


void SNTP_Init(void);
void GetSntpTime(char *pRealTime);
void sntp_read_timer_callback(void *arg);
#endif /* APP_INCLUDE_USER_SNTPH_ */

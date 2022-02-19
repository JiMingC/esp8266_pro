/*
 * user_sntp.c
 *
 *  Created on: 2022��2��18��
 *      Author: jiming
 */

#include "user_sntp.h"
#include "ets_sys.h"
#include "sntp.h"
#include "osapi.h"
/**
 @brief ��ʼ��SNTP����
 @param ��
 @return ��
*/
void ICACHE_FLASH_ATTR
SNTP_Init(void)
{
	sntp_setservername(0,"0.cn.pool.ntp.org");
	sntp_setservername(1,"1.cn.pool.ntp.org");
	sntp_setservername(2,"2.cn.pool.ntp.org");
	sntp_init();
}


typedef struct sntpData_t
{
	uint8 week;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 minute;
	uint8 second;
	uint8 year;
} SntpData_t;



/**
 @brief ������ڼ�
 @param pWeek -[in] ������ַ���
 @return ���ڼ�
*/
static uint8 ICACHE_FLASH_ATTR
checkWahtWeek(char *pWeek)
{
	if(!pWeek)
	{
		return 0;
	}

	uint8 week;
	if(strcmp(pWeek, "Mon") == 0)
	{
		week = 1;									// ����һ
	}
	else if(strcmp(pWeek, "Tue") == 0)
	{
		week = 2;									// ���ڶ�
	}
	else if(strcmp(pWeek, "Wed") == 0)
	{
		week = 3;									// ������
	}
	else if(strcmp(pWeek, "Thu") == 0)
	{
		week = 4;									// ������
	}
	else if(strcmp(pWeek, "Fri") == 0)
	{
		week = 5;									// ������
	}
	else if(strcmp(pWeek, "Sat") == 0)
	{
		week = 6;									// ������
	}
	else if(strcmp(pWeek, "Sun") == 0)
	{
		week = 7;									// ������
	}
	return week;
}

/**
 @brief ��鼸�·�
 @param pMonth -[in] ������ַ���
 @return ���·�
*/
static uint8 ICACHE_FLASH_ATTR
checkWahtMonth(char *pMonth)
{
	if(!pMonth)
	{
		return 0;
	}

	uint8 month;
	if(strcmp(pMonth, "Jan") == 0)
	{
		month = 1;									// һ��
	}
	else if(strcmp(pMonth, "Feb") == 0)
	{
		month = 2;									// ����
	}
	else if(strcmp(pMonth, "Mar") == 0)
	{
		month = 3;									// ����
	}
	else if(strcmp(pMonth, "Apr") == 0)
	{
		month = 4;									// ����
	}
	else if(strcmp(pMonth, "May") == 0)
	{
		month = 5;									// ����
	}
	else if(strcmp(pMonth, "Jun") == 0)
	{
		month = 6;									// ����
	}
	else if(strcmp(pMonth, "Jul") == 0)
	{
		month = 7;									// ����
	}
	else if(strcmp(pMonth, "Aug") == 0)
	{
		month = 8;									// ����
	}
	else if(strcmp(pMonth, "Sep") == 0)
	{
		month = 9;									// ����
	}
	else if(strcmp(pMonth, "Oct") == 0)
	{
		month = 10;									// ʮ��
	}
	else if(strcmp(pMonth, "Nov") == 0)
	{
		month = 11;									// ʮһ��
	}
	else if(strcmp(pMonth, "Dec") == 0)
	{
		month = 12;									// ʮ����
	}
	return month;
}

/**
 @brief ��鼸��
 @param pDay -[in] ������ַ���
 @return ����
*/
static uint8 ICACHE_FLASH_ATTR
checkWahtDay(char *pDay)
{
	if(!pDay)
	{
		return 0;
	}

	uint8 day = (*pDay & 0x0f) << 4;
	day = day | (*(pDay+1) & 0x0f);

	return day;
}

/**
 @brief ��鼸ʱ
 @param pHour -[in] ������ַ���
 @return ��ʱ
*/
static uint8 ICACHE_FLASH_ATTR
checkWahtHour(char *pHour)
{
	if(!pHour)
	{
		return 0;
	}

	uint8 hour = (*pHour & 0x0f) << 4;
	hour = hour | (*(pHour+1) & 0x0f);

	return hour;
}

/**
 @brief ��鼸��
 @param pMinute -[in] ������ַ���
 @return ����
*/
static uint8 ICACHE_FLASH_ATTR
checkWahtMinute(char *pMinute)
{
	if(!pMinute)
	{
		return 0;
	}

	uint8 minute = (*pMinute & 0x0f) << 4;
	minute = minute | (*(pMinute+1) & 0x0f);

	return minute;
}

/**
 @brief ��鼸��
 @param pSecond -[in] ������ַ���
 @return ����
*/
static uint8 ICACHE_FLASH_ATTR
checkWahtSecond(char *pSecond)
{
	if(!pSecond)
	{
		return 0;
	}

	uint8 second = (*pSecond & 0x0f) << 4;
	second = second | (*(pSecond+1) & 0x0f);

	return second;
}

/**
 @brief ��鼸��
 @param pYear -[in] ������ַ���
 @return ����
*/
static uint8 ICACHE_FLASH_ATTR
checkWahtYear(char *pYear)
{
	if(!pYear)
	{
		return 0;
	}

	uint8 year = (*(pYear+2) & 0x0f) << 4;
	year = year | (*(pYear+3) & 0x0f);

	return year;
}

static void ICACHE_FLASH_ATTR
sntpTimestampToSntpdata(uint32 cur_timetamp, SntpData_t *sntpdata) {

}
/**
 @brief SNTPʱ��ת��Ϊ�����ڸ�ʽ
 @param pSntpRealTime -[in] ʵʱʱ��
 @return �����ڸ�ʽʱ���ַ���
*/
static char *ICACHE_FLASH_ATTR
sntpTimeChangeToSimpleDateFormat(char *pSntpRealTime)
{
	if(!pSntpRealTime)
	{
		return "";
	}

	pSntpRealTime[24] = '\0';						// ��Ҫ��ݺ��������
	SntpData_t sntpData;
	uint8 dateType[7] = {3, 3, 2, 2, 2, 2, 4};		// 3-Fri 3-May 2-31 2-11: 2-21: 2-42 4-2019
	uint8 temp[5];
	uint8 i = 0, j = 0;

	while(*pSntpRealTime != '\0')
	{
		if(*pSntpRealTime == ' ' || *pSntpRealTime == ':')
		{
			pSntpRealTime++;
			i++;
		}
		else
		{
			os_memset(temp, '\0', 5);
			for(j = 0; j < dateType[i]; j++)
			{
				temp[j] = *pSntpRealTime;
				pSntpRealTime++;
			}
			switch(i)
			{
			case 0: // ��
				sntpData.week = checkWahtWeek(temp);
				break;
			case 1: // ��
				sntpData.month = checkWahtMonth(temp);
				break;
			case 2: // ��
				sntpData.day = checkWahtDay(temp);
				break;
			case 3: // ʱ
				sntpData.hour = checkWahtHour(temp);
				break;
			case 4: // ��
				sntpData.minute = checkWahtMinute(temp);
				break;
			case 5: // ��
				sntpData.second = checkWahtSecond(temp);
				break;
			case 6: // ��
				sntpData.year = checkWahtYear(temp);
				break;
			default:
				break;
			}
		}
	}

	os_memset(pSntpRealTime, 0, 24);
	os_sprintf(pSntpRealTime, "20%x-%02x-%02x %02x:%02x:%02x",
								sntpData.year, sntpData.month,
								sntpData.day, sntpData.hour,
								sntpData.minute, sntpData.second);
	return pSntpRealTime;
}

/**
 @brief ��ȡSNTPʱ��
 @param pRealTime -[in&out] ʵʱʱ��
 @return ��
*/
void ICACHE_FLASH_ATTR
GetSntpTime(char *pRealTime)
{
	uint32_t time = sntp_get_current_timestamp();
	os_sprintf(pRealTime, "%s", sntp_get_real_time(time));
	//os_sprintf(pRealTime, "%s", sntpTimeChangeToSimpleDateFormat(pRealTime));
}

void ICACHE_FLASH_ATTR
sntp_read_timer_callback(void *arg)
{
	uint32_t time = sntp_get_current_timestamp();
	uint8 time1[25];
	os_sprintf(time1, "%s", sntp_get_real_time(time));
	SntpData_t sntpdata;
//	os_printf("time:%d\r\n",time);
//	os_printf("date:%s\r\n",sntp_get_real_time(time));
	os_printf("date:%s\r\n", sntpTimeChangeToSimpleDateFormat(time1));
}

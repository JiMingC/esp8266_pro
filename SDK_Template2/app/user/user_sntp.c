/*
 * user_sntp.c
 *
 *  Created on: 2022��2��18��
 *      Author: jiming
 */

#include "user_sntp.h"

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

static char* ICACHE_FLASH_ATTR
sntpTimestampToSntpdata(char *pSntpRealTime) {
	if(!pSntpRealTime)
	{
		return "";
	}

	pSntpRealTime[24] = '\0';						// ��Ҫ��ݺ��������
	uint8 dateType[7] = {3, 3, 2, 2, 2, 2, 4};		// 3-Fri 3-May 2-31 2-11: 2-21: 2-42 4-2019
	uint8 temp[5];
	uint8 i = 0, j = 0;
	SntpData_t sntpData;
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


static const int MON1[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    //ƽ��
static const int MON2[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    //����
static const int FOURYEARS = (366 + 365 +365 +365);    //ÿ�������������
static const int DAYMS = 24*3600;    //ÿ��ĺ�����
/**
 @brief SNTPʱ��ת��Ϊ�����ڸ�ʽ
 @param pSntpRealTime -[in] ʵʱʱ��
 @return �����ڸ�ʽʱ���ַ���
*/
char* ICACHE_FLASH_ATTR
sntpTimeChangeToSimpleDateFormat(char *pSntpRealTime, SntpData_t *sntpData, u32 timeS)
{
	if(!pSntpRealTime || (sntpData == NULL))
	{
		return "";
	}

	//timeS -= (8*3600);
	u32 nDays = timeS/24/3600 + 1;
	u32 nYear4 = nDays/1461;
	u32 nRemain = nDays%1461;
	u32 nDesYear = 1970 + nYear4*4;
	u32 nDesMonth = 0, nDesDay = 0;
	os_printf("%d %d %d %d\n", nDays, nYear4, nRemain, nDesYear);
	bool bLeapYear = false;
	u32 sRemain = timeS%DAYMS;
	if ( nRemain<365 )//һ�������ڣ���һ��
	{//ƽ��

	}
	else if ( nRemain<(365+365) )//һ�������ڣ��ڶ���
	{//ƽ��
		nDesYear += 1;
		nRemain -= 365;
	}
	else if ( nRemain<(365+365+365) )//һ�������ڣ�������
	{//ƽ��
		nDesYear += 2;
		nRemain -= (365+365);
	}
	else//һ�������ڣ������꣬��һ��������
	{//����
		nDesYear += 3;
		nRemain -= (365+365+366);
		bLeapYear = true;
	}
    int *pMonths = bLeapYear?MON2:MON1;
    int nTemp = nRemain;
    //ѭ����ȥ12������ÿ���µ�������ֱ��ʣ������С�ڵ���0�����ҵ��˶�Ӧ���·�
    for ( int i=0; i<12; ++i )
    {
        nTemp -= pMonths[i];
        if ( nTemp<=0 )
        {
        	nDesMonth = i+1;
            if ( nTemp == 0 )//��ʾ�պ�������µ����һ�죬��ô������������µ���������
            	nDesDay = pMonths[i];
            else
            	nDesDay = nDays;
            break;
        }
        nDays = nTemp;
    }
    sntpData->year = nDesYear;
    sntpData->month = nDesMonth;
    sntpData->day = nDesDay;
    sntpData->hour = sRemain/3600;
    sntpData->minute = sRemain/60%60;
    sntpData->second = sRemain%60;

	os_memset(pSntpRealTime, 0, 24);
	os_sprintf(pSntpRealTime, "%d-%d-%d %02d:%02d:%02d",
								sntpData->year, sntpData->month,
								sntpData->day, sntpData->hour,
								sntpData->minute, sntpData->second);
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
	os_printf("time:%d\r\n",time);
	os_printf("date:%s\r\n",sntp_get_real_time(time));
	os_printf("date:%s\r\n", sntpTimeChangeToSimpleDateFormat(time1, &sntpdata, time));
}


u8 ICACHE_FLASH_ATTR
SntpdataUpadte(SntpData_t *O_sntpData, SntpData_t N_sntpData) {
	if (O_sntpData == NULL)
		return 0;
	u8 ret = 0;
//	sntpData->year, sntpData->month,
//	sntpData->day, sntpData->hour,
//	sntpData->minute, sntpData->second);
	if (O_sntpData->year == N_sntpData.year)
		ret |= 0;
	else {
		ret |= 1;
		O_sntpData->year = N_sntpData.year;
	}
	ret <<= 1;

	if (O_sntpData->month == N_sntpData.month)
		ret |= 0;
	else {
		ret |= 1;
		O_sntpData->month = N_sntpData.month;
	}
	ret <<= 1;

	if (O_sntpData->day == N_sntpData.day)
		ret |= 0;
	else {
		ret |= 1;
		O_sntpData->day = N_sntpData.day;
	}
	ret <<= 1;

	if (O_sntpData->hour == N_sntpData.hour)
		ret |= 0;
	else {
		ret |= 1;
		O_sntpData->hour = N_sntpData.hour;
	}
	ret <<= 1;

	if (O_sntpData->minute == N_sntpData.minute)
		ret |= 0;
	else {
		ret |= 1;
		O_sntpData->minute = N_sntpData.minute;
	}
	ret <<= 1;

	if (O_sntpData->second == N_sntpData.second)
		ret |= 0;
	else {
		ret |= 1;
		O_sntpData->second = N_sntpData.second;
	}
	return ret;
}

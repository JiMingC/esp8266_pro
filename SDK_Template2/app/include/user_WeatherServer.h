/*
 * user_TcpServer.h
 *
 *  Created on: 2022Äê2ÔÂ18ÈÕ
 *      Author: jiming
 */

#ifndef APP_INCLUDE_USER_WEATHERSERVERH_
#define APP_INCLUDE_USER_WEATHERSERVERH_
#include "osapi.h"
#include "espconn.h"
#include "mem.h"
#include "os_type.h"
#include "driver/cJSON.h"
#include "stdlib.h"
#include "ets_sys.h"
//At the same time, only one can be 1
#define TCP_CLIENT 1
#define TCP_SERVER 0
#define UDP_TEST   0

#define TCP_SERVER_IP    "175.178.82.209"//TCP server IP
//#define TCP_SERVER_IP    "192.168.1.101"//TCP server IP,it is according your TCP server PC or device IP
#define TCP_SERVER_PORT  7070
#define TCP_LOCAL_PORT	 8888

#define WEATHER_SERVER	"116.62.81.138"
#define WEATHER_PORT	 80

typedef struct weatherData_t
{
	uint8 code;
	uint8 temperature;
	uint32 pressure;
	uint8 humidity;
	uint8 clouds;
	char local_name[20];
	char local_country[20];
	char last_update[30];
	uint8 update;
} WeatherData_t;

void tcp_weather_init(struct espconn *espconn,uint8 *remote_ip,struct ip_addr *local_ip, int remote_port) ;
sint8 weather_client_send_data(struct espconn *espconn,uint8 *pdata,uint16 length);
void weatherDataInit(WeatherData_t * wdata);
uint8 weather_codeparse(u8 code);
void DisplayWeatherInfo(WeatherData_t * wdata);
//void TCP_Send_data(struct espconn *espconn);
#endif /* APP_INCLUDE_USER_TCPSERVERH_ */

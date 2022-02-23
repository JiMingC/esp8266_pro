/*
 * user_TcpServer.h
 *
 *  Created on: 2022Äê2ÔÂ18ÈÕ
 *      Author: jiming
 */

#ifndef APP_INCLUDE_USER_TCPSERVERH_
#define APP_INCLUDE_USER_TCPSERVERH_
#include "osapi.h"
#include "espconn.h"
#include "mem.h"
#include "os_type.h"

//At the same time, only one can be 1
#define TCP_CLIENT 1
#define TCP_SERVER 0
#define UDP_TEST   0

#define TCP_SERVER_IP    "175.178.82.209"//TCP server IP
//#define TCP_SERVER_IP    "192.168.1.101"//TCP server IP,it is according your TCP server PC or device IP
#define TCP_SERVER_PORT  7070
#define TCP_LOCAL_PORT	 8888


void tcp_client_init(struct espconn *espconn,uint8 *remote_ip,struct ip_addr *local_ip, int remote_port);
sint8 tcp_client_send_data(struct espconn *espconn,uint8 *pdata,uint16 length);
//void TCP_Send_data(struct espconn *espconn);
#endif /* APP_INCLUDE_USER_TCPSERVERH_ */

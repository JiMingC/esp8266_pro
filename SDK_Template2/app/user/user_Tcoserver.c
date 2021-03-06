#include "../include/user_TcpServer.h"
#include "user_Msghandler.h"
#include "typedef.h"
/**********************************
 *   TCP CLIENT STATIC VARIABLES  *
 **********************************/
net_message_t *srcNetMsg;
extern u16 net_id;
extern u8 NetMsgBuff[];
extern struct espconn tcp_client;
/**********************************
 *   TCP CLIENT STATIC FUNCTIONS  *
 **********************************/

LOCAL void ICACHE_FLASH_ATTR
tcp_NetMsghandler(net_message_t *pstNetMsg){
	switch (pstNetMsg->enOpcode) {
		case EN_MSG_GIVE_USRID:
			net_id = pstNetMsg->body[0] << 8 | pstNetMsg->body[1];
			break;
		case EN_MSG_SINGLE_SEND:
			LOGD("recv from 0x%x:%s\n", pstNetMsg->netId, pstNetMsg->body);
			os_memcpy(NetMsgBuff, pstNetMsg->body, pstNetMsg->body_len - 1);
			break;
		case EN_MSG_ESP_TFTSHOW:
			os_memcpy(NetMsgBuff, pstNetMsg->body, pstNetMsg->body_len);
			break;
		default:
			break;
	}
	char buf[6];
	netreplyNetMsg(pstNetMsg->enOpcode, buf);
	tcp_client_send_data(&tcp_client, buf,6);
}

/**
 * TCP Client数据发送回调函数
 */
LOCAL void ICACHE_FLASH_ATTR
tcp_client_sent_cb(void *arg){
	LOGD("tcp client send data successful\r\n");
}

/**
 * TCP Client数据接收回调函数，可以在这处理收到Server发来的数据
 */
LOCAL int recv_pos = 0;
LOCAL int recv_t_len = 0;
LOCAL u8* recv_pdata;
LOCAL void ICACHE_FLASH_ATTR
tcp_client_recv_cb(void *arg,char *pdata,unsigned short len){
#if 0
	os_printf("tcp client receive tcp server data\r\n");
	os_printf("length: %d \r\ndata: %s\r\n",len,pdata);
#endif
	//LOGD("%d %d\n", recv_pos, recv_t_len);
	if (recv_pos == 0) {
		recv_t_len = ((pdata[6] << 8 | pdata[7]) + 8);
	}
	os_memcpy(recv_pdata + recv_pos, pdata, len);
	if (len >= (recv_t_len - recv_pos)) {
		recv_pos = 0;
		recv_t_len = 0;
	} else {
		recv_pos += len;
	}
	if (recv_pos == 0) {
		srcNetMsg = (net_message_t *)os_calloc(1,NET_MESSAGE_MAX_LENGTH);
		netparseMsg(srcNetMsg, recv_pdata);
		tcp_NetMsghandler(srcNetMsg);
		os_free(srcNetMsg);
	}

	//TO DO

	/**
	 *process the receive data
	 */
}

/**
 * TCP Client重连回调函数，可以在此函数里做重连接处理
 */
LOCAL void ICACHE_FLASH_ATTR
tcp_client_recon_cb(void *arg,sint8 error){
	LOGD("tcp client connect tcp server error %d\r\n",error);
}

/**
 * TCP Client断开连接回调函数
 */
LOCAL void ICACHE_FLASH_ATTR
tcp_client_discon_cb(void *arg){
	LOGD("tcp client disconnect tcp server successful\r\n");
	recv_pos = 0;
	recv_t_len = 0;
}

/**
 * TCP Client连接成功回调函数
 */
LOCAL void ICACHE_FLASH_ATTR
tcp_client_connect_cb(void *arg){
	struct espconn *pespconn = arg;

	LOGD("tcp client connect tcp server successful\r\n");
	espconn_regist_recvcb(pespconn,tcp_client_recv_cb);//注册接收数据回调函数
	espconn_regist_sentcb(pespconn,tcp_client_sent_cb);//注册数据发送完成回调函数
	espconn_regist_disconcb(pespconn,tcp_client_discon_cb);//注册断开连接回调函数

}
/**
 * TCP Client初始化函数
 * @remote_ip    要连接的TCP Server IP地址
 * @local_ip     ESP8266 IP地址
 * @remote_port  要连接的TCP Server端口号
 */
void ICACHE_FLASH_ATTR
tcp_client_init(struct espconn *espconn,uint8 *remote_ip,struct ip_addr *local_ip, int remote_port) {

	uint32 server_ip = ipaddr_addr(remote_ip);

	LOGD("TCP client connect to tcp server\r\n");
	espconn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	espconn->type = ESPCONN_TCP;

	os_memcpy(espconn->proto.tcp->remote_ip,&server_ip,4);//设置要连接的Server IP地址
	espconn->proto.tcp->remote_port = remote_port;//设置要连接的Server 端口号
	os_memcpy(espconn->proto.tcp->local_ip,local_ip,4);//设置本地IP地址
	espconn->proto.tcp->local_port = TCP_LOCAL_PORT;//设置本地端口号

	espconn_regist_connectcb(espconn,tcp_client_connect_cb);//注册连接成功回调函数
	espconn_regist_reconcb(espconn,tcp_client_recon_cb);//注册断连重新连接回调函数

	espconn_connect(espconn);//Client连接Server
	recv_pdata = (u8*)os_calloc(1,NET_MESSAGE_MAX_LENGTH);
}

/**********************************
 *   TCP CLIENT GLOBAL FUNCTIONS  *
 **********************************/
/**
 * TCP Client定时发送数据回调函数
 */
sint8 ICACHE_FLASH_ATTR
tcp_client_send_data(struct espconn *espconn,uint8 *pdata,uint16 length){
    if (espconn == NULL) {
    	return ESPCONN_ARG;
    }
    espconn_send(espconn,pdata,length);
}

//LOCAL uint8  messages_send_buffer[50];
//LOCAL uint16 messages_send_count = 0;
//void ICACHE_FLASH_ATTR
//TCP_Send_data(struct espconn *espconn){
//#if   TCP_CLIENT
//	os_sprintf(messages_send_buffer,"hi this is ESP8266 TCP client![%d]\r\n",messages_send_count);
//	tcp_client_send_data(espconn,messages_send_buffer,strlen(messages_send_buffer));
//#elif TCP_SERVER
//	os_sprintf(messages_send_buffer,"hi this is ESP8266 TCP server![%d]\r\n",messages_send_count);
//	tcp_server_send_data(tcp_server,messages_send_buffer,strlen(messages_send_buffer));
//#endif
//	messages_send_count++;
//}

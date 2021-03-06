#include "../include/user_WeatherServer.h"

extern WeatherData_t weatherData;
/**********************************
 *   TCP CLIENT STATIC VARIABLES  *
 **********************************/

/**********************************
 *   TCP CLIENT STATIC FUNCTIONS  *
 **********************************/
void ICACHE_FLASH_ATTR
weatherDataInit(WeatherData_t * wdata) {
	wdata->clouds = 0;
	wdata->code = 0;
	wdata->humidity = 0;
	wdata->pressure = 0;
	wdata->temperature = 0;
	wdata->update = 0;
}

LOCAL u8 ICACHE_FLASH_ATTR
parse_weatherJS(char *receiveData,unsigned short len, WeatherData_t * wdata){
	// receiveData是要剖析的数据
	//首先整体判断是否为一个json格式的数据
	cJSON *pJsonRoot = cJSON_Parse(receiveData);
	cJSON *arrayItem, *object, *item;
	//如果是否json格式数据
	if (pJsonRoot !=NULL)
	{
		arrayItem = cJSON_GetObjectItem(pJsonRoot, "results");
		if (!arrayItem) {
			cJSON_Delete(pJsonRoot);
			return 1;
		}
		else {
			int arryLength = cJSON_GetArraySize(arrayItem);              // 获取数组长度
		    int i;
		    for (i = 0; i < 1; i++)
		    {                                                        // 打印数组内容
		    	wdata->update = 1;
		        //os_printf("cJSON_GetArrayItem(pArry, %d)= %d\n",i,cJSON_GetArrayItem(arrayItem, i)->valueint);
		        object=cJSON_GetArrayItem(arrayItem,i);
		        object=cJSON_GetObjectItem(object,"now");
		        item=cJSON_GetObjectItem(object,"temperature");
		        if(item!=NULL)
					wdata->temperature = atoi(item->valuestring);

		        item=cJSON_GetObjectItem(object,"code");
		        if(item!=NULL)
					wdata->code = atoi(item->valuestring);

//		        item=cJSON_GetObjectItem(object,"humidity");
//		        if(item!=NULL)
//					wdata->humidity = atoi(item->valuestring);
//
//		        item=cJSON_GetObjectItem(object,"clouds");
//		        if(item!=NULL)
//					wdata->clouds = atoi(item->valuestring);



		        object=cJSON_GetArrayItem(arrayItem,i);
		        object=cJSON_GetObjectItem(object,"location");
		        item=cJSON_GetObjectItem(object,"name");
		        if(item!=NULL)
		        	os_memcpy(wdata->local_name, item->valuestring, os_strlen(item->valuestring));

		        item=cJSON_GetObjectItem(object,"country");
		        if(item!=NULL)
		        	os_memcpy(wdata->local_country, item->valuestring, os_strlen(item->valuestring));
		        cJSON_Delete(pJsonRoot);
		        return 0;
		    }
		}
	} else {
		cJSON_Delete(pJsonRoot);
		return 1;
	}
}

/**
 * TCP Client数据发送回调函数
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_sent_cb(void *arg){
	LOGD("weather client send data successful\r\n");
}

/**
 * TCP Client数据接收回调函数，可以在这处理收到Server发来的数据
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_recv_cb(void *arg,char *pdata,unsigned short len){
#ifdef DEBUG
	os_printf("weather client receive tcp server data\r\n");
	os_printf("length: %d \r\ndata: %s\r\n",len,pdata);
#endif
	//TO DO

	/**
	 *process the receive data
	 */
	parse_weatherJS(pdata, len, &weatherData);
}

/**
 * TCP Client重连回调函数，可以在此函数里做重连接处理
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_recon_cb(void *arg,sint8 error){
	LOGD("weather client connect tcp server error %d\r\n",error);
}

/**
 * TCP Client断开连接回调函数
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_discon_cb(void *arg){
	LOGD("weather client disconnect tcp server successful\r\n");
}

/**
 * TCP Client连接成功回调函数
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_connect_cb(void *arg){
	struct espconn *pespconn = arg;

	LOGD("weather client connect tcp server successful\r\n");
	espconn_regist_recvcb(pespconn,weather_client_recv_cb);//注册接收数据回调函数
	espconn_regist_sentcb(pespconn,weather_client_sent_cb);//注册数据发送完成回调函数
	espconn_regist_disconcb(pespconn,weather_client_discon_cb);//注册断开连接回调函数

}
/**
 * weather Client初始化函数
 * @remote_ip    要连接的TCP Server IP地址
 * @local_ip     ESP8266 IP地址
 * @remote_port  要连接的TCP Server端口号
 */
void ICACHE_FLASH_ATTR
tcp_weather_init(struct espconn *espconn,uint8 *remote_ip,struct ip_addr *local_ip, int remote_port) {

	uint32 server_ip = ipaddr_addr(remote_ip);

	os_printf("weather client connect to tcp server\r\n");
	espconn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	espconn->type = ESPCONN_TCP;

	os_memcpy(espconn->proto.tcp->remote_ip,&server_ip,4);//设置要连接的Server IP地址
	espconn->proto.tcp->remote_port = remote_port;//设置要连接的Server 端口号
	os_memcpy(espconn->proto.tcp->local_ip,local_ip,4);//设置本地IP地址
	espconn->proto.tcp->local_port = WEATHER_LOCAL_PORT;//设置本地端口号

	espconn_regist_connectcb(espconn,weather_client_connect_cb);//注册连接成功回调函数
	espconn_regist_reconcb(espconn,weather_client_recon_cb);//注册断连重新连接回调函数

	espconn_connect(espconn);//Client连接Server
}
/**********************************
 *   TCP CLIENT GLOBAL FUNCTIONS  *
 **********************************/
/**
 * TCP Client定时发送数据回调函数
 */
sint8 ICACHE_FLASH_ATTR
weather_client_send_data(struct espconn *espconn,uint8 *pdata,uint16 length){
    if (espconn == NULL) {
    	return ESPCONN_ARG;
    }
    espconn_send(espconn,pdata,length);
}

uint8 ICACHE_FLASH_ATTR
weather_codeparse(u8 code){
	u8 ret = 0;
    if (code == 0 || code == 2) {
    	ret = 1;
    } else if (code == 1 || code == 3) {
    	ret = 2;
    } else if (code == 4 || code == 6 || code == 8 || code == 9) {
    	ret = 3;
    } else if (code == 5 || code == 7) {
    	ret = 4;
    } else if (code == 10) {
    	ret = 5;
    } else if (code == 11) {
    	ret = 6;
    } else if (code == 13 || code == 14) {
    	ret = 7;
    } else if (code >= 15 || code <= 18) {
    	ret = 8;
    } else {
    	ret = 0;
    }
    return ret;
}



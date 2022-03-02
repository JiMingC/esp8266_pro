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
	// receiveData��Ҫ����������
	//���������ж��Ƿ�Ϊһ��json��ʽ������
	cJSON *pJsonRoot = cJSON_Parse(receiveData);
	cJSON *arrayItem, *object, *item;
	//����Ƿ�json��ʽ����
	if (pJsonRoot !=NULL)
	{
		arrayItem = cJSON_GetObjectItem(pJsonRoot, "results");
		if (!arrayItem) {
			cJSON_Delete(pJsonRoot);
			return 1;
		}
		else {
			int arryLength = cJSON_GetArraySize(arrayItem);              // ��ȡ���鳤��
		    int i;
		    for (i = 0; i < 1; i++)
		    {                                                        // ��ӡ��������
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
 * TCP Client���ݷ��ͻص�����
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_sent_cb(void *arg){
	os_printf("tcp client send data successful\r\n");
}

/**
 * TCP Client���ݽ��ջص��������������⴦���յ�Server����������
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_recv_cb(void *arg,char *pdata,unsigned short len){
	os_printf("tcp client receive tcp server data\r\n");
	os_printf("length: %d \r\ndata: %s\r\n",len,pdata);

	//TO DO

	/**
	 *process the receive data
	 */
	parse_weatherJS(pdata, len, &weatherData);
}

/**
 * TCP Client�����ص������������ڴ˺������������Ӵ���
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_recon_cb(void *arg,sint8 error){
	os_printf("tcp client connect tcp server error %d\r\n",error);
}

/**
 * TCP Client�Ͽ����ӻص�����
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_discon_cb(void *arg){
	os_printf("weather client disconnect tcp server successful\r\n");
}

/**
 * TCP Client���ӳɹ��ص�����
 */
LOCAL void ICACHE_FLASH_ATTR
weather_client_connect_cb(void *arg){
	struct espconn *pespconn = arg;

	os_printf("weather client connect tcp server successful\r\n");
	espconn_regist_recvcb(pespconn,weather_client_recv_cb);//ע��������ݻص�����
	espconn_regist_sentcb(pespconn,weather_client_sent_cb);//ע�����ݷ�����ɻص�����
	espconn_regist_disconcb(pespconn,weather_client_discon_cb);//ע��Ͽ����ӻص�����

}
/**
 * weather Client��ʼ������
 * @remote_ip    Ҫ���ӵ�TCP Server IP��ַ
 * @local_ip     ESP8266 IP��ַ
 * @remote_port  Ҫ���ӵ�TCP Server�˿ں�
 */
void ICACHE_FLASH_ATTR
tcp_weather_init(struct espconn *espconn,uint8 *remote_ip,struct ip_addr *local_ip, int remote_port) {

	uint32 server_ip = ipaddr_addr(remote_ip);

	os_printf("weather client connect to tcp server\r\n");
	espconn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	espconn->type = ESPCONN_TCP;

	os_memcpy(espconn->proto.tcp->remote_ip,&server_ip,4);//����Ҫ���ӵ�Server IP��ַ
	espconn->proto.tcp->remote_port = remote_port;//����Ҫ���ӵ�Server �˿ں�
	os_memcpy(espconn->proto.tcp->local_ip,local_ip,4);//���ñ���IP��ַ
	espconn->proto.tcp->local_port = TCP_LOCAL_PORT;//���ñ��ض˿ں�

	espconn_regist_connectcb(espconn,weather_client_connect_cb);//ע�����ӳɹ��ص�����
	espconn_regist_reconcb(espconn,weather_client_recon_cb);//ע������������ӻص�����

	espconn_connect(espconn);//Client����Server
}
/**********************************
 *   TCP CLIENT GLOBAL FUNCTIONS  *
 **********************************/
/**
 * TCP Client��ʱ�������ݻص�����
 */
sint8 ICACHE_FLASH_ATTR
weather_client_send_data(struct espconn *espconn,uint8 *pdata,uint16 length){
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



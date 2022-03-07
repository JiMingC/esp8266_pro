/*
 * typedef.h
 *
 *  Created on: 2022��3��4��
 *      Author: jiming
 */

#ifndef APP_INCLUDE_TYPEDEF_H_
#define APP_INCLUDE_TYPEDEF_H_

#include "osapi.h"
#include "espconn.h"
#include "mem.h"
#include "os_type.h"

#define INFO_OUTPUT      3
#define WARNING_OUTPUT   2
#define DEBUG_OUTPUT     1
#define ERROR_OUTPUT     0

#define DEBUG_LEVEL     INFO_OUTPUT
//����ʹ�����´�ӡ���.c�ļ��ж��塰DEBUG_LEVEL������
#define LOGI(info,...)  \
do{ \
    if(DEBUG_LEVEL>=INFO_OUTPUT){\
        os_printf("[Info]  %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#define LOGW(info,...)  \
do{ \
    if(DEBUG_LEVEL>=WARNING_OUTPUT){\
        os_printf("[Warn]  %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#define LOGD(info,...)  \
do{ \
    if(DEBUG_LEVEL>=DEBUG_OUTPUT){\
        os_printf("[Debug] %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#define LOGE(info,...)  \
do{ \
    if(DEBUG_LEVEL>=ERROR_OUTPUT){\
        os_printf("[Error] %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#endif /* APP_INCLUDE_TYPEDEF_H_ */

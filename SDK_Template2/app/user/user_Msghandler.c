/*
 * user_Msghandler.c
 *
 *  Created on: 2022Äê3ÔÂ4ÈÕ
 *      Author: jiming
 */
#include "user_Msghandler.h"
#include "typedef.h"

extern u16 net_id;
int netenOpcodeNetMsg(Net_MSG_Opcode_LIST enOpcode, char *buf) {
    buf[0] = NET_ADDR_DEV;
    buf[1] = NET_ADDR_SERVER;
    buf[2] = 0;
    buf[3] = enOpcode;
    buf[4] = net_id >> 8;
    buf[5] = net_id;
    //buf[5] = 0;

    return 6;
}

int netreplyNetMsg(Net_MSG_Opcode_LIST enOpcode, char *buf) {
    buf[0] = NET_ADDR_DEV;
    buf[1] = NET_ADDR_SERVER;
    buf[2] = 0;
    buf[3] = enOpcode + 1;
    buf[4] = net_id >> 8;
    buf[5] = net_id;
    //buf[6] = 0;

    return 6;
}

int netparseMsg(net_message_t *pstNetMsg, unsigned char* buf) {
    int i = 0;
    os_memset(pstNetMsg, 0, NET_MESSAGE_MAX_LENGTH);
    if (buf == NULL)
        return -1;

	pstNetMsg->initiator = buf[0];
	pstNetMsg->destination = buf[1];
	pstNetMsg->enOpcode = (Net_MSG_Opcode_LIST)buf[3];
	pstNetMsg->netId = (buf[4] << 8) + (buf[5]);
	pstNetMsg->body_len = buf[6] << 8 | buf[7];
	for(i = 0 ;i< pstNetMsg->body_len;i++)
	{
	   pstNetMsg->body[i] = buf[i + 8];
	}
	LOGD("0:%x 1:%x 2:%x 3:%x 4:%x 5:%x 6:%x 7:%x\n",
			buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
	pr_netMsg(pstNetMsg);

    return 0;
}

void pr_netMsg(net_message_t *srcNetMsg) {
    if (srcNetMsg == NULL)
        return;
    LOGD("initiator[0x%x] destination[0x%x] enOpcode[0x%x] netID[%d] len[%d]\n",
                        srcNetMsg->initiator,
                        srcNetMsg->destination,
                        srcNetMsg->enOpcode,
                        srcNetMsg->netId,
                        srcNetMsg->body_len);
    switch(srcNetMsg->enOpcode) {
        case EN_MSG_GIVE_USRNAME :
            LOGD("GIVE_USRNAME: %s\n", (char*)srcNetMsg->body);
            break;
        case EN_MSG_GIVE_USRID :
        	LOGD("GIVE_USRID: 0x%4x\n", (srcNetMsg->body[0] << 8 | srcNetMsg->body[1]));
        	break;
    }
}

#include "zzigbee_handler.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "zlog.h"

#include "zinner_message.h"
#include "zmodule.h"
#include "zdispatcher.h"
#include "zerrno.h"

int ZZigBeeHandler::init()
{
	return 0;
}

int ZZigBeeHandler::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZZigBeeHandler::onInnerMsg(%p)\n", msg);
	return 0;
}

int ZZigBeeHandler::event(char *buf, uint32_t buf_len)
{
	uint8_t msg_type = ZZigBeeMsg::getMsgType(buf, buf_len);
	switch (msg_type) {
		case Z_ID_ZB_REG_REQ:
			{
				printf("Z_ID_ZB_REG_REQ\n");
				ZZBRegReq msg;
				int rv = msg.decode(buf, buf_len);
				if (rv < 0) {
					printf("Failed to decode message\n");
				} else {
					printf("decoding success\n");
					processMsg(msg);
				}
				break;
			}
	case Z_ID_ZB_GET_RSP:
		{
			printf("Z_ID_ZB_GET_RSP\n");
			ZZBGetRsp msg;
			int rv = msg.decode(buf, buf_len);
			if (rv < 0) {
				printf("Failed to decode message\n");
			} else {
				printf("decoding success\n");
				processMsg(msg);
			}
			break;
		}
	case Z_ID_ZB_SET_RSP:
		{
			printf("Z_ID_ZB_SET_RSP\n");
			ZZBSetRsp msg;
			int rv = msg.decode(buf, buf_len);
			if (rv < 0) {
				printf("Failed to decode message\n");
			} else {
				printf("decoding success\n");
				processMsg(msg);
			}
			break;
		}
	default:
		{
			printf("Unknow message: %u\n", msg_type);
			break;
		}
	}

	return OK;
}

int ZZigBeeHandler::processMsg(ZZBRegReq &msg)
{
	printf("MAC:\n");
	trace_bin(msg.mac_.c_str(), msg.mac_.size());

	ZZBRegRsp rsp;
	rsp.addr_ = 0x88;
	rsp.status_ = 0x00;

	int rv = rsp.encode(buf_, sizeof(buf_));
	if (rv < 0) {
		printf("failed to encode register response\n");
		return -1;
	}

	send(fd_, buf_, rv, 0);

	return 0;
}

int ZZigBeeHandler::processMsg(ZZBGetRsp &msg)
{
	printf("rsp item count: %ld\n", msg.items_.size());
	for (unsigned int i = 0; i < msg.items_.size(); ++i) {
		printf("items.id[%d]: 0x%02X\n", i, msg.items_[i].id);
		printf("items.val[%d]: 0x%02X\n", i, msg.items_[i].val);
	}
	// XXX: should remember where did the request came
	ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_WEBAPI, 1);
	innerMsg->msgType = Z_ZB_GET_DEV_RSP;
	innerMsg->data = &msg;
	ZDispatcher::instance()->sendMsg(innerMsg);
	return 0;
}

int ZZigBeeHandler::processMsg(ZZBSetRsp &msg)
{
	printf("rsp status: %d\n", msg.status_);

	// XXX: should remember where did the request came
	ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_WEBAPI, 1);
	innerMsg->msgType = Z_ZB_SET_DEV_RSP;
	innerMsg->data = &msg;
	ZDispatcher::instance()->sendMsg(innerMsg);
	return 0;
}




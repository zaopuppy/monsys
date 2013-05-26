#include "zzigbee_handler.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "zlog.h"

#include "zmodule.h"
#include "zdispatcher.h"
#include "zerrno.h"

// /////////////////////////////////////////////////////////////////////
// // ZZBInnerGetDevSession
// int ZZBInnerGetDevSession::event(ZMsg *msg) {
// 	// ZZBGetReq *req = (ZZBGetReq*) msg;
// 	return 0;
// }

/////////////////////////////////////////////////////////////////////

int ZZigBeeHandler::init()
{
	return 0;
}

void ZZigBeeHandler::close()
{
}

int ZZigBeeHandler::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZZigBeeHandler::onInnerMsg(%p)\n", msg);
	switch (msg->msg_type_) {
		case Z_ZB_GET_DEV_REQ:
			{
				return processMsg((ZInnerGetDevInfoReq*)msg);
			}
		case Z_ZB_SET_DEV_REQ:
			{
				// ZZBSetReq *req = (ZZBSetReq*) msg->data;
				// return processMsg(*req);
				return -1;
			}
		default:
			printf("Unknown message type!\n");
			return -1;
	}
}

int ZZigBeeHandler::onRead(char *buf, uint32_t buf_len)
{
	ZZigBeeMsg hdr;
	int rv = hdr.decode(buf, buf_len);
	if (rv < 0) {
		printf("Failed to decode header\n");
		return -1;
	}

	// uint8_t msg_type = ZZigBeeMsg::getMsgType(buf, buf_len);
	// switch (msg_type) {
	switch (hdr.cmd_) {
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
			printf("Unknow message: %u\n", hdr.cmd_);
			break;
		}
	}

	return OK;
}

void ZZigBeeHandler::initZBDevStruct(zb_dev_t *dev)
{
	memset(dev->mac, 0x00, sizeof(dev->mac));
	dev->state = zb_dev_state_invalid;
	dev->id_val_map.clear();
}

uint8_t ZZigBeeHandler::genDevAddr()
{
	// first find out the slot which is never used before
	for (uint8_t i = 1; i < DEV_LIST_LEN; ++i) {
		if (zb_dev_list_[i].state == zb_dev_state_invalid) {
			return i;
		}
	}

	// then try to free inactive slot
	for (uint8_t i = 1; i < DEV_LIST_LEN; ++i) {
		if (zb_dev_list_[i].state == zb_dev_state_inactive) {
			initZBDevStruct(&zb_dev_list_[i]);
			return i = 1;
		}
	}

	return 0;
}

int ZZigBeeHandler::processMsg(ZZBRegReq &msg)
{
	printf("MAC:\n");
	trace_bin(msg.mac_.c_str(), msg.mac_.size());

	ZZBRegRsp rsp;

	uint8_t addr = genDevAddr();
	if (addr == 0) {
		printf("Can't generate more addr, Too many devices\n");
		rsp.addr_ = 0;		// invalid
		rsp.status_ = 1;
		int rv = rsp.encode(buf_, sizeof(buf_));
		if (rv < 0) {
			printf("failed to encode register response\n");
			return -1;
		}

		// write(fd_, buf_, rv);
		send(buf_, rv);

		return -1;
	}

	// 1. send normal response first
	zb_dev_list_[addr].state = zb_dev_state_active;

	rsp.addr_ = addr;
	rsp.status_ = 0x00;

	int rv = rsp.encode(buf_, sizeof(buf_));
	if (rv < 0) {
		printf("failed to encode register response\n");
		return -1;
	}

	send(buf_, rv);
	// write(fd_, buf_, rv);

	// 2. send query request
	ZZBGetReq req;

	req.items_.push_back(ITEM_ID_ALL);

	processMsg(req);

	return 0;
}

int ZZigBeeHandler::processMsg(ZZBGetReq &msg)
{
	int rv = msg.encode(buf_, sizeof(buf_));
	if (rv < 0) {
		printf("Failed to encode\n");
		return -1;
	}
	// delete req;

	trace_bin(buf_, rv);

	rv = send(buf_, rv);
	// rv = write(fd_, buf_, rv);
	if (rv <= 0) {
		perror("send");
		printf("Failed to send\n");
	} else {
		printf("write over.\n");
	}

	return OK;
}

int ZZigBeeHandler::processMsg(ZZBGetRsp &msg)
{
	// printf("rsp item count: %ld\n", msg.items_.size());
	// for (unsigned int i = 0; i < msg.items_.size(); ++i) {
	// 	printf("items.id[%d]: 0x%02X\n", i, msg.items_[i].id);
	// 	printf("items.val[%d]: 0x%02X\n", i, msg.items_[i].val);
	// }
	// // XXX: should remember where did the request came
	// ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_WEBAPI, 1);
	// innerMsg->msgType = Z_ZB_GET_DEV_RSP;
	// innerMsg->data = &msg;
	// ZDispatcher::instance()->sendMsg(innerMsg);

	for (unsigned int i = 0; i < msg.items_.size(); ++i) {
		printf("items.id[%d]: 0x%02X\n", i, msg.items_[i].id);
		printf("items.val[%d]: 0x%02X\n", i, msg.items_[i].val);
	}

	return 0;
}

int ZZigBeeHandler::processMsg(ZZBSetReq &msg)
{
	int rv = msg.encode(buf_, sizeof(buf_));
	if (rv < 0) {
		printf("Failed to encode\n");
		return -1;
	}
	// delete req;

	trace_bin(buf_, rv);

	rv = send(buf_, rv);
	// rv = write(fd_, buf_, rv);
	if (rv <= 0) {
		perror("send");
		printf("Failed to send\n");
	} else {
		printf("write over.\n");
	}

	return OK;
}

int ZZigBeeHandler::processMsg(ZZBSetRsp &msg)
{
	printf("rsp status: %d\n", msg.status_);

	// // XXX: should remember where did the request came
	// ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_WEBAPI, 1);
	// innerMsg->msgType = Z_ZB_SET_DEV_RSP;
	// innerMsg->data = &msg;
	// ZDispatcher::instance()->sendMsg(innerMsg);
	return 0;
}

void ZZigBeeHandler::printDevInfo()
{
	for (int i = 1; i < DEV_LIST_LEN; ++i) {
		if (zb_dev_list_[i].state == zb_dev_state_active) {
			printf("idx: %d\n", i);
		}
	}
}

int ZZigBeeHandler::processMsg(ZInnerGetDevInfoReq *msg)
{
	printf("ZZigBeeHandler::processMsg(ZInnerGetDevInfoRsp)\n");

	printDevInfo();

	ZItemPair pair;
	ZInnerGetDevInfoRsp *rsp = new ZInnerGetDevInfoRsp(addr_);

	uint8_t i;
	for (i = 1; i < DEV_LIST_LEN; ++i) {
		if (msg->dev_addr_ == i) {
			pair.id = 1;
			pair.val = zb_dev_list_[i].state;
			rsp->dev_infos_.push_back(pair);
			break;
		}
	}

	if (i == DEV_LIST_LEN) {
		printf("invalid address: %d\n", msg->dev_addr_);
	}

	rsp->setSeq(msg->getSeq());
	ZDispatcher::instance()->sendMsg(rsp);

	return 0;
}



#include "zzigbee_handler.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>

#include "zmodule.h"
#include "zdispatcher.h"
#include "zerrno.h"

/////////////////////////////////////////////////////////////////////

int ZZigBeeHandler::init()
{
	printf("fd_: %d\n", fd_);
	return 0;
}

void ZZigBeeHandler::close()
{
}

int ZZigBeeHandler::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZZigBeeHandler::onInnerMsg(%p)\n", msg);

	// printf("fd_: %d\n", fd_);
	// if (fd_ < 0) {
	// 	printf("Invalid fd, not connected?\n");
	// 	return -1;
	// }

	switch (msg->msg_type_) {
		case Z_ZB_GET_DEV_LIST_REQ:
			{
				return processMsg((ZInnerGetDevListReq*)msg);
			}
		case Z_ZB_GET_DEV_REQ:
			{
				return processMsg((ZInnerGetDevInfoReq*)msg);
			}
		case Z_ZB_SET_DEV_REQ:
			{
				return processMsg((ZInnerSetDevInfoReq*)msg);
			}
		default:
			printf("Unknown message type: %u", msg->msg_type_);
			return -1;
	}
}

int ZZigBeeHandler::onRead(char *buf, uint32_t buf_len)
{
	printf("ZZigBeeHandler::onRead()\n");

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

int ZZigBeeHandler::processMsg(ZZBRegReq &msg)
{
	printf("ZZigBeeHandler::processMsg(RegReq)\n");

	ZZBRegRsp rsp;
	if (!dev_manager_.add(msg.mac_, msg.addr_)) {
		printf("Failed to add device to device manager\n");
		rsp.status_ = -1;
	} else {
		printf("Added/Updated into device manager\n");
		rsp.status_ = 0;
	}

	rsp.addr_ = msg.addr_;

	int rv = rsp.encode(buf_, sizeof(buf_));
	if (rv < 0) {
		printf("failed to encode register response\n");
		return -1;
	}

	send(buf_, rv);

	// // TODO:
	// // 2. send query request(with item id zero (means all))
	// ZZBGetReq req;

	// req.items_.push_back(ITEM_ID_ALL);

	// processMsg(req);

	return 0;
}

int ZZigBeeHandler::processMsg(ZZBGetRsp &msg)
{
	printf("ZZigBeeHandler::processMsg(ZZBGetRsp)\n");

	printf("rsp item count: %ld\n", msg.items_.size());

	ZInnerGetDevInfoRsp *rsp = new ZInnerGetDevInfoRsp(addr_);
	ZItemPair pair;

	for (uint32_t i = 0; i < msg.items_.size(); ++i) {
		printf("items.id[%d]: 0x%02X\n", i, msg.items_[i].id);
		printf("items.val[%d]: 0x%02X\n", i, msg.items_[i].val);
		pair.id = msg.items_[i].id;
		pair.val = msg.items_[i].val;
		rsp->dev_infos_.push_back(pair);
	}

	ZDispatcher::instance()->sendMsg(rsp);

	return 0;
}

int ZZigBeeHandler::processMsg(ZZBSetRsp &msg)
{
	printf("rsp status: %d\n", msg.status_);

	ZInnerSetDevInfoRsp *rsp = new ZInnerSetDevInfoRsp(addr_);
	rsp->status_ = msg.status_;

	ZDispatcher::instance()->sendMsg(rsp);

	return 0;
}

void ZZigBeeHandler::printDevInfo()
{
	// for (int i = 1; i < DEV_LIST_LEN; ++i) {
	// 	if (zb_dev_list_[i].info.state == zb_dev_state_active) {
	// 		printf("idx: %d\n", i);
	// 	}
	// }
}

int ZZigBeeHandler::processMsg(ZInnerGetDevListReq *msg)
{
	printf("ZZigBeeHandler::processMsg(ZInnerGetDevListReq)\n");

	ZInnerGetDevListRsp *rsp = new ZInnerGetDevListRsp(addr_);
	ZZBDevInfo *info = NULL;

	const ZDevManager::MAC_DEV_MAP_TYPE &dev_map = dev_manager_.getMacDevMap();
	ZDevManager::MAC_DEV_MAP_TYPE::const_iterator iter = dev_map.begin();
	for (; iter != dev_map.end(); ++iter) {
		printf("count\n");
		info = new ZZBDevInfo(*(iter->second));
		rsp->info_list_.push_back(info);
	}

	ZDispatcher::instance()->sendMsg(rsp);

	return 0;
}

int ZZigBeeHandler::processMsg(ZInnerGetDevInfoReq *msg)
{
	printf("ZZigBeeHandler::processMsg(ZInnerGetDevInfoReq)\n");

	// printDevInfo();

	ZZBGetReq req;
	req.addr_ = msg->addr_;

	for (uint32_t i = 0; i < msg->item_ids_.size(); ++i) {
		req.items_.push_back(msg->item_ids_[i]);
	}

	int rv = req.encode(buf_, sizeof(buf_));
	if (rv < 0) {
		printf("Failed to encode GetReq\n");
		return -1;
	}

	send(buf_, rv);

	return 0;
}

int ZZigBeeHandler::processMsg(ZInnerSetDevInfoReq *msg)
{
	// [ {"id":1, "val":4}, {"id":2, "val":5} ]
	printf("ZZigBeeHandler::processMsg(ZInnerSetDevInfoReq)\n");

	// printDevInfo();
	// // -- for debugging only --
	// ZInnerSetDevInfoRsp *rsp = new ZInnerSetDevInfoRsp(addr_);
	// rsp->status_ = 0;
	// ZDispatcher::instance()->sendMsg(rsp);
	// return 0;
	// // -- for debugging only --

	// // set all device
	// if (msg->addr_ == 0) {
	// 	printf("Trying to set all devices\n");
	// }
	ZZBDevInfo *dev_info = dev_manager_.find(msg->addr_);
	if (dev_info == NULL) {
		printf("wrong address\n");
		return -1;
	}

	ZZBSetReq req;
	req.addr_ = msg->addr_;
	for (size_t i = 0; i < msg->dev_vals_.size(); ++i) {
		req.items_.push_back(msg->dev_vals_[i]);
	}

	int rv = req.encode(buf_, sizeof(buf_));
	if (rv < 0) {
		printf("Failed to encode ZZBSetReq\n");
		return -1;
	}

	trace_bin(buf_, rv);

	rv = send(buf_, rv);
	if (rv <= 0) {
		perror("send");
		printf("Failed to send\n");
	} else {
		printf("write over.\n");
	}

	return 0;
}



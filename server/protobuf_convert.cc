#include "protobuf_convert.h"

#include "libbase/zlog.h"

PushMsg* new_pushmsg(MsgType type)
{
  PushMsg *msg = new PushMsg();
  msg->set_version(1);
  msg->set_type(type);
  return msg;
}

ZInnerMsg* push2inner(const GetDevList &msg)
{
  ZInnerGetDevListReq *req = new ZInnerGetDevListReq();
  return req;
}

ZInnerMsg* push2inner(const GetDevInfo &msg)
{
  ZInnerGetDevInfoReq *req = new ZInnerGetDevInfoReq();
  req->addr_ = msg.addr();
  int item_ids_count = msg.item_ids_size();
  for (int i = 0; i < item_ids_count; ++i) {
    req->item_ids_.push_back(msg.item_ids(i));
  }
  return req;
}

ZInnerMsg* push2inner(const SetDevInfo &msg)
{
  ZInnerSetDevInfoReq *req = new ZInnerSetDevInfoReq();
  req->addr_ = msg.addr();
  int id_val_pair_count = msg.id_value_pairs_size();
  ZItemPair item_pair;
  for (int i = 0; i < id_val_pair_count; ++i) {
    const IdValuePair &pair = msg.id_value_pairs(i);
    item_pair.id = pair.id();
    item_pair.val = pair.value();
    req->dev_vals_.push_back(item_pair);
  }
  return req;
}

ZInnerMsg* push2inner(const PushMsg &push_msg)
{
  switch (push_msg.type()) {
    case GET_DEV_LIST:
    {
      if (!push_msg.has_get_dev_list()) {
        Z_LOG_E("get_dev_list but we got nothing");
        return NULL;
      }
      return push2inner(push_msg.get_dev_list());
    }
    case GET_DEV_INFO:
    {
      if (!push_msg.has_get_dev_info()) {
        Z_LOG_E("get_dev_info but we got nothing");
        return NULL;
      }
      return push2inner(push_msg.get_dev_info());
    }
    case SET_DEV_INFO:
    {
      if (!push_msg.has_set_dev_info()) {
        Z_LOG_E("set_dev_info but we got nothing");
        return NULL;
      }
      return push2inner(push_msg.set_dev_info());
    }
    default:
      return NULL;
  }
}

PushMsg* inner2push(const ZInnerGetDevListRsp &inner_msg)
{
  PushMsg *push_msg = new PushMsg();
  push_msg->set_version(1);
  push_msg->set_type(GET_DEV_LIST_RSP);

  GetDevListRsp *rsp = new GetDevListRsp();
  rsp->set_code(0);
  int dev_info_count = inner_msg.info_list_.size();
  for (int i = 0; i < dev_info_count; ++i) {
    DeviceInfo *info = rsp->add_dev_infos();
    const ZZBDevInfo *zb_info = inner_msg.info_list_[i];
    info->set_addr(zb_info->addr_);
    info->set_type(zb_info->type_);
    info->set_name(zb_info->name_);
  }

  push_msg->set_allocated_get_dev_list_rsp(rsp);

  return push_msg;
}

PushMsg* inner2push(const ZInnerGetDevInfoRsp &inner_msg)
{
  PushMsg *push_msg = new PushMsg();
  push_msg->set_version(1);
  push_msg->set_type(GET_DEV_INFO_RSP);

  GetDevInfoRsp *rsp = new GetDevInfoRsp();
  rsp->set_code(0);
  int id_value_pair_count = inner_msg.dev_infos_.size();
  for (int i = 0; i < id_value_pair_count; ++i) {
    const ZItemPair &zb_pair = inner_msg.dev_infos_[i];
    IdValuePair *pair = rsp->add_id_value_pairs();
    pair->set_id(zb_pair.id);
    pair->set_value(zb_pair.val);
  }

  push_msg->set_allocated_get_dev_info_rsp(rsp);

  return push_msg;
}

PushMsg* inner2push(const ZInnerSetDevInfoRsp &inner_msg)
{
  PushMsg *push_msg = new PushMsg();
  push_msg->set_version(1);
  push_msg->set_type(SET_DEV_INFO_RSP);

  SetDevInfoRsp *rsp = new SetDevInfoRsp();
  rsp->set_code(inner_msg.status_);

  push_msg->set_allocated_set_dev_info_rsp(rsp);

  return push_msg;
}

PushMsg* inner2push(const ZInnerMsg &inner_msg)
{
  switch (inner_msg.msg_type_) {
    case Z_ZB_GET_DEV_LIST_RSP:
    {
      return inner2push((const ZInnerGetDevListRsp&)inner_msg);
    }
    case Z_ZB_GET_DEV_RSP:
    {
      return inner2push((const ZInnerGetDevInfoRsp&)inner_msg);
    }
    case Z_ZB_SET_DEV_RSP:
    {
      return inner2push((const ZInnerSetDevInfoRsp&)inner_msg);
    }
    default:
      return NULL;
  }
}





















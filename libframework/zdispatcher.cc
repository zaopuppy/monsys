#include "zdispatcher.h"

#include "zerrno.h"

ZDispatcher *ZDispatcher::instance_ = NULL;

int ZDispatcher::sendDirect(ZInnerMsg *msg)
{
  Z_LOG_D("ZDispatcher::sendDirect: %d", msg->msg_type_);
  ZInnerAddress dst_addr = msg->dst_addr_;

  // find module
  ZModule *module = findModule(dst_addr.module_type_);
  if (module == NULL) {
    Z_LOG_D("Target module [%d] is not found", dst_addr.module_type_);
    return FAIL;
  }

  // XXX: Don't forget to delete msg in sendMsg
  module->sendMsg(msg);

  return OK;
}

// // XXX: this mothod should not used currently, use message queue depends on
// // the implement of message loop, which will be a performance problem
// int ZDispatcher::sendMsg(ZInnerMsg *msg)
// {
//   if (!msg->src_addr_.isValid()) {
//     Z_LOG_D("Source address is invalid");
//     // delete msg;
//     // return -1;
//   }

//   msg_list_.push_back(msg);

//   return 0;
// }

int ZDispatcher::registerModule(ZModule *m)
{
  if (findModule(m->getType())) {
    Z_LOG_D("Duplicated module");
    return FAIL;
  }

  module_list_.push_back(m);

  // Z_LOG_D("Module(%d:%s), added",
  //    m->getType(), moduleType2string(m->getType()));

  return OK;
}

ZModule* ZDispatcher::findModule(int moduleType)
{
  MODULE_LIST_TYPE::iterator iter = module_list_.begin();
  ZModule *m = NULL;

  for (; iter != module_list_.end(); ++iter) {
    m = *iter;
    if (m->getType() == moduleType) {
      return m;
    }
  }

  return NULL;
}

// // TODO: very bad
// void ZDispatcher::consumeMsg()
// {
//   std::list<ZInnerMsg*>::iterator iter = msg_list_.begin();
//   ZInnerMsg *msg;
//   ZModule *module;
//   for (; iter != msg_list_.end(); ++iter) {
//     msg = *iter;
//     Z_LOG_D("consume: %d", msg->msg_type_);
//     Z_LOG_D("consume src: (module_type=%d, module_id=%d, handler_id=%d)",
//       msg->src_addr_.module_type_,
//       msg->src_addr_.module_id_,
//       msg->src_addr_.handler_id_);
//     Z_LOG_D("consume dst: (module_type=%d, module_id=%d, handler_id=%d)",
//       msg->dst_addr_.module_type_,
//       msg->dst_addr_.module_id_,
//       msg->dst_addr_.handler_id_);
//     ZInnerAddress &dst_addr = msg->dst_addr_;
//     // find module
//     module = findModule(dst_addr.module_type_);
//     if (module == NULL) {
//       Z_LOG_D("Target module [%d] is not found", dst_addr.module_type_);
//     } else {
//       module->sendMsg(msg);
//     }
//     delete *iter;
//   }
//   msg_list_.clear();
// }


void ZDispatcher::checkTimeout(long delta)
{
  // Z_LOG_D("ZDispatcher::checkTimeout");
}

void ZDispatcher::moduleRoutine(long delta)
{
  ZModule *module;
  MODULE_LIST_TYPE::iterator iter = module_list_.begin();
  for (; iter != module_list_.end(); ++iter) {
    module = *iter;
    module->routine(delta);
  }
}

void ZDispatcher::routine(long delta)
{
  // Z_LOG_D("ZDispatcher::routine(%ld)", delta);
  // consumeMsg();

  checkTimeout(delta);

  moduleRoutine(delta);
}


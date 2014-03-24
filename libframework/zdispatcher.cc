#include "zdispatcher.h"

#include "zerrno.h"

ZDispatcher *ZDispatcher::instance_ = NULL;

int ZDispatcher::sendDirect(ZInnerMsg *msg)
{
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

void ZDispatcher::checkTimeout(long delta)
{
  // Z_LOG_D("ZDispatcher::checkTimeout");

  // ZSession *session;

  // SESSION_CTRL_TYPE::iterator iter = session_ctrl_.begin();
  // SESSION_CTRL_TYPE::iterator tmp_iter;

  // while (iter != session_ctrl_.end()) {
  //  // Z_LOG_D("while loop");
  //  session = iter->second;
  //  session->doTimeout(delta);
  //  if (session->isComplete())  {
  //    // Z_LOG_D("delta: [%ld]", delta);
  //    // Z_LOG_D("timeout: [%ld]", session->getTimeout());
  //    tmp_iter = iter;
  //    ++iter;
  //    session_ctrl_.erase(tmp_iter);
  //  } else {
  //    ++iter;
  //  }
  // }
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


#ifndef _Z_ROUTE_H__
#define _Z_ROUTE_H__

#include "zframework.h"

typedef struct {
	int msg_type;
	ZInnerAddress src_addr;
	ZInnerAddress dst_addr;
} route_record_t;

// class ZRoute
// {
//  public:
//  	int sendMsg(ZInnerMsg *msg) {
// 		int msg_type;
// 
// 		ZSession *session = NULL;
// 
// 		// int module_type;
// 		// int handler_id;
// 		switch (msg_type) {
// 			case Z_ZB_GET_DEV_LIST_REQ:
// 			{
// 				session = new ZInnerForwardSession();
// 				{
// 					// session->init();
// 					// session->event(msg);
// 				}
// 				break;
// 			}
// 			default:
// 			{
// 				return FAIL;
// 			}
// 		}
// 
// 		return OK;
//  	}
// };

// const route_record_t g_webapi_route_records[] = {
// 	{},
// };

#endif // _Z_ROUTE_H__


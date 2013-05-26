#ifndef _Z_SESSION_CTRL_H__
#define _Z_SESSION_CTRL_H__

#include <map>
#include <stdint.h>


// T_Session
// {
//  key_;
//  seq_;
// }
template <typename T_Key, typename T_Session>
class ZSessionCtrl {
 public:
	ZSessionCtrl() {}

 public:
	typedef std::map<T_Key, T_Session*> MAP_TYPE;
	typedef typename std::map<T_Key, T_Session*>::iterator MAP_TYPE_ITERATOR;

 public:
	T_Session* find(const T_Key &key);
	bool add(const T_Key &key, T_Session *session);
	bool remove(const T_Key &key);

 private:
	MAP_TYPE session_map_;
};

template <typename T_Key,typename T_Session>
T_Session* ZSessionCtrl<T_Key, T_Session>::find(const T_Key &key)
{
	MAP_TYPE_ITERATOR iter = session_map_.find(key);
	if (iter == session_map_.end()) {
		return NULL;
	}

	return iter->second;
}

template <typename T_Key,typename T_Session>
bool ZSessionCtrl<T_Key, T_Session>::add(const T_Key &key, T_Session *session)
{
	std::pair<MAP_TYPE_ITERATOR, bool> ret =
		session_map_.insert(std::pair<T_Key, T_Session*>(key, session));
	return ret.second;
}

template <typename T_Key,typename T_Session>
bool ZSessionCtrl<T_Key, T_Session>::remove(const T_Key &key)
{
	MAP_TYPE_ITERATOR iter = session_map_.find(key);
	if (iter == session_map_.end()) {
		return false;
	}

	session_map_.erase(iter);

	return true;
}


#endif // _Z_SESSION_CTRL_H__



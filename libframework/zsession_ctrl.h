#ifndef _Z_SESSION_CTRL_H__
#define _Z_SESSION_CTRL_H__

#include <stdint.h>
#include <assert.h>
#include <map>
#include <vector>

#include "libbase/zlog.h"

////////////////////////////////////////////////////////////////////////////////////
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
  // typedef typename std::map<T_Key, T_Session*>::iterator MAP_TYPE_ITERATOR;
  typedef typename MAP_TYPE::iterator iterator;

 public:
  T_Session* find(const T_Key &key);
  bool add(const T_Key &key, T_Session *session);
  T_Session* remove(const T_Key &key);

  iterator begin() { return session_map_.begin(); }
  iterator end() { return session_map_.end(); }
  void erase(iterator iter) { session_map_.erase(iter); }

 private:
  MAP_TYPE session_map_;
};

//////////////////////////////////////////////////
template <typename T_Key,typename T_Session>
T_Session* ZSessionCtrl<T_Key, T_Session>::find(const T_Key &key)
{
  iterator iter = session_map_.find(key);
  if (iter == session_map_.end()) {
    return NULL;
  }

  return iter->second;
}

template <typename T_Key,typename T_Session>
bool ZSessionCtrl<T_Key, T_Session>::add(const T_Key &key, T_Session *session)
{
  std::pair<iterator, bool> ret =
    session_map_.insert(std::pair<T_Key, T_Session*>(key, session));
  return ret.second;
}

template <typename T_Key,typename T_Session>
T_Session* ZSessionCtrl<T_Key, T_Session>::remove(const T_Key &key)
{
  iterator iter = session_map_.find(key);
  if (iter == session_map_.end()) {
    return NULL;
  }

  T_Session *session = iter->second;
  session_map_.erase(iter);

  return session;
}

////////////////////////////////////////////////////////////////////////////////////
// template <typename T_Key1, typename T_Key2, typename T_Session, typename T_Map>
template <typename T_Key1, typename T_Key2, typename T_Session>
class ZSessionCtrl2Key {
 public:
  ZSessionCtrl2Key() {}

 public:
  typedef struct key_pair {
    T_Key1 key1;
    T_Key2 key2;
    key_pair(T_Key1 k1, T_Key2 k2) { key1 = k1; key2 = k2; }
  } key_pair_t;

  typedef std::map<T_Key1, T_Session*> MAP_TYPE;
  typedef typename MAP_TYPE::iterator iterator;

  // mapping from key1 to key2
  typedef std::vector<key_pair_t> KEY_MAP_TYPE;

 public:
  T_Session* findByKey1(const T_Key1 &key1) {
    iterator iter = session_map_.find(key1);
    if (iter == session_map_.end()) {
      return NULL;
    }

    return iter->second;
  }

  T_Session* findByKey2(const T_Key2 &key2) {
  // T_Session* findByKey2(const T_Key2 key2) {
    T_Key1 key1;
    if (!getKey1ByKey2(key2, key1)) {
      // Z_LOG_D("Failed to get key1 by using key2\n");
      return NULL;
    }

    return findByKey1(key1);
  }

  bool add(const T_Key1 key1, const T_Key2 key2, T_Session *session)
  {
    key_pair_t pair(key1, key2);
    // check if there's already one in there
    for (size_t i = 0; i < key_map_.size(); ++i) {
      if (key_map_[i].key1 == key1 || key_map_[i].key2 == key2) {
        // duplicated record
        Z_LOG_D("duplicated record at index %ld\n", i);
        return false;
      }
    }

    if (!session_map_.insert(
        std::pair<T_Key1, T_Session*>(key1, session)
      ).second) {
      return false;
    }

    key_map_.push_back(pair);

    return true;
  }

  T_Session* removeByKey1(const T_Key1 key1) {
    // for session map
    T_Session *session = NULL;
    {
      iterator iter = session_map_.find(key1);
      if (iter == session_map_.end()) {
        return NULL;
      }

      session = iter->second;
      session_map_.erase(iter);
    }

    // TODO: abstract a new method
    {
      bool found = false;
      typename KEY_MAP_TYPE::iterator iter = key_map_.begin();
      for (; iter != key_map_.end(); ++iter) {
        if ((*iter).key1 == key1) { // difference
          found = true;
          key_map_.erase(iter);
          break;
        }
      }

      assert(found);
    }

    return session;
  }
  T_Session* removeByKey2(const T_Key2 key2) {
    T_Key1 key1;
    if (!getKey1ByKey2(key2, key1)) {
      Z_LOG_D("Failed to get key1 by using key2\n");
      return NULL;
    }

    return removeByKey1(key1);
  }

  bool getKey1ByKey2(const T_Key2 &key2, T_Key1 &key1) {
    for (size_t i = 0; i < key_map_.size(); ++i) {
      if (key_map_[i].key2 == key2) {
        key1 = key_map_[i].key1;
        return true;
      }
    }

    return false;
  }

  bool getKey2ByKey1(const T_Key1 &key1, T_Key2 &key2) {
    for (size_t i = 0; i < key_map_.size(); ++i) {
      if (key_map_[i].key1 == key1) {
        key2 = key_map_[i].key2;
        return true;
      }
    }

    return false;     
  }


  iterator begin() { return session_map_.begin(); }
  iterator end() { return session_map_.end(); }
  void erase(iterator iter) {
    {
      bool found = false;
      typename KEY_MAP_TYPE::iterator tmp_iter = key_map_.begin();
      for (; tmp_iter != key_map_.end(); ++tmp_iter) {
        if ((*tmp_iter).key1 == iter->first) {  // difference
          found = true;
          key_map_.erase(tmp_iter);
          break;
        }
      }

      assert(found);
    }

    session_map_.erase(iter);
  }

 private:
  MAP_TYPE session_map_;
  KEY_MAP_TYPE key_map_;
  // T_Map map_obj_;
};


#endif // _Z_SESSION_CTRL_H__



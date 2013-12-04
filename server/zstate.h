#ifndef _Z_STATE_H__
#define _Z_STATE_H__

class ZState
{
 public:
  virtual int init() = 0;
  virtual int process() = 0;
};

class ZStateMachine
{
 public:
};

#endif // _Z_STATE_H__


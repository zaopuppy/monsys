#ifndef _Z_STATE_H__
#define _Z_STATE_H__

class ZState
{
 public:
  virtual void init() = 0;
  virtual void process() = 0;
};

class ZStateMachine
{
 public:
};

#endif // _Z_STATE_H__


#ifndef _RECYCLE_ATEXIT_H_
#define _RECYCLE_ATEXIT_H_

#include<cstddef>

namespace ratexit {
template<class T, unsigned short N>
struct recycle_atexit {
  T *_todelete[N];
  unsigned short _signedup;
  
  recycle_atexit()
  {
    _signedup = 0;
    for (auto i(0); i < N; ++i)
      _todelete[i] = nullptr;
  }
  ~recycle_atexit()
  {
    for (auto i(0); i < N; ++i)
      delete _todelete[i];
  }

  int addObjToRecycle(T *p)
  {
    if (_signedup < N)
      _todelete[_signedup++] = p;
    else
      return -1;

    return 0;
  }

};

}


#endif

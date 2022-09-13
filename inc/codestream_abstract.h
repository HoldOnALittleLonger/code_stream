

#ifndef _CODESTREAM_ABSTRACT_H_
#define _CODESTREAM_ABSTRACT_H_

#include<functional>
#include<vector>
#include<mutex>
#include<bitset>
#include<cstddef>

#define VECTOR_SIZE 10

namespace codestream {

  using aindex = size_t;

#define opci aindex;
#define opip aindex;

  enum {INIT = 0,
	OP_ORDER,
	STOPPED
  };
	

  class Codestream {

  private:
    std::vector<std::function<void *(void *)>> _code_procedures;
    aindex _cp_end;
    std::bitset<3> _codestream_flag;
    /* if INIT == 0, _cp_end AND _ip AND _last_install AND _ip_end will are useless */ 

    aindex _ip;
    std::mutex _ip_mutex;
    std::unique_lock _ip_mutex_unique;
    aindex _ip_end;

    void *_last_op_ret;

    aindex _last_install;

  public:

    Codestream();
    ~Codestream();

    int startCode(void *vec);
    int stopCode(void);
    int restartCode(void);

    void installProcedure(std::function<void *(void *)> &&f);
    short uninstallProcedure(aindex which);

    opip getProgress(void);
    void *getLastResult(void);
    opci getOpChainSize(void);


  };









}






#endif



#ifndef _CODESTREAM_ABSTRACT_H_
#define _CODESTREAM_ABSTRACT_H_

#include<functional>
#include<vector>
#include<mutex>
#include<bitset>
#include<cstddef>
#include<string>
#include<cstdbool>

#define VECTOR_SIZE 1

namespace codestream {

  using aindex = size_t;
  using opip = short;
  using opci = aindex;

  enum codestream_process_state {
    INVAILD,
    NOERROR,
    NOINIT,
    IN_PROGRESSING,
    SHUTDOWN,
    UNINSTALL_FAILED,
    UNINSTALL_EXCEED,
    SUSPEND,
    SUSPEND_FAILED,
    NOT_SUSPEND
  };


  class Codestream {

  private:

    enum {INIT = 0,
	  OP_ORDER,
	  STOPPED,
	  CODESTREAM_FLAG_TOTAL
    };

    std::vector<std::function<void *(void *)>> _code_procedures;
    aindex _cp_end;
    std::bitset<CODESTREAM_FLAG_TOTAL> _codestream_flag;
    /* if INIT == 0, _cp_end AND _ip AND _last_install AND _ip_end will are useless */ 

    short _ip;
    std::mutex _ip_mutex;
    aindex _ip_start;
    aindex _ip_end;

    void *_last_op_ret;

    codestream_process_state _state;
    std::mutex _state_mutex;

    void lock_ip(void) { _ip_mutex.lock(); }
    void unlock_ip(void) { _ip_mutex.unlock(); }

    void lock_state(void) { _state_mutex.lock(); }
    void unlock_state(void) { _state_mutex.unlock(); }

  public:

    Codestream();
    ~Codestream();

    codestream_process_state startCode(void *vec);
    codestream_process_state stopCode(void);
    codestream_process_state restartCode(void);

    void installProcedure(std::function<void *(void *)> &&f);
    void installProcedure(std::function<void *(void *)> &&f, aindex pos);
    codestream_process_state uninstallProcedure(aindex which);

    opip getProgress(void);
    void *getLastResult(void);
    opci getOpChainSize(void);

    std::string processStateExplain(void);

    void triggerOpOrder(void) {
      lock_ip();

      if (_codestream_flag[OP_ORDER])
	_codestream_flag.reset(OP_ORDER);
      else
	_codestream_flag.set(OP_ORDER);

      unlock_ip();
    }

    bool is_processing(void);
    bool is_suspend(void);
    bool is_shutdown(void);
    bool is_execsuccess(void);


  };



}






#endif

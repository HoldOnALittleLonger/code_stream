#ifndef _CODESTREAM_ABSTRACT_H_
#define _CODESTREAM_ABSTRACT_H_

#include<functional>
#include<vector>
#include<mutex>
#include<bitset>
#include<cstddef>
#include<string>
#include<cstdbool>
#include<thread>
#include<condition_variable>
#include<exception>

namespace codestream {

  using aindex = size_t;
  using opip = short;
  using opci = aindex;

  class Codestream {

  private:

    std::vector<std::function<void *(void *)>> _code_procedures;
    aindex _cp_end;

    // _cp_end would be changed only the time at install procedure
    // so,can use _ip_mutex to save it.
    // because _ip_mutex would as index for vector


    enum codestream_flag {
      FLAG_INIT,
      FLAG_OPDIRECTION,	// 0 -> ltor, 1 -> rtol
      FLAG_TAIL
    };
    std::bitset<FLAG_TAIL> _codestream_flag;

    enum codestream_error {
      NOERROR,
      ERROR_NOINIT,
      ERROR_INSTALLPROCFAILED,
      ERROR_UNINSTALLPROCFAILED,
      ERROR_SUSPENDFAILED,
      ERROR_RECOVERFAILED
    };
    codestream_error _cerror;
    std::mutex _flag_error_mutex;
    void lock_foe(void) { _flag_error_mutex.lock(); }
    void unlock_foe(void) { _flag_error_mutex.unlock(); }

    enum codestream_state {
      CODESTREAM_PROGRESSING,
      CODESTREAM_SUSPEND,
      CODESTREAM_SHUTDOWN,
      CODESTREAM_ERROR
    };
    codestream_state _state;
    codestream_state _state_when_error_occur;
    std::mutex _state_mutex;
    void lock_state(void) { _state_mutex.lock(); }
    void unlock_state(void) { _state_mutex.unlock(); }

    short _ip;
    short _last_ip_start;
    std::mutex _ip_mutex;
    void lock_ip(void) { _ip_mutex.lock(); }
    void unlock_ip(void) { _ip_mutex.unlock(); }

    aindex _ip_start;
    aindex _ip_end;
    // for to access _ip<...>,must lock _ip_mutex

    void *_last_op_ret;
    // for to access _last_op_ret,must lock _ip_mutex

    void startCode(void *vec);
    std::condition_variable _work_condition;

    void resetCodestream(void);

    // If want to lock _flag_error_mutex, _ip_mutex, _state_mutex,
    // have to lock them in the order is :
    //   _state_mutex -> _ip_mutex -> _flag_error_mutex
    // unlock them in opposite order.

    std::condition_variable _state_migrated_condition;
    void notifyStateMigrated(void) { _state_migrated_condition.notify_one(); }

  public:

    Codestream();
    ~Codestream();


    void coding(void *vec);	// a thread wrapper for startCode()
    void stopCode(void);
    void restartCode(void);
    void setStartPoint(aindex i);
    void installProcedure(std::function<void *(void *)> &&f);
    void installProcedure(std::function<void *(void *)> &&f, aindex pos);
    void uninstallProcedure(aindex which);

    opip getProgress(void);
    void *getLastResult(void);
    opci getOpChainSize(void);

    std::string processStateExplain(void);
    std::string processErrorExplain(void);

    void programErrorRecover(void) noexcept(false);

    void toggleOpDirection(void);
    bool is_processing(void);
    bool is_suspend(void);
    bool is_shutdown(void);
    bool is_execsuccess(void);

    void waitForStateMove(void);


  };



}






#endif

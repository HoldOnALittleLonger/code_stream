#include"codestream_abstract.h"

namespace codestream {

    /* This is local lock macro for RAII */
#define LOCAL_UNIQUE_MUTEX_NAME __lum
#define LOCAL_UNIQUE_MUTEX(n) LOCAL_UNIQUE_MUTEX_NAME##n

    /* cant lock the mutex when enter matching zone */

#define LOCAL_FLAG_ERROR_MUTEX LOCAL_UNIQUE_MUTEX(0)
#define enter_foe_save_zone			\
    {						\
    std::unique_lock<std::mutex> LOCAL_FLAG_ERROR_MUTEX(_flag_error_mutex)
#define leave_foe_save_zone			\
    }

#define LOCAL_IP_MUTEX LOCAL_UNIQUE_MUTEX(1)
#define enter_ip_save_zone			\
    {						\
    std::unique_lock<std::mutex> LOCAL_IP_MUTEX(_ip_mutex)
#define leave_ip_save_zone			\
    }

#define LOCAL_STATE_MUTEX LOCAL_UNIQUE_MUTEX(2)
#define enter_state_save_zone					\
      {								\
      std::unique_lock<std::mutex> LOCAL_STATE_MUTEX(_state_mutex)
#define leave_state_save_zone			\
    }

/* while use coarser-grained lock,neednt lock __locker manually. */
/* lock would be released after exit local environment */
/* dont use them whith zone-savers were defined over there */
#define local_coarser_granularity_lock(__locker)		\ 
  std::unique_lock<std::mutex> LOCAL_UNIQUE_MUTEX(__locker##32)(__locker)


  Codestream::Codestream()
  {
    /* default constructor */

    enter_foe_save_zone;
    _codestream_flag.reset();
    _cerror = NOERROR;
    leave_foe_save_zone;

    enter_ip_save_zone;
    _cp_end = 0;
    _last_ip_start = _ip = 0;
    _ip_start = 0;
    _ip_end = 0;
    _last_op_ret = nullptr;
    leave_ip_save_zone;

    enter_state_save_zone;
    _state = CODESTREAM_SHUTDOWN;
    _state_when_error_occur = _state;
    leave_state_save_zone;

  }

  Codestream::~Codestream()
  {
    enter_ip_save_zone;
    _code_procedures.clear();
    leave_ip_save_zone;
  }
  //  getProgress - return progressing for code system
  opip Codestream::getProgress(void)
  {
    local_coarser_granularity_lock(_ip_mutex);
    return _ip;
  }

  //  getLastResult - return the latest result made by procedure
  void *Codestream::getLastResult(void)
  {
    local_coarser_granularity_lock(_state_mutex);
    return _last_op_ret;
  }
  
  //  getOpChainSize - return size of operations were installed
  opci Codestream::getOpChainSize(void)
  {
    local_coarser_granularity_lock(_ip_mutex);
    return _cp_end;
  }

  //  installProcedure - install a procedure into op-chain.
  //    # install methods would not use ERROR CODE.(even the code was defined)
  //    @f : function object.
  void Codestream::installProcedure(std::function<void *(void *)> &&f)
  {
    enter_ip_save_zone;
    _code_procedures.emplace_back(f);
    _ip_end = _cp_end = _code_procedures.size();        // update pos records.
    leave_ip_save_zone;

    /* FLAG_INIT would be true after a procedure was installed. */
    enter_foe_save_zone;
    if (!_codestream_flag.test(FLAG_INIT))
      _codestream_flag.set(FLAG_INIT);
    leave_foe_save_zone;

  }

  //  installProcedure - overload version this can special position of chain to install.
  //    @f : function object.
  //    @pos : position to install.
  void Codestream::installProcedure(std::function<void *(void *)> &&f, aindex pos)
  {
    enter_ip_save_zone;
    auto vector_it(_code_procedures.begin());
    vector_it += ((pos == 0) ? 0 : pos - 1);	// -1 cant be vector index
    _code_procedures.emplace(vector_it, f);
    _ip_end = _cp_end = _code_procedures.size();    
    leave_ip_save_zone;

    enter_foe_save_zone;
    if (!_codestream_flag.test(FLAG_INIT))
      _codestream_flag.set(FLAG_INIT);
    leave_foe_save_zone;

  }

  //  uninstallProcedure - uninstall a procedure from chain.
  //    # method would not recycle memory for it,just set to NULL as well.
  //    @which : the postion of procedure which will be uninstalled.
  void Codestream::uninstallProcedure(aindex which)
  {
    enter_ip_save_zone;
    if (which < _cp_end)
      if (_code_procedures.at(which) == nullptr) {
	enter_foe_save_zone;
	_cerror = ERROR_UNINSTALLPROCFAILED;
	leave_foe_save_zone;
	enter_state_save_zone;
	_state_when_error_occur = _state;
	_state = CODESTREAM_ERROR;
	notifyStateMigrated();
	leave_state_save_zone;
      } else {
	auto f = _code_procedures[which];
	f = nullptr;
	_code_procedures[which] = f;	// use nullptr means the element is in useless state.
      }
    else {
      enter_foe_save_zone;
      _cerror = ERROR_UNINSTALLPROCFAILED;
      leave_foe_save_zone;
      enter_state_save_zone;
      _state_when_error_occur = _state;
      _state = CODESTREAM_ERROR;
      notifyStateMigrated();
      leave_state_save_zone;
    }
    leave_ip_save_zone;

  }

  //  setStartPoint - specify where to start
  //    @i : position to start
  void Codestream::setStartPoint(aindex i)
  {
    local_coarser_granularity_lock(_ip_mutex);
    if (i >= _ip_start && i < _ip_end)
      _last_ip_start = _ip = i;
  }

  //  resetCodestream - reset system states.
  void Codestream::resetCodestream(void)
  {
    local_coarser_granularity_lock(_state_mutex);
    local_coarser_granularity_lock(_ip_mutex);
    local_coarser_granularity_lock(_flag_error_mutex);
    _state_when_error_occur = _state = CODESTREAM_SHUTDOWN;
    notifyStateMigrated();
    _cerror = NOERROR;
    _ip = _last_ip_start;
  }

  //  coding - outside interface to start coding.
  //    @vec : vec should be a data buffer which would used by procedures.
  //    !! method will reset system at each time !!
  void Codestream::coding(void *vec)
  {
    resetCodestream();

    /* codestream have to init */
    enter_foe_save_zone;
    if (!_codestream_flag.test(FLAG_INIT)) {
      _cerror = ERROR_NOINIT;
      enter_state_save_zone;
      _state_when_error_occur = _state;
      _state = CODESTREAM_ERROR;
      notifyStateMigrated();
      leave_state_save_zone;
      return;
    }
    leave_foe_save_zone;

    enter_state_save_zone;
    _state_when_error_occur = _state = CODESTREAM_PROGRESSING;
    notifyStateMigrated();
    leave_state_save_zone;

    std::thread worker1(&Codestream::startCode, this, vec);
    worker1.detach();	/* after detach,cant invoke join() to it. */
  }


  //  startCode - main coding method.
  //    @vec : data buffer which would be used by procedures.
  void Codestream::startCode(void *vec)
  {
    std::function<void *(void *)> f(nullptr);
    void *ret_of_f(vec);

    // like a signal system
    do {
      f = nullptr;

    work_condition_check:
      _state_mutex.lock();
      if (_state == CODESTREAM_SUSPEND || _state == CODESTREAM_ERROR) {
	_state_mutex.unlock();

	/* condition_variable needs a RAII wrapper for std::mutex */
	/* it's the type of std::unique_lock */
	std::mutex condition_mutex;
	std::unique_lock<std::mutex> LOCAL_UNIQUE_MUTEX(cond)(condition_mutex);
	_work_condition.wait(LOCAL_UNIQUE_MUTEX(cond));

	goto work_condition_check;
      } else if (_state == CODESTREAM_SHUTDOWN) {
	_state_mutex.unlock();
	break;
      }
      _state_mutex.unlock();
	
      enter_ip_save_zone;
      enter_foe_save_zone;
      if ((!_codestream_flag[FLAG_OPDIRECTION] && (_ip < _ip_end))
	  || (_codestream_flag[FLAG_OPDIRECTION] && (_ip >= _ip_start))) {
	f = _code_procedures.at(_ip);
	_ip = (!_codestream_flag[FLAG_OPDIRECTION]) ? _ip + 1 : _ip - 1;        // set direction.
      } else {
	enter_state_save_zone;
	_state = CODESTREAM_SHUTDOWN;
	notifyStateMigrated();
	leave_state_save_zone;
      }

      leave_foe_save_zone;
      leave_ip_save_zone;

      if (f != nullptr) {
	ret_of_f = f(ret_of_f);
	enter_ip_save_zone;
	_last_op_ret = ret_of_f;
	leave_ip_save_zone;
      }

    } while (1);

  }

  //  stopCode - stop coding.
  void Codestream::stopCode(void)
  {
    enter_foe_save_zone;
    if (!_codestream_flag.test(FLAG_INIT)) {
      _cerror = ERROR_NOINIT;
      enter_state_save_zone;
      _state_when_error_occur = _state;
      _state = CODESTREAM_ERROR;
      notifyStateMigrated();
      leave_state_save_zone;
      return;
    }
    leave_foe_save_zone;

    enter_ip_save_zone;
    if (_ip >= _ip_end) {        // if _ip > _ip_end, there would no residue procedure have to exec.
      enter_foe_save_zone;
      _cerror = ERROR_SUSPENDFAILED;
      leave_foe_save_zone;
      enter_state_save_zone;
      _state_when_error_occur = _state;
      _state = CODESTREAM_ERROR;
      notifyStateMigrated();
      leave_state_save_zone;
      return;
    }
    leave_ip_save_zone;

    if (this->is_processing()) {	// try to stop coding
      enter_state_save_zone;
      _state = CODESTREAM_SUSPEND;
      notifyStateMigrated();
      leave_state_save_zone;
    }
  }

  // should use stopCode() and restartCode() as a commands sequence
  // a1 a2 <stopCode()> a3 a4 <restartCode()> ...
  // and cant invoke get<Function> between them.

  //  restartCode - restart system after stopped.
  void Codestream::restartCode(void)
  {
    /* check if program had init */
    enter_foe_save_zone;
    if (!_codestream_flag.test(FLAG_INIT)) {
      _cerror = ERROR_NOINIT;
      enter_state_save_zone;
      _state_when_error_occur = _state;
      _state = CODESTREAM_ERROR;
      notifyStateMigrated();
      leave_state_save_zone;
      return;
    }
    leave_foe_save_zone;

    enter_state_save_zone;
    if (_state == CODESTREAM_SUSPEND) {
      _state = CODESTREAM_PROGRESSING;
      notifyStateMigrated();
      _work_condition.notify_one();
    } else {
      _state_when_error_occur = _state;
      _state = CODESTREAM_ERROR;
      notifyStateMigrated();
      enter_foe_save_zone;
      _cerror = ERROR_RECOVERFAILED;
      leave_foe_save_zone;
    }
    leave_state_save_zone;
  }

  //  toggleOpDirection - switch direction while coding.
  void Codestream::toggleOpDirection(void)
  {
      local_coarser_granularity_lock(_flag_error_mutex);
      if (_codestream_flag.test(FLAG_OPDIRECTION))
	_codestream_flag.reset(FLAG_OPDIRECTION);
      else
	_codestream_flag.set(FLAG_OPDIRECTION);
  }
  
  //  is_processing - return true if system is coding,return false if it is not.
  bool Codestream::is_processing(void)
  {
    local_coarser_granularity_lock(_state_mutex);
    return _state == CODESTREAM_PROGRESSING;
  }

  //  is_suspend - return true if system is suspended,return false if it is not.
  bool Codestream::is_suspend(void)
  {
    local_coarser_granularity_lock(_state_mutex);
    return _state == CODESTREAM_SUSPEND;
  }

  //  is_shutdown - return true if system is shutdown,return false if it is not.
  bool Codestream::is_shutdown(void)
  {
    local_coarser_granularity_lock(_state_mutex);
    return _state == CODESTREAM_SHUTDOWN;
  }

  //  is_execsuccess - return true if latest exec was succeed,return false if it was not.
  bool Codestream::is_execsuccess(void)
  {
    local_coarser_granularity_lock(_flag_error_mutex);
    return _cerror == NOERROR;
  }

  //  processStateExplain - this method used to covert status to a explain text.
  std::string Codestream::processStateExplain(void)
  {
    local_coarser_granularity_lock(_state_mutex);
    std::string msg("0000");
    switch (_state) {
    case CODESTREAM_PROGRESSING:
      msg.clear();
      msg = "codestream: progressing...";
      break;

    case CODESTREAM_SUSPEND:
      msg.clear();
      msg = "codestream: process suspended.";
      break;

    case CODESTREAM_SHUTDOWN:
      msg.clear();
      msg = "codestream: process shutdown.";
      break;

    case CODESTREAM_ERROR:
      msg.clear();
      msg = "codestream: in trap,must to deal with the problem before start work.";
      break;

    default:
      msg.clear();
      msg = "codestream: unknown state,program error.";
      break;

    }

    return msg;
  }

  //  processErrorExplain - this method used to covert error code to a explain text.
  std::string Codestream::processErrorExplain(void)
  {
    local_coarser_granularity_lock(_flag_error_mutex);
    std::string msg("0000");

    switch (_cerror) {
    case NOERROR:
      msg.clear();
      msg = "codestream error: working fine,no error occurred.";
      break;

    case ERROR_NOINIT:
      msg.clear();
      msg = "codestream error: program had not init,this may be a programing error.";
      break;

    case ERROR_INSTALLPROCFAILED:
      msg.clear();
      msg = "codestream error: install procedure failed.";
      break;

    case ERROR_UNINSTALLPROCFAILED:
      msg.clear();
      msg = "codestream error: uninstall procedure failed.";
      break;

    case ERROR_SUSPENDFAILED:
      msg.clear();
      msg = "codestream error: suspend process failed.may be process shutdown.";
      break;

    case ERROR_RECOVERFAILED:
      msg.clear();
      msg = "codestream error: recover process failed.may be process wasnt suspended or shutdown.";
      break;

    default:
      msg.clear();
      msg = "codestream error: unknown error,this is a programming error.";
      break;
    }

    return msg;
  }

  //  programErrorRecover - try to recover system after it was strunk.
  void Codestream::programErrorRecover(void) noexcept(false)
  {
    local_coarser_granularity_lock(_state_mutex);
    local_coarser_granularity_lock(_ip_mutex);
    local_coarser_granularity_lock(_flag_error_mutex);

    switch (_state) {
    case CODESTREAM_ERROR:
      switch (_cerror) {
      case NOERROR:
	/* in state but no error number,this is a trap */
	/* in this case,should throw a exception */
	throw std::string{"exception: system is ERROR state now,but nothing of error number was setted."};

	/* These situations,just retry operations */

      case ERROR_NOINIT:
	_state_when_error_occur = _state = CODESTREAM_SHUTDOWN;
	notifyStateMigrated();
	_cerror = NOERROR;
	break;


	/* if failed in install or uninstall,should suspend system */
      case ERROR_INSTALLPROCFAILED:
	_state = CODESTREAM_SUSPEND;
	notifyStateMigrated();
	_cerror = NOERROR;
	break;

      case ERROR_UNINSTALLPROCFAILED:
	_state = CODESTREAM_SUSPEND;
	notifyStateMigrated();
	_cerror = NOERROR;
	break;

	/* suspendfailed and recoverfailed,may be system not in a properly state */
      case ERROR_SUSPENDFAILED:
	_state = _state_when_error_occur;
	notifyStateMigrated();
	_cerror = NOERROR;
	break;

      case ERROR_RECOVERFAILED:
	_state = _state_when_error_occur;
	notifyStateMigrated();
	_cerror = NOERROR;
	break;

      default:
	throw std::string{"exception: system is ERROR state now,but has a unknown error."};
      }

    default:;	/* do nothing */
    }
  }

  //  waitForStateMove - outside interface for event driving.
  void Codestream::waitForStateMove(void)
  {
    std::mutex LOCAL_UNIQUE_MUTEX(_waitforstatemove);
    std::unique_lock<std::mutex> LOCAL_UNIQUE_MUTEX(_wfsm_unique)(LOCAL_UNIQUE_MUTEX(_waitforstatemove));
    _state_migrated_condition.wait(LOCAL_UNIQUE_MUTEX(_wfsm_unique));	// wait notification
  }

}
